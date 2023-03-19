#include "9cc.h"

Token *token;
FunctionData functionData = {0};
char *user_input;

int main(int argc, char **argv) {
    if (argc != 2) {
        error("引数の個数が正しくありません。");
        return 1;
    }

    // トークナイズしてパースする。
    user_input = argv[1];
    tokenize();
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // プロローグ
    // 変数25個分の領域を確保する
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", functionData.locals * 8);

    // 先頭の式から順にコード生成
    for (int i = 0; code[i]; i++) {
        gen(code[i]);
    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf(".Lreturn:\n");
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}
