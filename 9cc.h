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
    TK_RESERVED,    // 記号
    TK_IDENT,       // 識別子
    TK_NUM,         // 整数トークン
    TK_EOF,         // 入力の終わりを表すトークン
    TK_RETURN,      // return
    TK_IF,          // if
    TK_ELSE,        // else
    TK_WHILE,       // while
    TK_FOR,         // for
    TK_SIZEOF,      // sizeof
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
    ND_EQ, // ==
    ND_NE, // !=
    ND_LT, // <
    ND_LE, // <=
    ND_ASSIGN, // =
    ND_RETURN, // return
    ND_IF,  // if
    ND_WHILE, // while
    ND_FOR,  // for
    ND_BLOCK, // Block
    ND_EXPR_STMT, // Expression statement
    ND_LVAR, // ローカル変数
    ND_NUM, // 整数
    ND_FUNCTION_CALL, // 関数呼び出し
    ND_FUNCTION_DEF, // 関数定義
    ND_ADDR,        // &
    ND_DEREF,       // *
} NodeKind;

typedef enum {
    INT,    // int型
    PTR,    // *
} TypeName;

typedef struct Vector Vector;
typedef struct Node Node;
typedef struct Type Type;

struct Vector {
    int size;
    int length;
    Node *data;
};

struct Type {
    TypeName ty;     // 型
    struct Type *ptr_to;    // tyがPTRの場合のみ使用する、参照先の型情報
};

struct Node {
    NodeKind kind; // ノードの型
    Node *lhs;      // 左辺
    Node *rhs;      // 右辺
    Node *cond;     // ifの条件、whileの条件、forの継続条件
    Node *then;     // ifのthen節、whileのthen, forのthen
    Node *els;      // ifのelse節
    Node *init;     // for(init, cond, loop)
    Node *loop;     // 同上
    int val;        // kindがND_NUMの場合のみ使う
    int offset;     // kindがND_LVARの場合のみ使う
    Vector *args;   // 関数の引数。kindがND_FUNCTION_CALLもしくはND_FUNCTION_DEFの場合、使用する。
    Vector *stmt;   // kindがND_BLOCKもしくはND_FUNCTION_DEFの場合、使用。
    char *name;     // 関数名
    int variables;  // 変数の数
    Type *type;      // 型情報
};

// 入力プログラム
extern char *user_input;

// パース結果のノード
extern Node *code[];

void error(char *fmt, ...);

void error_at(char *loc, char *fmt, ...);

void *tokenize();

void gen(Node *node);

Node *func();

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

Node *make_node();

Node *new_node_num(int val);

// For Debug
void printNode(Node *node);

char* printNodeKind(NodeKind kind);

#endif //INC_9CC_9CC_H
