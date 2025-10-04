#include "lexer.hpp"

#include <cctype>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

std::ostream& operator<<(std::ostream& os, Token token) {
    static constexpr std::string_view token_strings[] = {
        "Null",         "Plus",   "Minus",     "Star",       "Slash",     "Equal",
        "EqualEqual",   "Bang",   "BangEqual", "Less",       "LessEqual", "Greater",
        "GreaterEqual", "LParen", "RParen",    "LBrace",     "RBrace",    "Semicolon",
        "Comma",        "Number", "String",    "Identifier", "EOF",
    };
    os << token_strings[static_cast<size_t>(token.type)];
    if (token.type == TokenType::String || token.type == TokenType::Identifier ||
        token.type == TokenType::Number)
        os << "(\"" << token.value << "\")";
    return os;
}

std::string Token::get_lexeme() const {
    switch (type) {
    case TokenType::Plus: return "+";
    case TokenType::Minus: return "-";
    case TokenType::Star: return "*";
    case TokenType::Slash: return "/";
    case TokenType::LParen: return "(";
    case TokenType::RParen: return ")";
    case TokenType::Number:
    case TokenType::Identifier: return value;
    default: throw std::runtime_error("Unknown token type");
    }
}

void Lexer::lex_space() {
    while (isspace(peek())) next();
}

void Lexer::lex_number() {
    const size_t begin_index = pos;
    while (isdigit(peek())) next();
    const size_t      size = pos - begin_index;
    const std::string number = text.substr(begin_index, size);
    emit(TokenType::Number, number);
}

void Lexer::lex_identifier() {
    std::string  identifier;
    const size_t begin_index = pos;
    next();
    while (is_identifier_part(peek())) next();

    const size_t      size = pos - begin_index;
    const std::string str = text.substr(begin_index, size);
    emit(TokenType::Identifier, str);
}

char Lexer::parse_escape() {
    switch (consume()) {
    case '"': return '"';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'b': return '\b';
    case 'f': return '\f';
    case 'v': return '\v';
    case 'a': return '\a';
    case '\\': return '\\';
    default: throw std::runtime_error("Invalid escape sequence.");
    }
}

void Lexer::lex_string() {
    std::string str;
    next(); // skip opening quote
    while (true) {
        if (pos >= text_length) throw std::runtime_error("Missing terminating \" character.");
        if (peek() == '"') break;

        switch (peek()) {
        case '\n': throw std::runtime_error("Missing terminating \" character.");
        case '\\':
            next(); // consume backslash
            str += parse_escape();
            break;
        default: str += consume(); break;
        }
    }

    emit(TokenType::String, str);
}

void Lexer::skip_line_comment() {
    while (pos < text_length && peek() != '\n') next();
}

void Lexer::skip_block_comment() {
    next(); // consume '/'
    next(); // consume '*'
    while (!(peek() == '*' && peek_ahead() == '/')) {
        if (pos >= text_length) throw std::runtime_error("Unterminated multi-line comment.");
        next();
    }
    next(); // consume '/'
}

// TODO: add more TODOs
std::vector<Token> Lexer::lex() {
    while (pos < text_length) {
        if (isspace(peek())) lex_space();
        if (isdigit(peek())) lex_number();
        if (is_identifier_start(peek())) lex_identifier();

        auto op_eq = [this](TokenType s, TokenType d) {
            if (peek_ahead() == '=') {
                next();
                emit(d);
            } else {
                emit(s);
            }
        };

        switch (peek()) {
        case '+': emit(TokenType::Plus); break;
        case '-': emit(TokenType::Minus); break;
        case '*': emit(TokenType::Star); break;
        case '=': op_eq(TokenType::Equal, TokenType::EqualEqual); break;
        case '!': op_eq(TokenType::Bang, TokenType::BangEqual); break;
        case '<': op_eq(TokenType::Less, TokenType::LessEqual); break;
        case '>': op_eq(TokenType::Greater, TokenType::GreaterEqual); break;
        case '(': emit(TokenType::LParen); break;
        case ')': emit(TokenType::RParen); break;
        case '{': emit(TokenType::LBrace); break;
        case '}': emit(TokenType::RBrace); break;
        case ';': emit(TokenType::Semicolon); break;
        case ',': emit(TokenType::Comma); break;
        case '"': lex_string(); break;
        case '/':
            if (peek_ahead() == '/') {
                skip_line_comment();
                break;
            } else if (peek_ahead() == '*') {
                skip_block_comment();
                break;
            }
            emit(TokenType::Slash);
            break;
        }

        next();
    }

    emit(TokenType::EndOfFile);
    return tokens;
}
