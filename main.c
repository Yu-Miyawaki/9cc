// #include <ctype.h>
// #include <stdarg.h>
// #include <stdbool.h>
#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include "9cc.h"

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
