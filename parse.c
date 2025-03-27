#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "9cc.h"

// bool DEBUG_9CC = true;
bool DEBUG_9CC = false;

// 現在着目しているトークン(グローバル変数)
Token *token;
// 入力プログラム
char *user_input;

// エラー箇所の報告
// loc: 入力プログラムの対象の位置のポインタ
void error_at(char *loc, char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    // printf("------pos: %d------", pos);
    fprintf(stderr, "%s\n", user_input);
    // pos個の空白を出力
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// エラー報告
void error(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// デバッグ用
void d_printf(char *fmt, ...){
    if(!DEBUG_9CC){
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
}

// 次のトークンがopならトークンを1つ読み進めてtrueを返す
bool consume(char *op){
    if(token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)
        ){
        return false;
    }
    else{
        token = token->next;
        return true;
    }
}

// 次のトークンがopならトークンを1つ読み進め、そうでないならエラー
void expect(char *op){
    if(token->kind != TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len)
        ){
        error_at(token->str, "'%s'ではありません", op);
    }
    else{
        token = token->next;
    }
}

// 次のトークンがopならトークンを1つ読み進め数字を返し、そうでないならエラー
int expect_number(){
    // d_printf("---expect_number\n%s\nlen:%d\n---\n", token->str, token->len);
    if(token->kind != TK_NUM){
        error_at(token->str, "数ではありません");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof(){
    return token->kind == TK_EOF;
}

// 新しいトークンを繋げ返す
Token *new_token(Tokenkind kind, Token *cur, char *str, int len){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    cur->next = tok;
    return tok;
}

// 入力文字列pをトークナイズし返す
Token *tokenize(char *p){
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            ++p;
            continue;
        }
        
        else if(!memcmp(p, "==", 2) ||
                !memcmp(p, "!=", 2) ||
                !memcmp(p, "<=", 2) ||
                !memcmp(p, ">=", 2)
                ){
            d_printf("---2 in tokenize\n%s\n---\n", p);
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        else if(strchr("+-*/()<>", *p)){
            d_printf("---1 in tokenize\n%s\n---\n", p);
            cur = new_token(TK_RESERVED, cur, p, 1);
            ++p;
            continue;
        }
        else if(isdigit(*p)){
            d_printf("---digit in tokenize\n%s\n---\n", p);
            char *before_p = p;
            int val = strtol(p, &p, 10);
            cur = new_token(TK_NUM, cur, before_p, p-before_p);
            cur->val = val;
            continue;
        }
        else{
            error_at(token->str, "トークナイズできません");
        }
    }

    new_token(TK_EOF, cur, p, 1);
    
    return head.next;
}

// 左辺と右辺から親ノードを作成し返す
Node *new_node(NodeKind kind, Node *lhs, Node *rhs){
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 数値根ノードを作成し返す
Node *new_node_num(int val){
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

// ------以下の四則演算の文法のパーサ------
// expr       = equality
// equality   = relational ("==" relational | "!=" relational)*
// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
// add        = mul ("+" mul | "-" mul)*
// mul        = unary ("*" unary | "/" unary)*
// unary      = ("+" | "-")? primary
// primary    = num | "(" expr ")"

Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr(){
    Node *node = equality();
}

Node *equality(){
    Node *node = relational();

    while(true){
        if(consume("==")){
            node = new_node(ND_EQ, node, relational());
        }
        else if(consume("!=")){
            node = new_node(ND_NE, node, relational());
        }
        else{
            return node;
        }
    }
}

Node *relational(){
    Node *node = add();

    while(true){
        if(consume("<=")){
            node = new_node(ND_LE, node, add());
        }
        else if(consume("<")){
            node = new_node(ND_LT, node, add());
        }
        else if(consume(">=")){
            node = new_node(ND_GE, node, add());
        }
        else if(consume(">")){
            node = new_node(ND_GT, node, add());
        }
        else{
            return node;
        }
    }
}

Node *add(){
    Node *node = mul();
    
    while(true){
        if(consume("+")){
            node = new_node(ND_ADD, node, mul());
        }
        else if(consume("-")){
            node = new_node(ND_SUB, node, mul());
        }
        else{
            return node;
        }
    }
}

Node *mul(){
    Node *node = unary();

    while(true){
        if(consume("*")){
            node = new_node(ND_MUL, node, unary());
        }
        else if(consume("/")){
            node = new_node(ND_DIV, node, unary());
        }
        else{
            return node;
        }
    }
}

Node *unary(){
    if(consume("+")){
        return primary();
    }
    else if(consume("-")){
        // 簡単のため0-xとみなす
        return new_node(ND_SUB, new_node_num(0), primary());
    }
    else{
        return primary();
    }
}

Node *primary(){
    // num or ()
    if(consume("(")){
        Node *node = expr();
        expect(")");
        return node;
    }
    else{
        return new_node_num(expect_number());
    }
}
