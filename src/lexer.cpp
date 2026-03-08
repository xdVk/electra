#include "lexer.hpp"

#include <cctype>
#include <vector>

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;
    Token token;

    do {
        while (std::isspace(peek())) {
            
        }


        if (std::isdigit(peek())) 
            tokens.emplace_back(lex_number());

    } while (token.kind != TokenKind::EndOfFile);
    return tokens;
}

Token Lexer::lex_number() {
    return Token(TokenKind::Number, 0, 0, "");
}
