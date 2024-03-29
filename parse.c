#include "9cc.h"

Node *code[100];

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

/**
 * 次のトークンが期待している記号の場合は、trueを返す。
 * それ以外の場合は、falseを返す。
 * @param op
 * @return
 */
bool is_expect_op(Token *tok, char *op) {
    if (tok->kind != TK_RESERVED ||
        strlen(op) != tok->len ||
        memcmp(tok->str, op, tok->len))
        return false;
    return true;
}

// 次のトークンが期待している記号のときには、トークンを1つ読み進める。
// それ以外の場合にはエラーを報告する。
void expect(char *op) {
    if (!is_expect_op(token, op))
        error_at(token->str, "'%s'ではありません\n", op);
    token = token->next;
}

// 次のトークンが数値の場合、トークンを1つ読み進めてその数値を返す。
// それ以外の場合にはエラーを報告する。
int expect_number() {
    if (token->kind != TK_NUM)
        error_at(token->str, "数ではありません\n");
    int val = token->val;
    token = token->next;
    return val;
}

// 引数のトークンが型の場合は、トークンを1つ読み進め、該当する型名を返す。
// それ以外の場合は、エラーを報告する。
TypeName expect_type() {
    if (is_expect_op(token, "int")) {
        token = token->next;
        return INT;
    } else if (is_expect_op(token, "*")) {
        token = token->next;
        return PTR;
    } else {
        error_at(token->str, "型が宣言されていません\n");
    }
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

Type *new_type(TypeName typeName) {
    Type *type = malloc(sizeof (Type));
    type->ty = typeName;
    return type;
}

// FIXME: もう少しシンプルにしたい
Type *find_deref_type(Node *node, Type *type) {
    while (node && ND_DEREF == node->kind) {
        if (!type->ptr_to) return type;
        type = type->ptr_to;
        node = node->lhs;
    }
    return type;
}


Type *findType(Node *node) {
    if (!node) return NULL;
    Type *ltype = findType(node->lhs);
    Type *rtype = findType(node->rhs);


    if (ND_DEREF == node->kind) {
        if (ltype && PTR == ltype->ty) return find_deref_type(node, ltype);
        if (rtype && PTR == rtype->ty) return find_deref_type(node, rtype);
    }

    // どれかがポインタ型なら、ポインタ型を返す。
    if (node->type && PTR == node->type->ty) return node->type;
    if (ltype && PTR == ltype->ty) return ltype;
    if (rtype && PTR == rtype->ty) return rtype;

    if (node->type && INT == node->type->ty) return node->type;
    if (ltype && INT == ltype->ty) return ltype;
    if (rtype && INT == rtype->ty) return rtype;
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
    locals = NULL;

    Node *node = new_node_kind(ND_FUNCTION_DEF);
    Vector *args = initVector();

    // int型か、ポインタ型しかないので、最初は必ずint
    expect("int");

    Type *type = malloc(sizeof(Type));
    type->ty = INT;
    type->ptr_to = NULL;
    node->type = type;

    while (consume_symbol("*")) {
        // 「*」は任意個連続する可能性がある。
        type = malloc(sizeof (Type));
        type->ty = PTR;
        type->ptr_to = node->type;
        node->type = type;
    }

    Token *tok = consume(TK_IDENT);
    if (!tok) {
        // 関数名が存在しない時
        error_at(token->str, "識別子ではありません\n");
    }
    expect("(");

    while(!consume_symbol(")")) {
        // 関数の引数の処理
        // Todo: 関数の引数の数が、関数の呼び出し元と異なる場合、エラーにする。
        Node *arg = new_node_kind(ND_LVAR);

        // int型か、ポインタ型しかないので、最初は必ずint
        expect("int");
        Type *type = malloc(sizeof (Type));
        type->ty = INT;
        type->ptr_to  = NULL;
        arg->type = type;

        while (consume_symbol("*")) {
            // 「*」は任意個連続する可能性がある。
            type = malloc(sizeof (Type));
            type->ty = PTR;
            type->ptr_to = arg->type;
            arg->type = type;
        }

;       Token *tok = consume(TK_IDENT);
        // 下記は変数の記録
        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = !locals? 8 : (locals->offset + 8);
        lvar->type = type;
        arg->offset = lvar->offset;

        locals = lvar;
        addItem(args,arg);

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
    node->args = args;
    node->locals = locals;
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
        // blockの場合
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
        if (consume_symbol("+")) {
            node = new_node(ND_ADD, node, mul());
        } else if (consume_symbol("-")) {
            node = new_node(ND_SUB, node, mul());
        } else if (!node->lhs || !node->rhs) {
            return node;
        } else if (node->lhs && node->rhs) {
            // ポインタ型の演算を行う(左辺も右辺もポインタ型の場合は、対応しない)
            if ( ND_LVAR == node->lhs->kind && PTR == node->lhs->type->ty && (
                    ND_NUM == node->rhs->kind || ND_LVAR == node->rhs->kind && INT == node->rhs->type->ty)) {
                // lhsがポインタ型の変数かつ、rhsが数値もしくは、数値型の変数の場合
                if ( INT == node->lhs->type->ptr_to->ty ) {
                    // lhsが数値型の場合
                    Node *rhs = new_node(ND_NUM, NULL, NULL);
                    rhs->val = 4;
                    Node *tmp = new_node(ND_MUL, node->rhs, rhs);
                    node->rhs = tmp;
                } else if (PTR == node->lhs->type->ptr_to->ty) {
                    // lhsがポインタ型の場合
                    Node *rhs = new_node(ND_NUM, NULL, NULL);
                    rhs->val = 8;
                    Node *tmp = new_node(ND_MUL, node->rhs, rhs);
                    node->rhs = tmp;
                }
            } else if ( ND_LVAR == node->rhs->kind && PTR == node->rhs->type->ty && (
                    ND_NUM == node->lhs->kind || ND_LVAR == node->lhs->kind && INT == node->lhs->type->ty)) {
                // rhsがポインタ型の変数かつ、lhsが数値もしくは、数値型の変数の場合
                if (INT == node->rhs->type->ptr_to->ty) {
                    // rhsが数値型の場合
                    Node *lhs = new_node(ND_NUM, NULL, NULL);
                    lhs->val = 4;
                    Node *tmp = new_node(ND_MUL, node->lhs, lhs);
                    node->lhs = tmp;
                } else if (PTR == node->rhs->type->ptr_to->ty) {
                    // rhsがポインタ型の場合
                    Node *lhs = new_node(ND_NUM, NULL, NULL);
                    lhs->val = 8;
                    Node *tmp = new_node(ND_MUL, node->lhs, lhs);
                    node->rhs = tmp;
                }
            }
            return node;
        }
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
    // Todo: sizeofと&以外の場合、primaryが配列の場合、ポインタに変換する。

    if (consume_symbol("+")) {
//        Node *tmp = primary();
//        if (ND_LVAR == tmp->kind && ARRAY == tmp->type->ty) {
//            tmp->type->ty = PTR;
//        }
//        return tmp;
        return primary();
    }
    if (consume_symbol("-")) {
//        Node *tmp = primary();
//        if (ND_LVAR == tmp->kind && ARRAY == tmp->type->ty) {
//            tmp->type->ty = PTR;
//        }
//        return new_node(ND_SUB, new_node_num(0), tmp);
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    if (consume_symbol("*")) {
        int count = 0;

        while (consume_symbol("*")) {
            // 「*」は任意個連続する可能性がある。
            count++;
        }

        Node *new;
        Node *tmp = primary();
        if (ND_LVAR == tmp->kind && ARRAY == tmp->type->ty) {
            tmp->type->ty = PTR;
        }
        Node *old = tmp;

        while (count > 0) {
            new = new_node(ND_DEREF, old, NULL);
            old = new;
            count--;
        }

        return new_node(ND_DEREF, old, NULL);
    }
    if (consume_symbol("&"))
        return new_node(ND_ADDR, unary(), NULL);

    if (consume(TK_SIZEOF)) {
        Node *child = unary();
        Type *type = findType(child);
        if (type && INT == type->ty) {
            return new_node_num(4);
        } else if (type && PTR == type->ty) {
            return new_node_num(8);
        } else {
            return NULL;
        }
    }

    Node* tmp = primary();
    if (ND_LVAR == tmp->kind && ARRAY == tmp->type->ty) {
        tmp->type->ty = PTR;
    }
    return tmp;
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
            // 関数呼び出しの場合
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
        // 変数呼び出しの場合
        Node *node = new_node_kind(ND_LVAR);

        LVar *lvar = find_lvar(tok);
        if (lvar) {
            node->offset = lvar->offset;
            node->type = lvar->type;
        } else {
            //  定義されていない変数が見つかった
            printf("%d\n", tok->kind);
            error_at(tok->str, "変数が宣言されていません。\n");
        }
        return node;
    }

    tok = consume(TK_RESERVED);
    if (tok && (is_expect_op(tok, "int"))) {
        // 変数宣言の場合
        // int型か、ポインタ型しかないので、最初は必ずintと宣言されている。
        Node *node = make_node();

        Type *type = malloc(sizeof (Type));
        type->ty = INT;
        type->ptr_to  = NULL;
        node->type = type;

        while (consume_symbol("*")) {
            // 「*」は任意個連続する可能性がある。
            type = malloc(sizeof (Type));
            type->ty = PTR;
            type->ptr_to = node->type;
            node->type = type;
        }

        tok = consume(TK_IDENT);
        while (consume_symbol("[")) {
            // Todo: 二重配列の場合、OKかチェック
            // Todo: ポインタ型の配列
            // 配列の宣言
            type = malloc(sizeof (Type));
            type->ty = ARRAY;
            type->ptr_to = node->type;
            node->type = type;
            // [の後に続くのは変数か数値
            Node *child = primary();
            if (ND_NUM == child->kind) {
                type->array_size = child->val;
            }
            // Todo: Node種別がLVARの場合
            expect("]");
        }

        LVar *lvar = calloc(1, sizeof(LVar));
        lvar->next = locals;
        lvar->name = tok->str;
        lvar->len = tok->len;
        lvar->offset = !locals? 8 : (locals->offset + 8);
        node->kind = ND_LVAR;
        node->offset = lvar->offset;
        lvar->type = node->type;
        locals = lvar;

        return node;
    }

    return new_node_num(expect_number());
}