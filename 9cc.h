#ifndef INCLUDED_9CC_H
#define INCLUDED_9CC_H

#include <stdbool.h>

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
    int len;        // トークン長
};

// 四則演算,比較演算ASTのノードの種類
typedef enum {
    // 四則演算
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    // 比較演算
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_GT,
    ND_GE,
} NodeKind;
typedef struct Node Node;

// 四則演算ASTのノード型
struct Node {
    NodeKind kind; // ノードの型
    Node *lhs;     // 左辺: left-hand side
    Node *rhs;     // 右辺
    int val;       // numの場合
};


// extern bool DEBUG_9CC;

// 現在着目しているトークン(グローバル変数)
extern Token *token;
// 入力プログラム
extern char *user_input;


// codegen.c
void gen(Node *node);


// parse.c
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
void d_printf(char *fmt, ...);
// bool consume(char *op);
// void expect(char *op);
// int expect_number();
// bool at_eof();
// Token *new_token(Tokenkind kind, Token *cur, char *str, int len);
Token *tokenize(char *p);
// Node *new_node(NodeKind kind, Node *lhs, Node *rhs);
// Node *new_node_num(int val);
Node *expr();
// Node *equality();
// Node *relational();
// Node *add();
// Node *mul();
// Node *unary();
// Node *primary();


#endif
