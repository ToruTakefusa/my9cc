//
// Created by toru on 22/10/24.
//

#ifndef INC_9CC_9CC_H
#define INC_9CC_9CC_H
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_IDENT,    // 識別子
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
    TK_RETURN,   // return
    TK_IF,       // if
    TK_ELSE,     // else
    TK_WHILE,    // while
    TK_FOR,      // for
} TokenKind;

typedef struct Token Token;

struct Token {
    TokenKind kind; // トークンの型
    Token *next;    // 次の入力トークン
    int val;        // kindがTK_NUMの場合、その数値
    char *str;      // トークン文字列
    int len;
};

// 現在着目しているトークン
extern Token *token;

typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_ASSIGN, // =
    ND_LVAR, // ローカル変数
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_NUM, // 整数
    ND_RETURN, // return
    ND_IF,  // if
    ND_WHILE, // while
    ND_FOR,  // for
    ND_BLOCK, // Block
} NodeKind;

typedef struct Vector Vector;

typedef struct Node Node;

struct Vector {
    int size;
    int length;
    Node *data;
};

struct Node {
    NodeKind kind; // ノードの型
    Node *lhs; // 左辺
    Node *rhs; // 右辺
    // Todo: Vectorにする
    Node *els; // ifのelse節
    Node *init; // for(init, cond, loop)
    Node *cond; // 同上
    Node *loop; // 同上
    int val;   // kindがND_NUMの場合のみ使う
    int offset; // kindがND_LVARの場合のみ使う
    Vector *stmt;
};

typedef struct FunctionData FunctionData;

extern FunctionData functionData;

// 関数関連の情報
struct FunctionData {
    int locals; // ローカル変数の数
};

// 入力プログラム
extern char *user_input;

// パース結果のノード
extern Node *code[];

void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

void *tokenize();

void gen(Node *node);

Node *stmt();

Node *expr();

Node *assign();

Node *equality();

Node *relational();

Node *add();

Node *mul();

Node *unary();

Node *primary();

void program();

Vector *initVector();

Vector *addItem(Vector *vector, Node *item);

Node *getItem(Vector *vector, int i);

Node *new_node_kind(NodeKind kind);

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

Node *new_node_num(int val);

// For Debug
void printNode(Node *node);

char* printNodeKind(NodeKind kind);

#endif //INC_9CC_9CC_H
