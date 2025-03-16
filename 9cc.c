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

// 四則演算ASTのノードの種類
typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
} NodeKind;
typedef struct Node Node;

// 四則演算ASTのノード型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺: left-hand side
    Node *rhs;     // 右辺
    int val;       // numの場合
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
        else if(strchr("+-*/()", *p)){
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
// expr    = mul ("+" mul | "-" mul)*
// mul     = primary ("*" primary | "/" primary)*
// primary = num | "(" expr ")"

Node *mul();
Node *primary();

Node *expr(){
    Node *node = mul();
    
    while(true){
        if(consume('+')){
            node = new_node(ND_ADD, node, mul());
        }
        else if(consume('-')){
            node = new_node(ND_SUB, node, mul());
        }
        else{
            return node;
        }
    }
}

Node *mul(){
    Node *node = primary();

    while(true){
        if(consume('*')){
            node = new_node(ND_MUL, node, primary());
        }
        else if(consume('/')){
            node = new_node(ND_DIV, node, primary());
        }
        else{
            return node;
        }
    }
}

Node *primary(){
    // num or ()
    if(consume('(')){
        Node *node = expr();
        expect(')');
        return node;
    }
    else{
        return new_node_num(expect_number());
    }
}

void gen(Node *node){
    if(node->kind == ND_NUM){
        // push val
        printf("    mov x0, #%d\n", node->val);
        printf("    str x0, [sp, -16]!\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    // pop 右部分木: 第2オペランド
    printf("    ldr x1, [sp], #16\n");
    // 左部分木
    printf("    ldr x0, [sp], #16\n");

    switch (node->kind){
    case ND_ADD:
        printf("    add x0, x0, x1\n");
        break;
    case ND_SUB:
        printf("    sub x0, x0, x1\n");
        break;
    case ND_MUL:
        printf("    mul x0, x0, x1\n");
        break;
    case ND_DIV:
        printf("    sdiv x0, x0, x1\n");
        break;
    default:
        error_at(token->str, "生成できません");
        break;
    }

    // push
    printf("    str x0, [sp, -16]!\n");
}

int main(int argc, char* argv[]){
    if(argc != 2){
        error("引数の個数が正しくありません\n");
        return 1;
    }
    
    // トークナイズとパース
    user_input = argv[1];
    token = tokenize(argv[1]);
    Node *node = expr();

    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    // スタックトップの値を返す
    printf("    ldr x0, [sp], #16\n");
    printf("    ret\n");
    
    return 0;
}
