#include "lexer.hpp"

// TODO: ive GOTTEN make this safer :sob:
// TODO: split into smaller functions
// TODO: tests tests tests....
std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;
    while (get_pos() < text_length) {
        while (get_pos() < text_length && isspace(peek()))
            next();

        if (get_pos() < text_length && isdigit(peek())) {
            int number = 0;
            while (get_pos() < text_length && isdigit(peek()))
                number = number * 10 + (consume() - '0');

            tokens.emplace_back(number);
        }

        // TODO: MAJOR testing with this and string... its making me uneasy...
        if (get_pos() < text_length && is_identifier_start(peek())) {
            std::string identifier;
            size_t begin_index = get_pos();
            next();
            while (get_pos() < text_length && is_identifier_part(peek()))
                next();

            size_t size = get_pos() - begin_index;
            std::string_view str(&text[begin_index], size);
            tokens.emplace_back(TokenType::Identifier, str);
        }

        switch (peek()) {
        case '+':
            tokens.emplace_back(TokenType::Plus);
            break;
        case '-':
            tokens.emplace_back(TokenType::Minus);
            break;
        case '*':
            tokens.emplace_back(TokenType::Star);
            break;
        case '=':
            tokens.emplace_back(TokenType::Equal);
            break;
        case ';':
            tokens.emplace_back(TokenType::Semicolon);
            break;
        // TODO: handle escape characters
        case '"': {
            next();
            size_t begin_index = get_pos();
            while (get_pos() < text_length && peek() != '"')
                next();

            size_t size = get_pos() - begin_index;
            std::string_view str(&text[begin_index], size);
            tokens.emplace_back(TokenType::String, str);
            break;
        }
        case '/':
            if (get_pos() < text_length) {
                if (peek_ahead(1) == '/') {
                    while (get_pos() < text_length && peek() != '\n')
                        next();
                } else if (peek_ahead(1) == '*') {
                    while (get_pos() < text_length && peek() != '*' && peek_ahead(1) != '/') {
                        next();
                        next();
                    }
                    next();
                } else {
                    tokens.emplace_back(TokenType::Slash);
                }
            }
            break;
        }

        next();
    }
    return tokens;
}