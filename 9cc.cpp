#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>

using std::cout;

int main(int argc, char* argv[]){
    if(argc != 2){
        std::cerr << "引数の個数が正しくありません" << std::endl;
        return 1;
    }
    std::vector<std::string> args(argv, argv+argc);

    cout << ".globl main\n";
    cout << "main:\n";

    std::istringstream stream(args[1]);
    long input_num;
    char input_ch;

    if(stream.eof() || !std::isdigit(stream.peek())){
        std::cerr << "数字で始まっていません" << stream.rdbuf() << std::endl;
        return 1;
    }
    stream >> input_num;
    cout << "    mov x0, " << input_num << "\n";

    while(!stream.eof()){
        input_ch = stream.peek();
        if(input_ch == '+'){
            stream.get();
            stream >> input_num;
            cout << "    add x0, x0, " << input_num << std::endl;
            continue;
        }
        else if(input_ch == '-'){
            stream.get();
            stream >> input_num;
            cout << "    sub x0, x0, " << input_num << std::endl;
            continue;
        }
        else{
            std::cerr << "予期しない文字列です: " << stream.rdbuf() << std::endl;
            return 1;
        }
    }

    cout << "    ret\n";
}
