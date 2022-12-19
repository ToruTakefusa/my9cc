#include "9cc.h"

Node *code[100];

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
    LVar *next; // 次の変数がNULL
    char *name; // 変数の名前
    int len;    // 変数の長さ
    int offset; // RBPからのオフセット
};

LVar *locals;

// 次のトークンが期待している記号のときには、トークンを1つ読み進めて
// 真を返す。それ以外の場合には偽を返す。
bool consume_symbol(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        return false;
    token = token->next;
    return true;
}

// 次のトークンが引数のトークン種別の場合、トークンを1つ読み進める。
// それ以外の場合は、偽を返す。
Token* consume(TokenKind kind) {
    if (token->kind != kind) {
        return NULL;
    }
    Token *ret = token;
    token = token->next;
    return ret;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
        error_at(token->str, "'%s'ではありません", op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません");
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// 変数を名前で検索する。見つからなかった場合は、NULLを返す。
LVar *find_lvar(Token *tok) {
    for (LVar *var = locals; var; var = var->next) {
        if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
            return var;
        }
    }
    return NULL;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

void program() {
    int i = 0;
    while (!at_eof())
        code[i++] = stmt();
    code[i] = NULL;
}

Node *stmt() {
    Node *node;

    if (consume(TK_IF)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_IF;
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();

        if (consume(TK_ELSE)) node->rhs->lhs = stmt();
        return node;
    } else if (consume(TK_WHILE)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_WHILE;
        expect("(");
        node->lhs = expr();
        expect(")");
        node->rhs = stmt();
        return node;
    } else if (consume(TK_FOR)) {
        // Todo: 実装
        // token->nextで次の値をチェック
        // consume_symbol()が使えるはず
    } else if (consume(TK_RETURN)) {
        node = calloc(1, sizeof(Node));
        node->kind = ND_RETURN;
        node->lhs = expr();
    } else {
        node = expr();
    }

    expect(";");
    return node;
}

Node *expr() {
    return assign();
}

Node *assign() {
    Node *node = equality();
    if (consume_symbol("="))
        node = new_node(ND_ASSIGN, node ,assign());
    return node;
}

Node *equality() {
    Node *node = relational();

    for (;;) {
        if (consume_symbol("==")) {
            node = new_node(ND_EQ, node, relational());
        } else if (consume_symbol("!=")) {
            node = new_node(ND_NE, node, relational());
        } else {
            return node;
        }
    }
}

Node *relational() {
    Node *node = add();

    for (;;) {
        if (consume_symbol("<")) {
            node = new_node(ND_LT, node, add());
        } else if (consume_symbol("<=")) {
            node = new_node(ND_LE, node, add());
        } else if (consume_symbol(">")) {
            node = new_node(ND_LT, add(), node);
        } else if (consume_symbol(">=")) {
            node = new_node(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume_symbol("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume_symbol("-"))
            node = new_node(ND_SUB, node, mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume_symbol("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume_symbol("/"))
            node = new_node(ND_DIV, node, unary());
        else
            return node;
    }
}

Node *unary() {
    if (consume_symbol("+"))
        return primary();
    if (consume_symbol("-"))
        return new_node(ND_SUB, new_node_num(0), primary());
    return primary();
}

Node *primary() {
    // 次のトークンが"("なら、"(" expr ")"のはず
    if (consume_symbol("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }

    Token *tok = consume(TK_IDENT);
    if (tok) {
        Node *node = calloc(1, sizeof(Node));
        node->kind = ND_LVAR;

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            lvar = calloc(1, sizeof(LVar));
            lvar->next = locals;
            lvar->name = tok->str;
            lvar->len = tok->len;
            lvar->offset = !locals? 0 : (locals->offset + 8);
            node->offset = lvar->offset;
            locals = lvar;
            functionData.locals++;
        }
        return node;
    }

    return new_node_num(expect_number());
}