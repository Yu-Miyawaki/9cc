

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

* RAX - 演算結果の格納
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

