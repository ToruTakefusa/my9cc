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
int variables = 0;

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

// 次のトークンが数値の場合、trueを返す。
// それ以外の場合は、falseを返す。
bool is_number() {
    if (token->kind == TK_NUM) {
        return true;
    }
    return false;
}

// 次のトークンが引数で指定した記号のときには、trueを返す。
// それ以外はfalseを返す。
bool is_symbol(char *op) {
    if (token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)) {
        return true;
    }
    return false;
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

void program() {
    int i = 0;
    while (!at_eof()) {
        code[i++] = func();
    }
    code[i] = NULL;
}

Node *func() {
    // 関数呼び出し毎に、ローカル変数の数とローカル変数の情報は初期化
    variables = 0;
    locals = NULL;

    Node *node = new_node_kind(ND_FUNCTION_DEF);
    Vector *args = initVector();

    expect("int");

    Token *tok = consume(TK_IDENT);
    if (!tok) {
        // 関数名が存在しない時
        error_at(token->str, "識別子ではありません\n");
    }
    expect("(");

    while(!consume_symbol(")")) {
        // Todo: 関数の引数の数が、関数の呼び出し元と異なる場合、エラーにする。
        Node *arg = new_node_kind(ND_LVAR);
        expect("int");
;       Token *tok = consume(TK_IDENT);
        // 下記は変数の記録
        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = !locals? 8 : (locals->offset + 8);
        arg->offset = lvar->offset;
        arg->type = INT;
        locals = lvar;
        addItem(args,arg);
        variables++;
        if (consume_symbol(",")) {
            continue;
        } else {
            // 引数の後は","か")"
            expect(")");
            break;
        }
    }

    expect("{");

    node->name = strndup(tok->str, tok->len);
    Vector *vec = initVector();
    while (!consume_symbol("}")) {
        addItem(vec, stmt());
    }
    node->stmt = vec;
    node->variables = variables;
    node->args = args;
    node->type = INT;
    return node;
}

Node *stmt() {
    Node *node = NULL;

    if (consume(TK_RETURN)) {
        node = new_node(ND_RETURN, expr(), NULL);
        expect(";");
        return node;
    } else if (consume(TK_IF)) {
        node = new_node_kind(ND_IF);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        if (consume(TK_ELSE)) {
            node->els = stmt();
        }
        return node;
    } else if (consume(TK_WHILE)) {
        node = new_node_kind(ND_WHILE);
        expect("(");
        node->cond = expr();
        expect(")");
        node->then = stmt();
        return node;

    } else if (consume(TK_FOR)) {
        node = new_node_kind(ND_FOR);
        expect("(");
        // 初期化式の処理
        if (consume_symbol(";")) {
            // 初期化式が存在しない
        } else {
            // 初期化が存在する
            node->init = expr();
            expect(";");
        }
        // 条件式の処理
        if (consume_symbol(";")) {
            // 条件式が存在しない
        } else {
            node->cond = expr();
            expect(";");
        }
        // 継続式の処理
        if (consume_symbol(")")) {
            // 継続式が存在しない
        } else {
            node->loop = expr();
            expect(")");
        }
        node->then = stmt();
        return node;
    } else if (consume_symbol("{")) {
        // block
        node = new_node_kind(ND_BLOCK);
        Vector *vector = initVector();
        while(!consume_symbol("}")) {
            addItem(vector,stmt());
        }
        node->stmt = vector;
        return node;
    } else {
        // その他
        node = expr();
        expect(";");
        return node;
    }
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
    if (consume_symbol("*"))
        return new_node(ND_DEREF, unary(), NULL);
    if (consume_symbol("&"))
        return new_node(ND_ADDR, unary(), NULL);
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
        if (consume_symbol("(")) {
            // 関数の場合
            Node *node = new_node_kind(ND_FUNCTION_CALL);
            node->name = strndup(tok->str, tok->len);

            // 引数が存在しない場合
            if (consume_symbol(")")) {
                return node;
            }
            // 引数が存在する場合(引数は6つしかサポートしない)
            // primary, primaryのはず
            node->args = initVector();
            addItem(node->args,add());
            while(consume_symbol(",")) {
                addItem(node->args, add());
            }
            consume_symbol(")");
            return node;
        }
        Node *node = new_node_kind(ND_LVAR);

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
        } else {
            //  定義されていない変数が見つかった
            printf("%d\n", tok->kind);
            error_at(tok->str, "変数が宣言されていません。\n");
        }
        return node;
    }

    tok = consume(TK_RESERVED);
    if (tok && !strncmp(tok->str, "int", 3)) {
        // 変数宣言
        tok = consume(TK_IDENT);
        Node *node = new_node_kind(ND_LVAR);
        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = !locals? 8 : (locals->offset + 8);
        node->offset = lvar->offset;
        node->type = INT;
        locals = lvar;
        variables++;
        return node;
    }

    return new_node_num(expect_number());
}