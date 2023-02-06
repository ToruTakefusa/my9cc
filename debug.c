//
// Created by toru on 23/01/31.
//
#include "9cc.h"

void printNode(Node *node) {
    if (!node) {
        printf("node is Null");
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
            return "ND_ASSIGN";
        case 5:
            return "ND_LVAR";
        case 6:
            return "ND_EQ";
        case 7:
            return "ND_NE";
        case 8:
            return "ND_LT";
        case 9:
            return "ND_LE";
        case 10:
            return "ND_NUM";
        case 11:
            return "ND_RETURN";
        case 12:
            return "ND_IF";
        case 13:
            return "ND_WHILE";
        case 14:
            return "ND_FOR";
        case 15:
            return "ND_BLOCK";
        case 16:
            return "ND_OTHER";
        default:
            return "";
    }
}