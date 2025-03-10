#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }
    
    char *p = argv[1];

    printf(".globl main\n");
    printf("main:\n");

    if(!*p || !isdigit(*p)){
        fprintf(stderr, "数字で始まっていません: %s\n", argv[1]);
        return 1;
    }
    
    printf("    mov x0, %ld\n", strtol(p, &p, 10));

    while(*p){
        if(*p == '+'){
            ++p;
            printf("    add x0, x0, %ld\n", strtol(p, &p, 10));
            continue;
        }
        else if(*p == '-'){
            ++p;
            printf("    sub x0, x0, %ld\n", strtol(p, &p, 10));
            continue;
        }
        else{
            fprintf(stderr, "予期しない文字列です: %c: %s\n", *p, argv[1]);
            return 1;
        }
    }

    printf("    ret\n");
    
    return 0;
}
