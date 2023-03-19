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
    int count = 0;
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
            gen_lval(node->lhs);
            gen(node->rhs);

            printf("    pop rdi\n");
            printf("    pop rax\n");
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
            printf("    jmp .Lreturn\n");
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
