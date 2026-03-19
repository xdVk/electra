#pragma once

#include "lexer.hpp"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace electra {

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

struct Expression {
    Expression() = default;
    virtual ~Expression() = default;
    Expression(const Expression&) = delete;
    Expression& operator=(const Expression&) = delete;
    Expression(Expression&&) = delete;
    Expression& operator=(Expression&&) = delete;
    virtual void print(size_t indent = 0) const = 0;
};

struct Statement {
    Statement() = default;
    virtual ~Statement() = default;
    Statement(const Statement&) = delete;
    Statement& operator=(const Statement&) = delete;
    Statement(Statement&&) = delete;
    Statement& operator=(Statement&&) = delete;
    virtual void print(size_t indent = 0) const = 0;
};

struct NumberExpression : Expression {
    explicit NumberExpression(Token token) : token(std::move(token)) {}

    void print(size_t indent = 0) const override {
        std::cout << std::string(indent, ' ') << token << "\n";
    }

    Token token;
};

struct IdentifierExpression : Expression {
    explicit IdentifierExpression(Token token) : token(std::move(token)) {}
    void print(size_t indent = 0) const override {
        std::cout << std::string(indent, ' ') << token << "\n";
    }

    Token token;
};

struct UnaryExpression : Expression {
    explicit UnaryExpression(Token operation, std::unique_ptr<Expression> right)
        : operation(std::move(operation)), right(std::move(right)) {}
    Token operation;
    std::unique_ptr<Expression> right;

    void print(size_t indent = 0) const override {
        std::cout << std::string(indent, ' ') << "UnaryExpr(" << operation << ")\n";
        right->print(indent + 2);
    }
};

struct BinaryExpression : Expression {
    explicit BinaryExpression(std::unique_ptr<Expression> left, Token operation,
                              std::unique_ptr<Expression> right)
        : left(std::move(left)), operation(std::move(operation)), right(std::move(right)) {};
    std::unique_ptr<Expression> left;
    Token operation;
    std::unique_ptr<Expression> right;

    void print(size_t indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BinaryExpr(" << operation << ")\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

struct CallExpression : Expression {
    Token name;
    std::vector<std::unique_ptr<Expression>> args;

    CallExpression(Token name, std::vector<std::unique_ptr<Expression>> args)
        : name(std::move(name)), args(std::move(args)) {}

    void print(size_t indent = 0) const override {
        std::cout << std::string(indent, ' ') << "CallExpr(" << name << ")\n";
        for (const auto& arg : args) {
            arg->print(indent + 2);
        }
    }
};

struct ExpressionStatement : Statement {
    std::unique_ptr<Expression> expression;
    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}
    void print(size_t indent = 0) const override {
        expression->print(indent);
    }
};

struct LetStatement : Statement {
    explicit LetStatement(Token name, std::unique_ptr<Expression> expression)
        : identifier(std::move(name)), expression(std::move(expression)) {}

    void print(size_t indent = 0) const override {
        std::cout << std::string(indent, ' ') << "LetStatement(" << identifier << ")\n";
        expression->print(indent + 2);
    }

    Token identifier;
    std::unique_ptr<Expression> expression;
};

struct ReturnStatement : Statement {
    std::unique_ptr<Expression> expression;
    explicit ReturnStatement(std::unique_ptr<Expression> expression)
        : expression(std::move(expression)) {}

    void print(size_t indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ReturnStatement\n";
        expression->print(indent + 2);
    }
};

struct Block {
    Block() = default;
    std::vector<std::unique_ptr<Statement>> statements;

    void print(size_t indent = 0) const {
        for (const auto& statement : statements) {
            statement->print(indent + 2);
        }
    }
};

struct Parameter {
    Parameter(Token name, Token type) : name(std::move(name)), type(std::move(type)) {}
    Token name;
    Token type;
};

struct Function {
    Function(Token name, std::vector<Parameter> params, Token return_type, Block body)
        : name(std::move(name)), params(std::move(params)), return_type(std::move(return_type)),
          body(std::move(body)) {}
    Token name;
    std::vector<Parameter> params;
    Token return_type;
    Block body;

    void print(size_t indent = 0) const {
        std::cout << std::string(indent, ' ') << "Function(" << name << ") -> " << return_type
                  << "\n";

        for (const auto& [pname, ptype] : params) {
            std::cout << std::string(indent + 2, ' ') << "Param(" << pname << " : " << ptype
                      << ")\n";
        }

        body.print(indent);
    }
};

struct Program {
    std::vector<Function> functions;

    void print() const {
        for (const auto& function : functions) {
            function.print();
        }
    }
};

class Parser {
  public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser(Parser&&) = default;
    Parser& operator=(Parser&&) = default;
    ~Parser() = default;

    Program parse();

  private:
    Program parse_program();
    Function parse_function();
    std::vector<Parameter> parse_parameters();
    Parameter parse_parameter();
    Block parse_block();
    std::unique_ptr<Statement> parse_let_statement();
    std::unique_ptr<Statement> parse_return_statement();
    std::unique_ptr<Statement> parse_expression_statement();
    std::unique_ptr<Expression> parse_expression();
    std::unique_ptr<Expression> parse_addition();
    std::unique_ptr<Expression> parse_multiplication();
    std::unique_ptr<Expression> parse_unary();
    std::unique_ptr<Expression> parse_primary();
    std::vector<std::unique_ptr<Expression>> parse_args();

    [[nodiscard]] Token peek(size_t offset = 0) const noexcept;

    [[nodiscard]] bool check(TokenKind kind) const noexcept;

    Token consume();

    Token expect(TokenKind kind);

    void next(size_t offset = 1);

    std::vector<Token> tokens;
    size_t pos = 0;
};

} // namespace electra