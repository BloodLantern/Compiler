#include <fstream>
#include <iostream>

#include "Lexer.hpp"

int main()
{
    std::cout << "Hello World!\n";

    std::ifstream input{"example.txt"};
    Lexer lexer{input};
}
