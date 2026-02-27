#include "../inc/tokenizer/Tokenizer.hpp"
#include "../inc/utils/FileUtils.hpp"
#include <iostream>

int main(int argc, char **argv) {
    if (argc == 2) {
        std::string input = readFile(argv[1]);
        Tokenizer tokenizer(input);
        std::vector<Token> tokens = tokenizer.tokenize();
        std::cout << tokens << std::endl;
    }
}