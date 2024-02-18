#include "9cc.h"

int labelCount = 0;
int lastRet = 0;
// 関数呼び出し時に、引数を格納するレジスタ
char* argRegister[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

/*
 * 変数が格納されているアドレスを、スタックに格納する。
 */
void gen_lval(Node *node) {

    if (ND_LVAR == node->kind) {
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n", node->offset);
        printf("    push rax\n");
    } else if (ND_DEREF == node->kind) {
        gen(node->lhs);
    }

}

/**
 * ローカル変数のバイト数を計算する。
 * @param node 対象Node
 * @return nodeがnullもしくは、ローカル変数が存在しない場合は0。それ以外は、ローカル変数のバイト数。
 */
size_t get_variable_size(Node *node) {
    if (!node) return 0;
    if (!node->locals) return 0;

    size_t sum = 0;
    LVar *var = node->locals;
    while(var) {
        if (INT == var->type->ty) {
            sum += 8;
        } else if (ARRAY == var->type->ty || PTR == var->type->ty) {
            // 扱える型はint, ptr, arrayで、一律8byteのため
            sum += 8 * (var->type->array_size);
        }

        var = var->next;
    }

    return sum;
}

void gen(Node *node) {
    int count = 0;
    switch (node->kind) {
        case ND_NUM:
            printf("    push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("    pop rax\n");
            // 変数が格納されているアドレスから、値をロードする。
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
            // 変数が格納されているアドレスに、値をセーブする。
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
        case ND_IF:
            count = labelCount++;
            gen(node->cond);
            printf("    pop  rax\n");
            printf("    cmp rax, 0\n");
            if (node->els) {
                printf("    je    .Lelse%d\n", count);
            } else {
                // elseがない場合
                printf("    je   .Lend%d\n", count);
            }
            gen(node->then);
            if (node->els) {
                printf("    jmp    .Lend%d\n", count);
                printf(".Lelse%d:\n", count);
                gen(node->els);
            }
            printf(".Lend%d:\n", count);
            return;
        case ND_WHILE:
            count = labelCount++;
            printf(".Lbegin%d:\n", count);
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", count);
            gen(node->then);
            printf("    jmp .Lbegin%d\n", count);
            printf(".Lend%d:\n", count);
            return;
        case ND_FOR:
            count = labelCount++;
            if (node->init) {
                gen(node->init);
            }
            printf(".Lbegin%d:\n", count);
            if (node->cond) {
                gen(node->cond);
                printf("    pop rax\n");
                printf("    cmp rax, 0\n");
                printf("    je .Lend%d\n", count);
            }
            gen(node->then);
            if (node->loop) {
                gen(node->loop);
            }
            printf("    jmp .Lbegin%d\n", count);
            printf(".Lend%d:\n", count);
            return;
        case ND_BLOCK:
            for (int i = 0; i < node->stmt->length; i++) {
                gen(getItem(node->stmt, i));
            }
            return;
        case ND_RETURN:
            gen(node->lhs);
            printf("    pop rax\n");
            printf("    jmp .Lreturn%d\n", lastRet);
            return;
        case ND_FUNCTION_CALL:
            count = labelCount++;
            if (node->args) {
                // 引数が存在する場合
                for (int i = 0; i < node->args->length; i++) {
                    gen(getItem(node->args,i));
                    printf("    pop %s\n", argRegister[i]);
                }
            }
            printf("    mov rax, rsp\n");
            // RSPが16の倍数かチェック
            printf("    and rax, 15\n");
            printf("    jnz .Lfunc%d\n", count);
            // RSPが16の倍数の場合
            printf("    mov rax, 0\n");
            printf("    call %s\n", node->name);
            printf("    jmp .Lend%d\n", count);
            // RSPが16の倍数以外の場合
            printf(".Lfunc%d:\n", count);
            printf("    sub rsp, 8\n");
            // RSPが16の倍数になるよう調整(8byteごとしか変動しない)
            printf("    mov rax, 0\n");
            printf("    call %s\n", node->name);
            printf("    add rsp, 8\n");
            printf(".Lend%d:\n", count);
            printf("    push rax\n");
            return;
        case ND_FUNCTION_DEF:
            // 関数名は、グローバルスコープにしておく。(関数名はユニークという前提)
            printf("%s:\n", node->name);

            // プロローグ
            // 変数の個数分、領域を確保する
            printf("    push rbp\n");
            printf("    mov rbp, rsp\n");
            printf("    sub rsp, %zu\n", get_variable_size(node));

            // node->argsの個数と、引数の値が格納されているレジスタの個数は一致している
            for (int i = 0; i < node->args->length; i++) {
                Node *arg = getItem(node->args, i);
                printf("    mov rax, rbp\n");
                printf("    sub rax, %d\n", arg->offset);
                printf("    mov [rax], %s\n", argRegister[i]);
            }

            lastRet = labelCount++;

            for (int i = 0; i < node->stmt->length; i++) {
                gen(getItem(node->stmt, i));
            }

            // エピローグ
            // 最後の式の結果がRAXに残っているのでそれが返り値になる
            printf(".Lreturn%d:\n", lastRet);
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
        case ND_ADDR:
            gen_lval(node->lhs);
            return;
        case ND_DEREF:
            gen(node->lhs);
            // 左辺値として評価
            printf("  pop rax\n");
            printf("  mov rax, [rax]\n");
            printf("  push rax\n");
            return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind) {
        case ND_ADD:
            printf("    add rax, rdi\n");
            break;
        case ND_SUB:
            printf("    sub rax, rdi\n");
            break;
        case ND_MUL:
            printf("    imul rax, rdi\n");
            break;
        case ND_DIV:
            printf("    cqo\n");
            printf("    idiv rdi\n");
            break;
        case ND_EQ:
            printf("    cmp rax, rdi\n");
            printf("    sete al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LT:
            printf("    cmp rax, rdi\n");
            printf("    setl al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_LE:
            printf("    cmp rax, rdi\n");
            printf("    setle al\n");
            printf("    movzb rax, al\n");
            break;
        case ND_NE:
            printf("    cmp rax, rdi\n");
            printf("    setne al\n");
            printf("    movzb rax, al\n");
            break;
    }
    printf("    push rax\n");
}
