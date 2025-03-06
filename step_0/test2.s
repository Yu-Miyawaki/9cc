
.globl main
.p2align 2
main:
        mov x0, #42  // x0 に 42 をセット
        ret          // リターン



// // x86-64
// .intel_syntax noprefix
// .globl main
// main:
//         mov rax, 42
//         ret
