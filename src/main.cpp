#include "lexer.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

int main(int argc, const char* argv[]) {
    std::filesystem::path file_path;
    if (argc > 1 && std::filesystem::exists(argv[1]))
        file_path = argv[1];
    else
        file_path = "tests/begin.cele";

    std::ifstream source_file(file_path, std::ios::binary);
    const std::string source_code((std::istreambuf_iterator<char>(source_file)),
                                  std::istreambuf_iterator<char>());
    Lexer lexer(source_code);
    
    auto tokens = lexer.lex();
    for (const Token token : tokens)
        std::cout << token << '\n';
}
