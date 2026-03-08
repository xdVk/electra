#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cassert>

enum struct TokenKind : uint8_t {
    Invalid,
    EndOfFile,
    Plus,
    Minus,
    Star,
    Slash,
    Semicolon,
    Number,
    String,
    Identifier,
};

struct Token {
    Token() : kind(TokenKind::Invalid), line(0), column(0), value("") {}
    Token(TokenKind kind, size_t line, size_t column, const std::string& value = "")
        : kind(kind), line(line), column(column), value(value) {}
    TokenKind kind;
    size_t line;
    size_t column;
    std::string value;
};

class Lexer {
public:
    explicit Lexer(const char* start, const char* end) : start(start), current(start), end(end) {}
    
    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    Lexer& operator=(Lexer&&) = delete;

    std::vector<Token> lex();
private:
    [[nodiscard]] inline char peek() const noexcept {
        return (current < end ) ? *current : '\0';
    }
 
    inline void next(size_t n = 1) {
        // *end represents EOL
        assert(current + n <= end && "Lexer advanced past end of input"); 
        current += n;
    }

    Token lex_number();
    Token lex_string();
    Token lex_identifier();

    const char* start;
    const char* current;
    const char* end;
};
