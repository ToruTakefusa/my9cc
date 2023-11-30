#include <stdlib.h>

int return5() {
  return 5;
}

int foo1(int a) {
    return a;
}

int foo2(int a, int b) {
    return a + b;
}

int foo6(int a, int b, int c, int d, int e, int f) {
    return a * b + c + d + e + f;
}

/**
 * 指定した要素を持つ配列を作成する。
 * @param p
 */
int *alloc4(int *p, int a, int b, int c, int d)  {
    p = malloc(sizeof (int) * 4);
    *p = a;
    *(p + 1) = b;
    *(p + 2) = c;
    *(p + 3) = d;
    return p;
}