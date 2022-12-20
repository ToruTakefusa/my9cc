#! /bin/bash
assert() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5; "
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4  "(3+5)/2;"
assert 10 "-10+20;"
assert 30 "+10+20;"
assert 0 "0==1;"
assert 1 "42==42;"
assert 1 "0!=1;"
assert 0 "42!=42;"

assert 1 "0<1;"
assert 0 "1<1;"
assert 0 "2<1;"
assert 1 "0<=1;"
assert 1 "1<=1;"
assert 0 "2<=1;"

assert 1 "1>0;"
assert 0 "1>1;"
assert 0 "1 > 2;"
assert 1 "1 >= 0;"
assert 1 "1 >= 1;"
assert 0 "1>=2;"
assert 14 "a = 3;
b = 5 * 6 - 8;
a + b / 2;"
assert 6 "foo = 1;
bar = 2 + 3;
foo + bar;"
assert 6 "Foo = 1;
BAR = 2 + 3;
Foo + BAR;"
assert 6 "_foo = 1;
Bar1 = 2 + 3;
_foo + Bar1;"
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
echo OK
