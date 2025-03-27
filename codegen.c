#include <stdio.h>
#include "9cc.h"

void gen(Node *node){
    if(node->kind == ND_NUM){
        // d_printf("---first num in gen\n---");
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
    case ND_EQ:
        printf("    cmp x0, x1\n");
        printf("    cset x0, eq\n");
        break;
    case ND_NE:
        printf("    cmp x0, x1\n");
        printf("    cset x0, ne\n");
        break;
    case ND_LT:
        printf("    cmp x0, x1\n");
        printf("    cset x0, lt\n");
        break;
    case ND_LE:
        printf("    cmp x0, x1\n");
        printf("    cset x0, le\n");
        break;
    case ND_GT:
        printf("    cmp x0, x1\n");
        printf("    cset x0, gt\n");
        break;
    case ND_GE:
        printf("    cmp x0, x1\n");
        printf("    cset x0, ge\n");
        break;
    default:
        error_at(token->str, "生成できません");
        break;
    }

    // push
    printf("    str x0, [sp, -16]!\n");
}