#include "9cc.h"

bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

// 新しいトークンを生成してcurに繋げる
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// 入力文字列pをトークナイズしてそれを返す
void *tokenize() {
    char *p = user_input;
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        // 空白文字列をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (startswith(p, "==") || startswith(p, "!=") ||
            startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if (strchr("+-*/()<>=;", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        if ('a' <= *p && *p <= 'z' || 'A' <= *p && *p <= 'Z' || '_' == *p) {
            // 変数名は、1文字目はa~z, A~Z, _のいずれか
            int length = 1;

            // 2文字目以降は、数字、a~z, A~Z_のいずれか
            while(('1' <= *(p + 1)  && *(p + 1) <= '9') ||
            ('a' <= *(p + 1) && *(p + 1) <= 'z') ||
            ('A' <= *(p + 1) && *(p + 1) <= 'Z') ||
            '_' == *(p + 1)) {
                length++;
                p++;
            }
            cur = new_token(TK_IDENT, cur, (p - length + 1) , length);
            p++;
            continue;
        }

        error_at(p, "トークナイズできません");
    }

    new_token(TK_EOF, cur, p, 0);
    token = head.next;
}