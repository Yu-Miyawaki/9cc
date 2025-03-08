#include <iostream>

using std::cout;

int main(int args, char* argv[]){
    if(args != 2){
        std::cerr << "引数の個数が正しくありません" << std::endl;
        return 1;
    }

    cout << ".globl main\n";
    cout << "main:\n";
    cout << "    mov x0, " << std::atoi(argv[1]) << "\n";
    cout << "    ret\n";
}
