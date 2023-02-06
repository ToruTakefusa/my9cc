#include "9cc.h"

Node *new_node_kind(NodeKind kind) {
    Node *node = malloc(sizeof(Node));
    node->kind = kind;
    node->stmt = NULL;
    node->val = 0;
    node->init = NULL;
    node->loop = NULL;
    node->cond = NULL;
    node->lhs = NULL;
    node->rhs = NULL;
    node->offset = 0;

    return node;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->kind = kind;
    node->stmt = NULL;
    node->val = 0;
    node->init = NULL;
    node->loop = NULL;
    node->cond = NULL;
    node->lhs = lhs;
    node->rhs = rhs;
    node->offset = 0;

    return node;
}

Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->kind = ND_NUM;
    node->stmt = NULL;
    node->val = val;
    node->init = NULL;
    node->loop = NULL;
    node->cond = NULL;
    node->lhs = NULL;
    node->rhs = NULL;
    node->offset = 0;

    return node;
}