# 9ccについて
9ccはRui Ueyamaさんが公開された[資料](https://www.sigbus.info/compilerbookv)を参考に作成したものです。<br>
2022年段階では、作成中となります。<br>
資料では28ステップが記載されており、29ステップ以降は要加筆となっています。<br>
そのため28ステップ完了を目標にしています。

## 文法
現在サポートしている文法は、下記となります。
```text
program    = int func
func = ident "(" (int ident)? (, int ident)* ")" "{" stmt* "}"
stmt       = expr ";"
            | "{" stmt* "}" 
            | "if" "(" expr ")" stmt ("else" stmt)?
            | "while" "(" expr ")" stmt
            | "for" "(" expr? ";" expr? ";" expr? ")" stmt
            | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = "sizeof" unary
            | "+"? primary
            | "-"? primary
            | "*" unary
            | "&" unary
primary    = num 
            | ident ("(" (add (, add)*)? ")")? 
            | "(" expr ")"
```

## 型のサポート
サポートしている型は、下記の通り
- int型
- ポインター型
- 配列