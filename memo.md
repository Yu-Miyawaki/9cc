

## ビルド

```zsh
$ docker run --rm -v $HOME/Documents/project/9cc:/9cc -w /9cc compilerbook make test
$ docker run --rm -it -v $HOME/Documents/project/9cc:/9cc -w /9cc compilerbook
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
など?
ARMではコメントは@ ?

* RAX - 演算結果の格納
   ARM64ではx0 ?
* $?

## 電卓レベルの言語の作成

