#include "lexer.hpp"

#include <cassert>
#include <cctype>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace electra {
using enum TokenKind;

[[nodiscard]] char Lexer::peek(size_t offset) const noexcept {
    return (current + offset < end) ? *(current + offset) : '\0';
}

void Lexer::next(size_t offset) {
    assert(current + offset <= end && "Lexer advanced past end of input.");

    for (size_t i = 0; i < offset; i++) {
        if (peek() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        current++;
    }
}

std::ostream& operator<<(std::ostream& os, TokenKind kind) {
    switch (kind) {
#define X(name)                                                                                    \
    case TokenKind::name:                                                                          \
        return os << #name;
        TOKEN_KINDS
#undef X
    }
    __builtin_unreachable();
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    if (token.kind == Number) {
        os << token.num_value;
    } else if (token.kind == Identifier) {
        os << token.str_value;
    } else if (token.kind == String) {
        os << '"' << token.str_value << '"';
    } else {
        os << token.kind;
    }
    if (token.line != 0 || token.column != 0) { os << " at " << token.line << ':' << token.column; }
    return os;
}

std::vector<Token> Lexer::lex() {
    std::vector<Token> tokens;

    do {
        while (std::isspace(peek()) != 0) {
            next();
        }

        if (std::isdigit(peek()) != 0) {
            tokens.emplace_back(lex_number());
            continue;
        }

        if (std::isalpha(peek()) != 0 || peek() == '_') {
            tokens.emplace_back(lex_identifier());
            continue;
        }

        switch (peek()) {
        case '\0':
            break;
        case '"':
            tokens.emplace_back(lex_string());
            break;
        case '+':
            tokens.emplace_back(Plus, line, column);
            next();
            break;
        case '-':
            if (peek(1) == '>') {
                tokens.emplace_back(Arrow, line, column);
                next(2);
            } else {
                tokens.emplace_back(Minus, line, column);
                next();
            }
            break;
        case '*':
            tokens.emplace_back(Star, line, column);
            next();
            break;
        case '/':
            if (peek(1) == '/') {
                while (peek() != '\n' && peek() != '\0') {
                    next();
                }
            } else {
                tokens.emplace_back(Slash, line, column);
                next();
            }
            break;
        case '(':
            tokens.emplace_back(LeftParen, line, column);
            next();
            break;
        case ')':
            tokens.emplace_back(RightParen, line, column);
            next();
            break;
        case '{':
            tokens.emplace_back(LeftBrace, line, column);
            next();
            break;
        case '}':
            tokens.emplace_back(RightBrace, line, column);
            next();
            break;
        case '[':
            tokens.emplace_back(LeftBracket, line, column);
            next();
            break;
        case ']':
            tokens.emplace_back(RightBracket, line, column);
            next();
            break;
        case ';':
            tokens.emplace_back(Semicolon, line, column);
            next();
            break;
        case ',':
            tokens.emplace_back(Comma, line, column);
            next();
            break;
        case ':':
            tokens.emplace_back(Colon, line, column);
            next();
            break;
        case '=':
            if (peek(1) == '=') {
                tokens.emplace_back(Equals, line, column);
                next(2);
            } else {
                tokens.emplace_back(Assign, line, column);
                next();
            }
            break;
        case '!':
            if (peek(1) == '=') {
                tokens.emplace_back(NotEquals, line, column);
                next(2);
            } else {
                throw std::runtime_error(
                    std::format("Unexpected character '!' at {}:{}", line, column));
            }
            break;
        case '<':
            if (peek(1) == '=') {
                tokens.emplace_back(LessEq, line, column);
                next(2);
            } else {
                tokens.emplace_back(Less, line, column);
                next();
            }
            break;
        case '>':
            if (peek(1) == '=') {
                tokens.emplace_back(GreaterEq, line, column);
                next(2);
            } else {
                tokens.emplace_back(Greater, line, column);
                next();
            }
            break;
        default:
            throw std::runtime_error(
                std::format("Unknown character: '{}' at {}:{}", peek(), line, column));
        }
    } while (peek() != '\0');

    tokens.emplace_back(EndOfFile, line, column);
    return tokens;
}

Token Lexer::lex_number() {
    assert(std::isdigit(peek()) && "lex_number must receive digit character.");
    const size_t lin = line;
    const size_t col = column;
    const char* num_begin = current;
    while (std::isdigit(peek()) != 0) {
        next();
    }

    const char* num_end = current;
    const long long value = std::stoll(std::string(num_begin, num_end));
    return {Number, lin, col, value};
}

static TokenKind keyword_or_identifier(const std::string& word) {
    if (word == "fn") { return Fn; }
    if (word == "return") { return Return; }
    if (word == "let") { return Let; }
    if (word == "if") { return If; }
    if (word == "else") { return Else; }
    if (word == "true") { return True; }
    if (word == "false") { return False; }
    if (word == "i32") { return I32; }
    if (word == "i64") { return I64; }
    if (word == "f32") { return F32; }
    if (word == "f64") { return F64; }
    if (word == "bool") { return Bool; }
    return Identifier;
}

Token Lexer::lex_identifier() {
    assert((std::isalpha(peek()) || peek() == '_') &&
           "lex_identifier must receive alpha or underscore.");
    const size_t lin = line;
    const size_t col = column;
    const char* iden_begin = current;
    next();
    while (std::isalnum(peek()) != 0 || peek() == '_') {
        next();
    }

    const char* iden_end = current;
    const std::string value(iden_begin, iden_end);
    const TokenKind kind = keyword_or_identifier(value);
    return {kind, lin, col, kind == Identifier ? value : std::string {}};
}

Token Lexer::lex_string() {
    assert(peek() == '"' && "lex_string must receive '\"'.");
    const size_t lin = line;
    const size_t col = column;
    next();
    const char* str_begin = current;
    while (peek() != '"' && peek() != '\0') {
        next();
    }
    if (peek() == '\0') {
        throw std::runtime_error(std::format("Unterminated string at {}:{}", lin, col));
    }
    const std::string value(str_begin, current);
    next(); // skip trailing '"'
    return {String, lin, col, value};
}

} // namespace electra