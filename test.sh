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

assert 0 "main() {
    return 0;
}"
assert 42 "main() {return 42;}"
assert 21 "main() {return 5+20-4;}"
assert 41 "main() {return  12 + 34 - 5;}"
assert 47 "main() {return 5+6*7;}"
assert 15 "main() {return 5*(9-6);}"
assert 4  "main() {return (3+5)/2;}"
assert 10 "main() {return -10+20;}"
assert 30 "main() {return +10+20;}"
assert 0 "main() {return 0==1;}"
assert 1 "main() {return 42==42;}"
assert 1 "main() {return 0!=1;}"
assert 0 "main() {return 42!=42;}"

assert 1 "main() {return 0<1;}"
assert 0 "main() {return 1<1;}"
assert 0 "main(){return 2<1;}"
assert 1 "main() {return 0<=1;}"
assert 1 "main() {return 1<=1;}"
assert 0 "main() {return 2<=1;}"

assert 1 "main() {return 1>0;}"
assert 0 "main() {return 1>1;}"
assert 0 "main() {return 1 > 2;}"
assert 1 "main() {return 1 >= 0;}"
assert 1 "main() {return 1 >= 1;}"
assert 0 "main() {return 1>=2;}"
assert 14 "main() {
a = 3;
b = 5 * 6 - 8;
return a + b / 2;
}"
assert 6 "main() {foo = 1;
bar = 2 + 3;
return foo + bar;}"
assert 6 "main() {
Foo = 1;
BAR = 2 + 3;
return Foo + BAR;}"
assert 6 "main() {
  _foo = 1;
  Bar1 = 2 + 3;
  return _foo + Bar1;
}"
assert 14 "main() {
  a = 3;
  b = 5 * 6 - 8;
  return a + b / 2;
  }"
assert 5 "main() {
  return 5;
}"

# ifのテスト
assert 0 "main() {if (1) return 0;}"
assert 1 "main() {a = 1;
if(a) return 1;}"
assert 4 "main() {b = 1;
if (1)
a = 3;
c = a + b;
return c;}"
# if elseのテスト
assert 0 "
main() {
  if (0)
  return 1;
  else return 0;
}"

assert 1 "
main() {
  if (0) return 0;
  else return 1;
}"

assert 5 "
main() {
  a = 1;
  if (0)
  return 0;
  else b = 4;
  return a + b;
}"

# whileのテスト
assert 2 "
main() {
  a = 0;
  while (a < 2)
  a = a + 1;
  return a;
}"

assert 1 "
main() {
  while (0)
    return 0;
  return 1;
}"

# forのテスト
assert 0 "
main() {
  for (;;)
  return 0;
}"

assert 1 "
main() {
  b = 0;
  for (a = 1 ; a < 2 ; a = a + 1)
  b = a + b;
  return b;
}"

assert 3 "
main() {
  for (a = 1; a < 3; a = a + 1)
  b = 0;
  return a;
}"
#
assert 3 "
main() {
  a = 0;
  for (; a < 3; a = a + 1)
  b = 0;
  return a;
}"
assert 0 "
main() {
  a = 0;
  for (; a < 3; )
  a = a +1;
  return 0;
}"

## 複合ケース
assert 4 "
main() {
  a = 0;
  for (; ; a = a + 1)
    if (a > 3) return a;
}"

assert 5 "
main() {
  a = 0;
  for (; ;)
    if (a > 4) return a; else return 5;
}"

# Block
assert 2 "
main() {
  a = 1;
  if (10 > 5) {
    a = a + 1;
    return a;
  }
  return 0;
}"

# 関数呼び出し
assert 5 "
main() {
  return return5();
}"

assert 2 "
main() {
  return foo1(2);
}"

assert 1 "
main() {
  a = foo1(1);
  return a;
}"

assert 1 "
main() {
  return foo1(foo1(1));
}"

assert 3 "
main() {
  return foo2(1, 2);
}"

assert 16 "
main() {
  return foo6(3, 2, 1, 2, 3, 4);
}"

# function definition
assert 1 "
def1() {
 return 1;
}

main() {
  return def1();
}
"

assert 5 "
def1() {
  a = 5;
  return a;
}

main() {
  return def1();
}
"

assert 3 "
def1(a, b) {
    c = a + b;
 return a + b;
}

main() {
  return def1(1, 2);
}
"
echo OK
