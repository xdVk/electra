#include <cassert>
#include <iostream>
#include <string_view>
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
    Token(TokenType t) : type(t) {}
    Token(int value) : type(TokenType::Number), value(value) {}
    Token(TokenType t, std::string_view str) : type(t), value(str) {
        assert((t == TokenType::String || t == TokenType::Identifier) &&
               "Token with string attaches must be of type "
               "TokenType::String or "
               "TokenType::Identifier");
    }

    TokenType type;
    std::variant<int, std::string_view> value;

    friend std::ostream& operator<<(std::ostream& os, Token token) {
        static constexpr std::string_view token_strings[] = {
            "Null",  "Plus",      "Minus",  "Star",   "Slash",
            "Equal", "Semicolon", "Number", "String", "Identifier",
        };

        os << token_strings[static_cast<size_t>(token.type)];
        if (token.type == TokenType::Number)
            os << '(' << std::get<int>(token.value) << ')';
        else if (token.type == TokenType::String || token.type == TokenType::Identifier)
            os << "(\"" << std::get<std::string_view>(token.value) << "\")";
        return os;
    }
};

class Lexer {
public:
    Lexer(std::string_view t) : pos(0), text(t), text_length(t.length()) {}

    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;

    std::vector<Token> lex();

private:
    // TODO: safety
    [[nodiscard]] inline char peek() const { return text[pos]; }
    [[nodiscard]] inline char peek_ahead(int n) const { return text[pos + n]; }
    [[nodiscard]] inline char consume() { return text[pos++]; }
    [[nodiscard]] inline size_t get_pos() const { return pos; }
    inline void next() { pos++; }

    [[nodiscard]] inline bool is_identifier_start(char c) { return isalpha(c) || c == '_'; }
    [[nodiscard]] inline bool is_identifier_part(char c) { return isalnum(c) || c == '_'; }

    size_t pos;
    std::string_view text;
    size_t text_length;
};
