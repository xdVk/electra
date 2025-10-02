#include "lexer.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>


// your path should be /ArtCele/build/
int main(int argc, const char* argv[]) {
    namespace fs = std::filesystem;
    fs::path file_path;

    if (argc > 1 && fs::exists(argv[1])) {
        file_path = argv[1];
    } else {
        const fs::path exe_path = argv[0];                  // /ArtCele/build/ArtCele.exe
        const fs::path exe_dir = exe_path.parent_path();    // /ArtCele/build/
        const fs::path project_dir = exe_dir.parent_path(); // /ArtCele/
        file_path = project_dir / "tests" / "test.cele";
        std::cout << file_path << '\n';
    }

    if (!fs::exists(file_path))
        throw std::runtime_error("File not found: " + file_path.string());

    std::ifstream source_file(file_path, std::ios::binary);
    if (!source_file)
        throw std::runtime_error("Failed to open file: " + file_path.string());

    const std::string source_code((std::istreambuf_iterator<char>(source_file)),
                                  std::istreambuf_iterator<char>());

    Lexer lexer(source_code);
    const auto tokens = lexer.lex();

    for (const Token& token : tokens)
        std::cout << token << '\n';
}
