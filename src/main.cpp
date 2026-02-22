#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>

int main(int argc, const char* argv[]) try {
    if (argc < 2)
        throw std::runtime_error("Missing input file.");

    const std::string file_name = argv[1];
    std::ifstream source_file(file_name, std::ios::in | std::ios::binary);
    if (!source_file) 
        throw std::runtime_error("Failed to open file: " + file_name);

    const std::string source_code{std::istreambuf_iterator<char>(source_file),
                                  std::istreambuf_iterator<char>()};
    
    std::cout << source_code << '\n';
} catch (const std::exception& exception) {
    std::cerr << "Error: " << exception.what() << std::endl;
    return 1;
}
