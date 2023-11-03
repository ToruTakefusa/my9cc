#! /bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc tmp.s foo.o -o tmp
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "int main() {
    return 0;
}"
assert 42 "int main() {return 42;}"
assert 21 "int main() {return 5+20-4;}"
assert 41 "int main() {return  12 + 34 - 5;}"
assert 47 "int main() {return 5+6*7;}"
assert 15 "int main() {return 5*(9-6);}"
assert 4  "int main() {return (3+5)/2;}"
assert 10 "int main() {return -10+20;}"
assert 30 "int main() {return +10+20;}"
assert 0 "int main() {return 0==1;}"
assert 1 "int main() {return 42==42;}"
assert 1 "int main() {return 0!=1;}"
assert 0 "int main() {return 42!=42;}"

assert 1 "int main() {return 0<1;}"
assert 0 "int main() {return 1<1;}"
assert 0 "int main(){return 2<1;}"
assert 1 "int main() {return 0<=1;}"
assert 1 "int main() {return 1<=1;}"
assert 0 "int main() {return 2<=1;}"

assert 1 "int main() {return 1>0;}"
assert 0 "int main() {return 1>1;}"
assert 0 "int main() {return 1 > 2;}"
assert 1 "int main() {return 1 >= 0;}"
assert 1 "int main() {return 1 >= 1;}"
assert 0 "int main() {return 1>=2;}"
assert 14 "int main() {
int a;
int b;
a = 3;
b = 5 * 6 - 8;
return a + b / 2;
}"
assert 6 "int main() {
int foo;
foo = 1;
int bar;
bar = 2 + 3;
return foo + bar;}"
assert 6 "int main() {
int FOO;
int BAR;
Foo = 1;
BAR = 2 + 3;
return Foo + BAR;}"
assert 6 "int main() {
  int _foo;
  int Bar1;
  _foo = 1;
  Bar1 = 2 + 3;
  return _foo + Bar1;
}"
assert 14 "int main() {
  int a;
  int b;
  a = 3;
  b = 5 * 6 - 8;
  return a + b / 2;
  }"
assert 5 "int main() {
  return 5;
}"

# ifのテスト
assert 0 "int main() {if (1) return 0;}"
assert 1 "int main() {int a;
a = 1;
if(a) return 1;}"
assert 4 "int main() {
int a;
int b;
int c;
b = 1;
if (1)
a = 3;
c = a + b;
return c;}"
# if elseのテスト
assert 0 "
int main() {
  if (0)
  return 1;
  else return 0;
}"

assert 1 "
int main() {
  if (0) return 0;
  else return 1;
}"

assert 5 "
int main() {
  int a;
  int b;
  a = 1;
  if (0)
  return 0;
  else b = 4;
  return a + b;
}"

# whileのテスト
assert 2 "
int main() {
  int a;
  int b;
  a = 0;
  while (a < 2)
  a = a + 1;
  return a;
}"

assert 1 "
int main() {
  while (0)
    return 0;
  return 1;
}"

# forのテスト
assert 0 "
int main() {
  for (;;)
  return 0;
}"

assert 1 "
int main() {
  int a;
  int b;
  b = 0;
  for (a = 1 ; a < 2 ; a = a + 1)
  b = a + b;
  return b;
}"

assert 3 "
int main() {
  int a;
  for (a = 1; a < 3; a = a + 1)
  int b;
  b = 0;
  return a;
}"
#
assert 3 "
int main() {
  int a;
  a = 0;
  for (; a < 3; a = a + 1)
  int b;
  b = 0;
  return a;
}"
assert 0 "
int main() {
  int a;
  a = 0;
  for (; a < 3; )
  a = a +1;
  return 0;
}"

## 複合ケース
assert 4 "
int main() {
  int a;
  a = 0;
  for (; ; a = a + 1)
    if (a > 3) return a;
}"

assert 5 "
int main() {
  int a;
  a = 0;
  for (; ;)
    if (a > 4) return a; else return 5;
}"

# Block
assert 2 "
int main() {
  int a;
  a = 1;
  if (10 > 5) {
    a = a + 1;
    return a;
  }
  return 0;
}"

# 関数呼び出し
assert 5 "
int main() {
  return return5();
}"

assert 2 "
int main() {
  return foo1(2);
}"

assert 1 "
int main() {
  int a;
  a = foo1(1);
  return a;
}"

assert 1 "
int main() {
  return foo1(foo1(1));
}"

assert 3 "
int main() {
  return foo2(1, 2);
}"

assert 16 "
int main() {
  return foo6(3, 2, 1, 2, 3, 4);
}"

# function definition
assert 1 "
int def1() {
 return 1;
}

int main() {
  return def1();
}
"

assert 5 "
int def1() {
  int a;
  a = 5;
  return a;
}

int main() {
  return def1();
}
"

assert 3 "
int def1(int a, int b) {
    int c;
    c = a + b;
 return a + b;
}

int main() {
  return def1(1, 2);
}
"

assert 3 "
int main() {
    int x;
    x = 3;
    int y;
    y = &x;
    return *y;
}
"

#assert 3 "
#int main() {
#    int x;
#    int *y;
#    y = &x;
#    *y = 3;
#    return x;
#}
#"

echo OK
