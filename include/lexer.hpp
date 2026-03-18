#pragma once

#include <cassert>
#include <cstdint>
#include <ostream>
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

struct Token {
    Token() = delete;
    Token(TokenKind kind, size_t line, size_t column, const std::string& value = "")
        : kind(kind), line(line), column(column), str_value(value), num_value(0) {}
    Token(TokenKind kind, size_t line, size_t column, long long value)
        : kind(kind), line(line), column(column), str_value(""), num_value(value) {}
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

    std::vector<Token> lex();

  private:
    [[nodiscard]] inline char peek(size_t offset = 0) const noexcept {
        return (current + offset < end) ? *(current + offset) : '\0';
    }

    inline void next(size_t n = 1) {
        assert(current + n <= end && "Lexer advanced past end of input.");

        for (size_t i = 0; i < n; i++) {
            if (peek() == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            current++;
        }
    }

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