#include "parser.hpp"

#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>

// might wanna put in parser.hpp later
enum class TokenType {
    Null,
    Plus,
    Minus,
    Star,
    Slash,
    Number,
};

struct Token {
    Token(TokenType t) : type(t) {}
    Token(int value) : type(TokenType::Number), number_value(value) {}

    TokenType type;
    int number_value;
    
    friend std::ostream& operator<<(std::ostream& os, Token token) {
        // TODO: make this all actually maintanable lol
        if (token.type == TokenType::Number) {
            os << "Number(" << token.number_value << ')';
        } else {
            // TODO: optimize, improve
            const std::vector<std::string> token_strings = {
                "Null",
                "Plus",
                "Minus",
                "Star",
                "Slash",
            };
            os << token_strings[static_cast<size_t>(token.type)];
        }
        return os;
    }
};


int main(int argc, const char* argv[]) {
    std::string file_name;
    if (argc > 1)
        file_name = argv[1];
    else
        file_name = "../tests/begin.cele";

    std::ifstream source_file {file_name};
    const std::string source_code { std::istreambuf_iterator<char>(source_file), std::istreambuf_iterator<char>() };
    const size_t source_code_length = source_code.length();
    Parser parser(source_code);

    std::vector<Token> tokens;
    // should probably put this in the parser class later.. unsure
    while (parser.get_pos() < source_code_length) {
        while (parser.get_pos() < source_code_length && isspace(parser.peek())) 
            parser.next();


        if (parser.get_pos() < source_code_length && isdigit(parser.peek())) {
            int number = 0;
            while (parser.get_pos() < source_code_length && isdigit(parser.peek())) {
                number = number * 10 + (parser.consume() - '0');
            }
            
            tokens.emplace_back(number);
        }
        
        switch (parser.peek()) {
        case '+':
            tokens.emplace_back(TokenType::Plus);
            break;
        case '-':
            tokens.emplace_back(TokenType::Minus);
            break;
        case '*':
            tokens.emplace_back(TokenType::Star);
            break;
        case '/':
            tokens.emplace_back(TokenType::Slash);
            break;
        }

        parser.next();
    }

    for (const Token token : tokens) {
        std::cout << token << '\n';
    }
}