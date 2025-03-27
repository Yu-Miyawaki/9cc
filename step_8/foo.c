
// objdump --disassemble=main step_0/test2
// リトルエンディアンに注意

// .data
int a = 7;

// .text
int f(){
    return 2;
}
int g(int x);

__attribute__ ((section (".text")))
char main[] = "\x40\x05\x80\xd2\xc0\x03\x5f\xd6";


// // mainを.dataセクションに置くと__ehdr_startがないというエラーが発生しコンパイルできない
// // これを作成しておくと実行時に止まらなくなった
// char __ehdr_start[] = "\x40\x05\x80\xd2\xc0\x03\x5f\xd6";
