

test3:     file format elf64-littleaarch64


Disassembly of section .init:

Disassembly of section .plt:
@ Procedure Linkage Table
@ <-> GOT: Global Offsets Table

Disassembly of section .text:

0000000000000718 <plus>:
@ アドレス, 機械語, アセンブリ
 718:   d10043ff        sub     sp, sp, #0x10
 71c:   b9000fe0        str     w0, [sp, #12]
 720:   b9000be1        str     w1, [sp, #8]
 724:   b9400fe1        ldr     w1, [sp, #12]
 728:   b9400be0        ldr     w0, [sp, #8]
 72c:   0b000020        add     w0, w1, w0
 730:   910043ff        add     sp, sp, #0x10
 734:   d65f03c0        ret

0000000000000738 <main>:
 738:   a9bf7bfd        stp     x29, x30, [sp, #-16]!
 @ Store Pair
 73c:   910003fd        mov     x29, sp
 740:   52800081        mov     w1, #0x4                        // #4
 744:   52800060        mov     w0, #0x3                        // #3
 748:   97fffff4        bl      718 <plus>
 74c:   a8c17bfd        ldp     x29, x30, [sp], #16
 750:   d65f03c0        ret

Disassembly of section .fini:
@ 終了時の後処理
