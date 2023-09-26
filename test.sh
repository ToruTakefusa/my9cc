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

assert 0 "return 0;"
assert 42 "return 42;"
assert 21 "return 5+20-4;"
assert 41 "return  12 + 34 - 5; "
assert 47 "return 5+6*7;"
assert 15 "return 5*(9-6);"
assert 4  "return (3+5)/2;"
assert 10 "return -10+20;"
assert 30 "return +10+20;"
assert 0 "return 0==1;"
assert 1 "return 42==42;"
assert 1 "return 0!=1;"
assert 0 "return 42!=42;"

assert 1 "return 0<1;"
assert 0 "return 1<1;"
assert 0 "return 2<1;"
assert 1 "return 0<=1;"
assert 1 "return 1<=1;"
assert 0 "return 2<=1;"

assert 1 "return 1>0;"
assert 0 "return 1>1;"
assert 0 "return 1 > 2;"
assert 1 "return 1 >= 0;"
assert 1 "return 1 >= 1;"
assert 0 "return 1>=2;"
assert 14 "a = 3;
b = 5 * 6 - 8;
return a + b / 2;"
assert 6 "foo = 1;
bar = 2 + 3;
return foo + bar;"
assert 6 "Foo = 1;
BAR = 2 + 3;
return Foo + BAR;"
assert 6 "_foo = 1;
Bar1 = 2 + 3;
return _foo + Bar1;"
assert 14 "a = 3;
b = 5 * 6 - 8;
return a + b / 2;"
assert 5 "return 5;"

# ifのテスト
assert 0 "if (1) return 0;"
assert 1 "a = 1;
if(a) return 1;"
assert 4 "b = 1;
if (1)
a = 3;
c = a + b;
return c;"
# if elseのテスト
assert 0 "
if (0)
return 1;
else return 0;"

assert 1 "
if (0) return 0;
else return 1;"

assert 5 "
a = 1;
if (0)
return 0;
else b = 4;
return a + b;"

# whileのテスト
assert 2 "
a = 0;
while (a < 2)
a = a + 1;
return a;
"

assert 1 "
while (0)
  return 0;
return 1;
"

# forのテスト
assert 0 "
for (;;)
return 0;
"

assert 1 "
b = 0;
for (a = 1 ; a < 2 ; a = a + 1)
b = a + b;
return b;
"

assert 3 "
for (a = 1; a < 3; a = a + 1)
b = 0;
return a;
"

assert 3 "
a = 0;
for (; a < 3; a = a + 1)
b = 0;
return a;
"
assert 0 "
a = 0;
for (; a < 3; )
a = a +1;
return 0;
"

## 複合ケース
assert 4 "
a = 0;
for (; ; a = a + 1)
  if (a > 3) return a;
"

assert 5 "
a = 0;
for (; ;)
  if (a > 4) return a; else return 5;
"

# Block
assert 2 "
a = 1;
if (10 > 5) {
  a = a + 1;
  return a;
}
return 0;
"

# 関数呼び出し
assert 5 "
return return5();
"

assert 2 "
return foo1(2);
"

assert 1 "
a = foo1(1);
return a;
"

assert 1 "
return foo1(foo1(1));
"

assert 3 "
return foo2(1, 2);
"

assert 16 "
return foo6(3, 2, 1, 2, 3, 4);
"
echo OK
