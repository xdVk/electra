#pragma once
#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

enum class TokenType {
    Null,
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    Semicolon,
    Number,
    String,
    Identifier,
};

struct Token {
    explicit Token(TokenType t) : type(t) {}
    explicit Token(int v) : type(TokenType::Number), value(v) {}
    explicit Token(TokenType t, std::string str) : type(t), value(str) {
        assert((t == TokenType::String || t == TokenType::Identifier) &&
               "Token with string must be String or Identifier");
    }

    friend std::ostream& operator<<(std::ostream& os, Token token);

    TokenType type;
    std::variant<int, std::string> value;
};

class Lexer {
public:
    explicit Lexer(std::string t) : pos(0), text(t), text_length(t.length()) {}
    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;

    std::vector<Token> lex();

private:
    [[nodiscard]] inline char peek() const { return pos < text_length ? text[pos] : '\0'; }
    [[nodiscard]] inline char peek_ahead(size_t n) const {
        return pos + n < text_length ? text[pos + n] : '\0';
    }
    [[nodiscard]] inline char consume() { return pos < text_length ? text[pos++] : '\0'; }
    [[nodiscard]] inline bool is_identifier_start(char c) const {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
    }
    [[nodiscard]] inline bool is_identifier_part(char c) const {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    inline void next() {
        if (pos < text_length)
            ++pos;
    }
    char parse_escape();

    void lex_number();
    void lex_identifier();
    void lex_string();
    void skip_comment();

    size_t pos;
    std::string text;
    const size_t text_length;
    std::vector<Token> tokens;
};
