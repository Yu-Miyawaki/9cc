#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数
    TK_EOF,      // EOF
} Tokenkind;

typedef struct Token Token;

// トークン型
struct Token {
    Tokenkind kind; // 型
    Token *next;    // 次のトークン
    int val;        // TK_NUMの値
    char *str;      // トークン文字列
};

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

// 次のトークンがopならトークンを1つ読み進めてtrueを返す
bool consume(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        return false;
    }
    else{
        token = token->next;
        return true;
    }
}

// 次のトークンがopならトークンを1つ読み進め、そうでないならエラー
void expect(char op){
    if(token->kind != TK_RESERVED || token->str[0] != op){
        error_at(token->str, "'%c'ではありません", op);
    }
    else{
        token = token->next;
    }
}

// 次のトークンがopならトークンを1つ読み進め数字を返し、そうでないならエラー
int expect_number(){
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
Token *new_token(Tokenkind kind, Token *cur, char *str){
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
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
        else if(*p == '+' || *p == '-'){
            cur = new_token(TK_RESERVED, cur, p++);
            continue;
        }
        else if(isdigit(*p)){
            cur = new_token(TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }
        else{
            error_at(token->str, "トークナイズできません");
        }
    }

    new_token(TK_EOF, cur, p);
    
    return head.next;
}

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }
    
    token = tokenize(argv[1]);
    user_input = argv[1];

    printf(".globl main\n");
    printf("main:\n");

    // 最初は数字で始まる必要がある
    printf("    mov x0, %d\n", expect_number());

    // (+ or -) <数字>の並びからアセンブリを出力
    while(!at_eof()){
        if(consume('+')){
            printf("    add x0, x0, %d\n", expect_number());
            continue;
        }
        expect('-');
        printf("    sub x0, x0, %d\n", expect_number());
    }

    printf("    ret\n");
    
    return 0;
}
