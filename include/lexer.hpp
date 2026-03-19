#pragma once

#include <cassert>
#include <cstdint>
#include <format>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace electra {

#define TOKEN_KINDS                                                                                \
    X(Invalid)                                                                                     \
    X(EndOfFile)                                                                                   \
    X(Number)                                                                                      \
    X(String)                                                                                      \
    X(Identifier)                                                                                  \
    X(Fn)                                                                                          \
    X(Return)                                                                                      \
    X(Let)                                                                                         \
    X(If)                                                                                          \
    X(Else)                                                                                        \
    X(True)                                                                                        \
    X(False)                                                                                       \
    X(I32)                                                                                         \
    X(I64)                                                                                         \
    X(F32)                                                                                         \
    X(F64)                                                                                         \
    X(Bool)                                                                                        \
    X(Plus)                                                                                        \
    X(Minus)                                                                                       \
    X(Star)                                                                                        \
    X(Slash)                                                                                       \
    X(Assign)                                                                                      \
    X(Equals)                                                                                      \
    X(NotEquals)                                                                                   \
    X(Less)                                                                                        \
    X(Greater)                                                                                     \
    X(LessEq)                                                                                      \
    X(GreaterEq)                                                                                   \
    X(Colon)                                                                                       \
    X(Semicolon)                                                                                   \
    X(Comma)                                                                                       \
    X(Arrow)                                                                                       \
    X(FatArrow)                                                                                    \
    X(LeftParen)                                                                                   \
    X(RightParen)                                                                                  \
    X(LeftBrace)                                                                                   \
    X(RightBrace)                                                                                  \
    X(LeftBracket)                                                                                 \
    X(RightBracket)

enum struct TokenKind : uint8_t {
#define X(name) name,
    TOKEN_KINDS
#undef X
};

std::ostream& operator<<(std::ostream& os, TokenKind kind);

struct Token {
    Token() = delete;
    Token(TokenKind kind, size_t line, size_t column, std::string value = "")
        : kind(kind), line(line), column(column), str_value(std::move(value)), num_value(0) {}
    Token(TokenKind kind, size_t line, size_t column, long long value)
        : kind(kind), line(line), column(column), num_value(value) {}
    TokenKind kind;
    size_t line;
    size_t column;
    std::string str_value;
    long long num_value;

    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

class Lexer {
  public:
    explicit Lexer(const char* start, const char* end) : current(start), end(end) {}

    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer(Lexer&&) = default;
    Lexer& operator=(Lexer&&) = default;
    ~Lexer() = default;

    std::vector<Token> lex();

  private:
    [[nodiscard]] inline char peek(size_t offset = 0) const noexcept;

    inline void next(size_t offset = 1);

    Token lex_number();
    Token lex_string();
    Token lex_identifier();

    // const char* start;
    const char* current;
    const char* end;

    size_t line = 1;
    size_t column = 1;
};

} // namespace electra

template <> struct std::formatter<electra::Token> : std::formatter<std::string> {
    auto format(const electra::Token& token, std::format_context& ctx) const {
        std::ostringstream os;
        os << token;
        return std::formatter<std::string>::format(os.str(), ctx);
    }
};