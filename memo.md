

## ビルド

```zsh
$ docker run --rm -v $HOME/Documents/project/9cc:/9cc -w /9cc compilerbook make test
$ docker run --rm -it -v $HOME/Documents/project/9cc:/9cc -w /9cc compilerbook
```

## テスト

```bash
$ make test
$ bash -x test.sh
```

---

## 機械語とアセンブラ
step0

### CPUとメモリ

* プログラムカウンタ
* 命令セット
   `$ uname -m`
   * x86-64 (AMD64)
   * ARM (AArch64?) - 組み込み, スマートフォン, Apple Siliconなど
   * RISC-V, MIPS, ...

### アセンブラ

`$ objdump -d -M intel <実行ファイル>`  
`$ objdump -d -M reg-names-std <>`  
`$ objdump --disassemble=main <>` - GNU  
`$ objdump --disassemble-symbols=_main <>` - LLVM  
dは逆アセンブル, SはCソースコード, Mは記法

ARM64のため記事と異なる.  
[Apple Siliconで進めるCコンパイラ作成入門](https://zenn.dev/micin/articles/78f292afb77ef0)  
[M1 Macで自作Cコンパイラをセルフホストした](https://zenn.dev/derbuihan/articles/e5387c5b28334f#参考)  
[Linux で Arm64 アセンブリプログラミング (00)](https://www.mztn.org/dragon/arm6400idx.html)  
など  
ARMではコメントは@

* RAX - 演算結果の格納 (Accumulator)
   ARM64ではx0 ?
* $?

## 電卓レベルの言語の作成

### step1

ARMではアセンブリ記法がUAL規格に統一されている  
ccは標準のCコンパイラ, gccはGNU Compiler Collection  
普通cc -> gccで、コンテナでも/usr/bin/cc -> /etc/alternatives/cc -> /usr/bin/gcc  
-gオプションではデバッグ情報がアセンブリに含まれるので$USER等が含まれないよう注意  
.sh: ''はリテラルで変数展開されない  
.sh: if文の中身はコマンドなのでスペースが必要, 文字列は=, 数値は-eq,ne  

### step2

ARMではadd, subは+=ではなく=  
* strtol - 2つ目の引数に指定したポインタを更新する. double: strtod

### step3

C:
* usingはない
* va_list, va_start, vprintf - stdarg.hで定義されている可変長引数のprintf
この3つをセットでerror関数みたいに使う (大元の引数はconstをつけるものが多い?)  
va_list: 可変長引数リストの型  
va_start: va_listと固定長引数の最後を渡すとva_listにリストが格納される  
vfprintf(出力先, フォーマット文字列, va_list)  
va_end(va_list): ポインタにNULLをセットし引数の取得を終了する
* C23以前にはbool型はなくstdbool.hのマクロを用いる
* そのまま使う場合structやenumの前にはtypedefが必要

### step4

C:
* フォーマット指定子に*をつけると個数を表す

---

### 構文解析

四則演算のパーサの動作について、括弧がない場合にexprが正常終了するまでを考えると分かりやすい
* CST: コメントやスペースなど意味のない部分も含めたもの

### スタックマシン

* imul - 符号つき(i: Integer ?)
* RDX - データレジスタ: 計算結果の一時記憶  
   64*64bitの上位64bit, 128/64bitの被除数上位64bit, 余り, その他汎用レジスタ
* idiv - cqoによりRAX -> RDX:RAXに符号拡張して商をRAX, 余りをRDXに格納

* ldr, str - ARMでのレジスタのLoad, Store
* sp - Stack Pointer  
   スタックは大きいメモリから割り当てられる  
   spに対してはmovで即値を書き込むことはできない  
   spをベースレジスタにする場合16バイトアライメントが必要
* \# - 即値
* ベースレジスタ - プログラムや関数の先頭のアドレス -> step9
* ポップ - ldr x0, [sp], #16 - x0=*sp; sp+=16;
* プッシュ - str x0, [sp, -16]! - sp-=16; *sp=x0;  
   プリ/ポストインデックスアドレッシング
* sdiv - 符号付き除算 (余りの計算は格納されない)
* madd, msub - MULとそれ以外をまとめて行う: 順番に注意  
   msub x3, x2, x1, x0 // x3 = x0 - (x2 * x1)

### step6

unary: 単項の

### step7

x86-64
* RFLAGS - フラグレジスタ: 1bitごとに異なるフラグを表す(上位の方は使われていない)
* cmp - 第1オペランドを更新しないSUB: RFLAGSのZF(ゼロフラグ)などに値をセットする
* sete - ZFから8bitレジスタにセット
* [その他比較](https://azelpg.gitlab.io/azsky2/note/prog/asm64/fc_gen3.html)
* AL - RAXの下位8bit
* movzb - ゼロ拡張ロード: 小さいサイズのデータのロード時に0埋めする  
   他に符号拡張(movs*)や指定した幅でコピーするストア(mov)など  
   l(long): 32bit, w(word): 16bit, b(byte): 8bit

ARM
* NZCV - ARMでのフラグレジスタ  
   対応: Z-ZF, N-SF(negative,符号フラグ), C-CF(キャリー), V-OF(Overflow)
* eq - Z=1を表す条件
* cset <レジスタ> 条件 - 条件に応じて1 or 0をセット(条件付きセット)
* [その他条件](https://www.mztn.org/dragon/arm6408cond.html)  
   lt: less than, ge: Greater than or Equal, lo: Lower, hs: Higher or Sameなど  
   条件分岐ではb.eq <ブランチ>のように書きgoto文のように動作する
* mov <> <即値>では1命令では特定のビットマスクに対して16ビットを設定するためそれ以上ではエラー表示が出る(が今回のtest自体は通る)  
   movz, movkを組み合わせる[link /MOV(定数)](https://www.mztn.org/dragon/arm6405str.html)  
   -2^16~2^16くらいがエラー表示の閾値なので17ビットのように見える?

