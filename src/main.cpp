#include "lexer.hpp"

#include <exception>
#include <format>
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, const char* argv[]) try {
    if (argc < 2) throw std::runtime_error("Missing input file.");

    const std::string file_name = argv[1];
    std::ifstream source_file(file_name, std::ios::binary | std::ios::ate);

    if (!source_file) throw std::runtime_error(std::format("Failed to open file: {}", file_name));

    const std::streamsize source_size = source_file.tellg();
    if (source_size < 0) throw std::runtime_error("Failed to determine file size.");

    source_file.seekg(0, std::ios::beg);

    std::string buffer(static_cast<size_t>(source_size), '\0');
    if (!source_file.read(buffer.data(), source_size))
        throw std::runtime_error("Failed to read file.");

    const char* start = buffer.data();
    const char* end = buffer.data() + buffer.size();

    electra::Lexer lexer(start, end);
    auto tokens = lexer.lex();
    for (const auto& token : tokens) {
        std::cout << token << '\n';
    }
} catch (const std::exception& exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
    return 1;
}
