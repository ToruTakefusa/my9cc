/**
 * 動的配列
 */
#include "9cc.h"

/** 初期サイズ */
const int INIT_SIZE = 1;

/**
 * Vectorの初期化
 * @param vector
 * @return 初期化済みの動的配列
 */
Vector *initVector() {
    Vector *vector = malloc(sizeof(Vector));
    vector->size = INIT_SIZE;
    vector->length = 0;
    vector->data = (Node*)malloc(sizeof(Node) * INIT_SIZE);
    return vector;
}

/**
 * 引数のVectorに要素を追加する。
 * @param vector
 * @param item 追加する要素
 * @return
 */
Vector *addItem(Vector *vector, Node *item) {
    if (vector->length >= vector->size) {
        vector->data = (Node*) realloc(vector->data, vector->size * sizeof(Node) * 2);
        vector->size *= 2;
    }
    memcpy(&vector->data[vector->length], item, sizeof(Node));
    vector->length++;
    return vector;
}

/**
 * 引数のVectorから要素を取得する
 * @param vector
 * @param i
 * @return
 */
Node *getItem(Vector *vector, int i) {
    if ((vector->length - 1) < i) {
        error("length is %d, i = %d, index out of range in Vector\n", vector->length, i);
        exit(1);
    } else {
        return &vector->data[i];
    }
}