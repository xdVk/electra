#include "lexer.hpp"
#include <cctype>
#include <stdexcept>

std::ostream& operator<<(std::ostream& os, Token token) {
    static constexpr std::string_view token_strings[] = {
        "Null",  "Plus",      "Minus",  "Star",   "Slash",
        "Equal", "Semicolon", "Number", "String", "Identifier",
    };

    os << token_strings[static_cast<size_t>(token.type)];
    if (token.type == TokenType::Number)
        os << '(' << std::get<int>(token.value) << ')';
    else if (token.type == TokenType::String || token.type == TokenType::Identifier)
        os << "(\"" << std::get<std::string>(token.value) << "\")";
    return os;
}

void Lexer::lex_number() {
    if (pos < text_length) {
        int number = 0;
        while (pos < text_length && isdigit(peek()))
            number = number * 10 + (consume() - '0');

        tokens.emplace_back(number);
    }
}

void Lexer::lex_identifier() {
    if (pos < text_length) {
        std::string identifier;
        const size_t begin_index = pos;
        next();
        while (pos < text_length && is_identifier_part(peek()))
            next();

        const size_t size = pos - begin_index;
        std::string str(&text[begin_index], size);
        tokens.emplace_back(TokenType::Identifier, str);
    }
}

char Lexer::parse_escape() {
    next(); // consume backslash
    switch (peek()) {
    case '"':
        return '"';
    case '\\':
        return '\\';
    case 'n':
        return '\n';
    case 'r':
        return '\r';
    case 't':
        return '\t';
    case 'b':
        return '\b';
    case 'f':
        return '\f';
    case 'v':
        return '\v';
    case 'a':
        return '\a';
    default:
        throw std::runtime_error("Invalid escape sequence.");
    }
}

void Lexer::lex_string() {
    std::string str;
    next(); // skip opening quote
    while (true) {
        if (pos >= text_length)
            throw std::runtime_error("Missing terminating \" character.");
        if (peek() == '"') 
            break;
        
        switch (peek()) {
        case '\n':
            throw std::runtime_error("Missing terminating \" character.");
        case '\\':
            str.push_back(parse_escape());
            next();
            break;
        default:
            str.push_back(consume());
            break;
        }
    }

    tokens.emplace_back(TokenType::String, str);
}

void Lexer::skip_comment() {
    if (peek_ahead(1) == '/') {
        // line comment
        while (pos < text_length && peek() != '\n')
            next();
    } else if (peek_ahead(1) == '*') {
        // block comment
        next(); // consume '/'
        next(); // consume '*'
        while (!(peek() == '*' && peek_ahead(1) == '/')) {
            if (pos >= text_length)
                throw std::runtime_error("Unterminated multi-line comment.");
            next();
        }
        next(); // consume '/'
    }
}

// TODO: add more TODOs
// TODO: ive GOTTEN make this safer :sob:
// TODO: split into smaller functions
// TODO: tests tests tests....
std::vector<Token> Lexer::lex() {
    while (pos < text_length) {
        while (pos < text_length && isspace(peek()))
            next();

        if (std::isdigit(peek()))
            lex_number();
        if (is_identifier_start(peek()))
            lex_identifier();

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
        case '"':
            lex_string();
            break;
        case '/':
            if (pos < text_length) {
                if (peek_ahead(1) == '/' || peek_ahead(1) == '*')
                    skip_comment();
                else
                    tokens.emplace_back(TokenType::Slash);
            }
            break;
        }

        next();
    }
    return tokens;
}
