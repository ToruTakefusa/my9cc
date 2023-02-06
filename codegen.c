#include "9cc.h"

int labelCount = 0;

void gen_lval(Node *node) {
    if (node->kind != ND_LVAR)
        error("代入の左辺値が変数ではありません");

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node->offset);
    printf("    push rax\n");
}

void gen(Node *node) {
    if (!node) return;

    int elseCount = 0;
    int beginCount = 0;
    int endCount = 0;

    if (node->stmt) {
        for (int i = 0; i < node->stmt->length; i++) {
            gen(getItem(node->stmt, i));
            return;
        }
    }

    switch (node->kind) {
        case ND_NUM:
            printf("    push %d\n", node->val);
            return;
        case ND_LVAR:
            gen_lval(node);
            printf("    pop rax\n");
            printf("    mov rax, [rax]\n");
            printf("    push rax\n");
            return;
        case ND_ASSIGN:
            for (int i = 0; i < node->lhs->length; i++) {
                gen_lval(getItem(node->lhs, i));
            }
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
            printf("    mov [rax], rdi\n");
            printf("    push rdi\n");
            return;
        case ND_RETURN:
            for (int i = 0; i < node->lhs->length; i++) {
                gen(getItem(node->lhs, i));
            }
            printf("    pop rax\n");
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
            return;
        case ND_IF:
            elseCount = labelCount;
            endCount = labelCount;
            labelCount++;
            for (int i = 0; i < node->lhs->length; i++) {
                gen(getItem(node->lhs, i));
            }
            printf("    pop  rax\n");
            printf("    cmp rax, 0\n");
            if (node->els) {
                printf("    je    .Lelse%d\n", elseCount);
            } else {
                // elseがない場合
                printf("    je   .Lend%d\n", endCount);
            }
            gen(node->rhs);
            if (node->els) {
                printf("    jmp    .Lend%d\n", endCount);
                printf(".Lelse%d:\n", elseCount);
                gen(node->els);
            }
            printf(".Lend%d:\n", endCount);
            return;
        case ND_WHILE:
            beginCount = labelCount;
            endCount = labelCount;
            labelCount++;
            printf(".Lbegin%d:\n", beginCount);
            for (int i = 0; i < node->lhs->length; i++) {
                gen(getItem(node->lhs, i));
            }
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", endCount);
            gen(node->rhs);
            printf("    jmp .Lbegin%d\n", beginCount);
            printf(".Lend%d:\n", endCount);
            return;
        case ND_FOR:
            // 節がなかった場合のことを考える必要がある。
            beginCount = labelCount;
            endCount = labelCount;
            labelCount++;
            if (node->init) {
                gen(node->init);
            }
            printf(".Lbegin%d:\n", beginCount);
            if (node->cond) {
                gen(node->cond);
            }
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", endCount);
            for (int i = 0; i < node->lhs->length; i++) {
                gen(getItem(node->lhs, i));
            }
            if (node->loop) {
                gen(node->loop);
            }
            printf("    jmp .Lbegin%d\n", beginCount);
            printf(".Lend%d:\n", endCount);
            return;
    }

    for (int i = 0; i < node->lhs->length; i++) {
        gen(getItem(node->lhs, i));
    }
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
