//
// Created by toru on 23/01/31.
//
#include "9cc.h"

void printNode(Node *node) {
    printf("Nodeを出力します。\n");
    if (!node) {
        printf("node is Null\n");
        return;
    }
    printf("Nodeの種別は%sです\n", printNodeKind(node->kind));

    if (node->stmt) {
        printf("stmtの長さは%dです。\n", node->stmt->length);
        printf("stmtの中身を出力\n");
        for (int i = 0; i < node->stmt->length; i++) {
            Node *node1 = getItem(node->stmt, i);
            printNode(node1);
        }
    }
    if (node->kind == ND_NUM) {
        printf("数値の値は、%dです。\n", node->val);
    }

    if (node->kind == ND_FUNCTION_CALL) {
        printf("関数名は%sです。\n", node->name);
        if (node->arg) {
            printf("引数のNodeを出力します。\n");
            printNode(node->arg);
        }
    }
    printNode(node->lhs);
    printNode(node->rhs);
}

char* printNodeKind(NodeKind kind) {
    switch (kind) {
        case 0:
            return "ND_ADD";
        case 1:
            return "ND_SUB";
        case 2:
            return "ND_MUL";
        case 3:
            return "ND_DIV";
        case 4:
            return "ND_EQ";
        case 5:
            return "ND_NE";
        case 6:
            return "ND_LT";
        case 7:
            return "ND_LE";
        case 8:
            return "ND_ASSIGN";
        case 9:
            return "ND_RETURN";
        case 10:
            return "ND_IF";
        case 11:
            return "ND_WHILE";
        case 12:
            return "ND_FOR";
        case 13:
            return "ND_BLOCK";
        case 14:
            return "ND_EXPR_STMT";
        case 15:
            return "ND_LVAR";
        case 16:
            return "ND_NUM";
        case 17:
            return "ND_FUNCTION_CALL";
        default:
            return "ERROR";
    }
}