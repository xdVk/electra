#include "parser.hpp"
#include "lexer.hpp"

#include <format>
#include <memory>
#include <stdexcept>
#include <vector>

namespace electra {
using enum TokenKind;

// program     = function*

// function    = 'fn' IDENTIFIER '(' params ')' '->' type block

// params      = (param (',' param)*)?
// param       = IDENTIFIER ':' type

// block       = '{' statement* '}'

// statement   = let_stmt
//             | return_stmt
//             | expression_stmt

// let_stmt    = 'let' IDENTIFIER '=' expression ';'
// return_stmt = 'return' expression ';'
// expression_stmt = expression ';'

// type        = 'i32' | 'i64' | 'f32' | 'f64' | 'bool'

// expression  = addition
// addition    = multiplication (('+' | '-') multiplication)*
// multiplication = unary (('*' | '/') unary)*
// unary       = '-' unary | primary

// primary = NUMBER
//         | IDENTIFIER ('(' args ')')?
//         | '(' expression ')'

// args    = (expression (',' expression)*)?

Program Parser::parse() {
    return parse_program();
}

Program Parser::parse_program() {
    Program program;
    while (check(Fn)) {
        program.functions.push_back(parse_function());
    }
    return program;
}

Parameter Parser::parse_parameter() {
    auto name = expect(Identifier);
    expect(Colon);
    if (!(check(I32) || check(I64) || check(F32) || check(F64) || check(Bool))) {
        throw std::runtime_error("Expected type.");
    }
    return {name, consume()};
}

std::vector<Parameter> Parser::parse_parameters() {
    std::vector<Parameter> params;
    if (check(Identifier)) {
        params.push_back(parse_parameter());
        while (check(Comma)) {
            consume();
            params.push_back(parse_parameter());
        }
    }
    return params;
}

Function Parser::parse_function() {
    expect(Fn);
    auto name = expect(Identifier);
    expect(LeftParen);
    std::vector<Parameter> params = parse_parameters();

    expect(RightParen);
    expect(Arrow);
    if (!(check(I32) || check(I64) || check(F32) || check(F64))) {
        throw std::runtime_error("Expected Type after arrow.");
    }
    auto return_type = consume();
    Block body = parse_block();

    return {name, params, return_type, std::move(body)};
}

Block Parser::parse_block() {
    Block block;
    expect(LeftBrace);
    while (!check(RightBrace) && !check(EndOfFile)) {
        if (check(Let)) {
            block.statements.push_back(parse_let_statement());
        } else if (check(Return)) {
            block.statements.push_back(parse_return_statement());
        } else {
            block.statements.push_back(parse_expression_statement());
        }
    }
    expect(RightBrace);
    return block;
}

std::unique_ptr<Statement> Parser::parse_return_statement() {
    expect(Return);
    auto expression = parse_expression();
    expect(Semicolon);
    return std::make_unique<ReturnStatement>(std::move(expression));
}

std::unique_ptr<Statement> Parser::parse_let_statement() {
    expect(Let);
    auto identifier = expect(Identifier);
    expect(Assign);
    auto expression = parse_expression();
    expect(Semicolon);
    return std::make_unique<LetStatement>(identifier, std::move(expression));
}

std::unique_ptr<Statement> Parser::parse_expression_statement() {
    auto expression = parse_expression();
    expect(Semicolon);
    return std::make_unique<ExpressionStatement>(std::move(expression));
}

std::unique_ptr<Expression> Parser::parse_expression() {
    return parse_addition();
}

std::unique_ptr<Expression> Parser::parse_addition() {
    auto left = parse_multiplication();
    while (check(Plus) || check(Minus)) {
        Token operation = consume();
        auto right = parse_multiplication();
        left = std::make_unique<BinaryExpression>(std::move(left), operation, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parse_multiplication() {
    auto left = parse_unary();
    while (check(Star) || check(Slash)) {
        Token operation = consume();
        auto right = parse_unary();
        left = std::make_unique<BinaryExpression>(std::move(left), operation, std::move(right));
    }
    return left;
}

std::unique_ptr<Expression> Parser::parse_unary() {
    if (check(Minus)) {
        auto operation = consume();
        auto right = parse_unary();
        return std::make_unique<UnaryExpression>(operation, std::move(right));
    }
    return parse_primary();
}

std::unique_ptr<Expression> Parser::parse_primary() {
    if (check(Number)) { return std::make_unique<NumberExpression>(consume()); }
    if (check(Identifier)) {
        Token name = consume();
        if (check(LeftParen)) {
            consume();
            auto args = parse_args();
            expect(RightParen);
            return std::make_unique<CallExpression>(std::move(name), std::move(args));
        }
        return std::make_unique<IdentifierExpression>(std::move(name));
    }
    if (check(LeftParen)) {
        consume();
        auto expression = parse_expression();
        expect(RightParen);
        return expression;
    }

    throw std::runtime_error(
        std::format("Expected expression at {}:{}", peek().line, peek().column));
}

std::vector<std::unique_ptr<Expression>> Parser::parse_args() {
    std::vector<std::unique_ptr<Expression>> args;
    if (!check(RightParen)) {
        args.push_back(parse_expression());
        while (check(Comma)) {
            consume();
            args.push_back(parse_expression());
        }
    }
    return args;
}
[[nodiscard]] Token Parser::peek(size_t offset) const noexcept {
    return pos + offset < tokens.size() ? tokens[pos + offset]
                                        : tokens.back(); // tokens.back() == EndOfFile
}
bool Parser::check(TokenKind kind) const noexcept {
    return peek().kind == kind;
}
Token Parser::consume() {
    const Token token = peek();
    next();
    return token;
}
Token Parser::expect(TokenKind kind) {
    if (!check(kind)) {
        Token unexpected = peek();
        Token expected = Token(kind, 0, 0);
        throw std::runtime_error(
            std::format("Unexpected token: {}, expected {}", unexpected, expected));
    }
    return consume();
}

void Parser::next(size_t offset) {
    assert(pos + offset <= tokens.size() && "Parser advanced past end of input.");
    pos += offset;
}
} // namespace electra
