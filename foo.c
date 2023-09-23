int foo() {
  printf("OK\n");
}

int foo1(int a) {
    return a;
}

int foo2(int a, int b) {
    printf("OK\n");
    printf("引数aの値は%d\n", a);
    printf("引数bの値は%d\n", b);
}