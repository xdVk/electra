#pragma once

#include <cassert>
#include <ostream>
#include <string>
#include <vector>

enum class TokenType {
    Null,
    Plus,
    Minus,
    Star,
    Slash,
    Equal,
    EqualEqual,
    Bang,
    BangEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,
    LParen,
    RParen,
    LBrace,
    RBrace,
    Semicolon,
    Comma,
    Number,
    String,
    Identifier,
    EndOfFile
};

struct Token {
    explicit Token(TokenType t) : type(t) {}
    explicit Token(TokenType t, std::string str) : type(t), value(str) {
        assert((t == TokenType::String || t == TokenType::Identifier || t == TokenType::Number) &&
               "Token with string attached must be String or Identifier or Number");
    }

    friend std::ostream& operator<<(std::ostream& os, Token token);

    TokenType   type;
    std::string value;
};

class Lexer {
public:
    explicit Lexer(std::string t) : pos(0), text(t), text_length(t.length()) {}
    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;

    std::vector<Token> lex();

private:
    [[nodiscard]] inline char peek() const { return pos < text_length ? text[pos] : '\0'; }
    [[nodiscard]] inline char peek_ahead(size_t n = 1) const {
        return pos + n < text_length ? text[pos + n] : '\0';
    }
    [[nodiscard]] inline char consume() { return pos < text_length ? text[pos++] : '\0'; }
    [[nodiscard]] inline bool is_identifier_start(char c) const { return isalpha(c) || c == '_'; }
    [[nodiscard]] inline bool is_identifier_part(char c) const { return isalnum(c) || c == '_'; }

    inline void next() { pos < text_length ? pos++ : 1; }
    char        parse_escape();

    inline void emit(TokenType type) { tokens.emplace_back(type); }
    inline void emit(TokenType type, const std::string& value) { tokens.emplace_back(type, value); }

    void lex_space();
    void lex_number();
    void lex_identifier();
    void lex_string();
    void skip_line_comment();
    void skip_block_comment();

    size_t             pos;
    const std::string  text;
    const size_t       text_length;
    std::vector<Token> tokens;
};
