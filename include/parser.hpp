#pragma once

#include "lexer.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>

namespace expression {
struct Expression {
    virtual ~Expression() = default;

    virtual void print(std::ostream& os, int indent = 0) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Expression& expr) {
        expr.print(os);
        return os;
    }
};

struct Primary : Expression {
    int value;
    explicit Primary(int v) : value(v) {}

    void print(std::ostream& os, int indent = 0) const override;
};

struct Unary : Expression {
    Token                       op;
    std::unique_ptr<Expression> rhs;
    Unary(Token o, std::unique_ptr<Expression> r) : op(o), rhs(std::move(r)) {}

    void print(std::ostream& os, int indent = 0) const override;
};

struct Binary : Expression {
    std::unique_ptr<Expression> lhs;
    std::unique_ptr<Expression> rhs;
    Token                       op;
    Binary(std::unique_ptr<Expression> l, Token o, std::unique_ptr<Expression> r)
        : lhs(std::move(l)), rhs(std::move(r)), op(o) {}

    void print(std::ostream& os, int indent = 0) const override;
};

struct Variable : Expression {
    std::string name;
    Variable(const std::string& n) : name(n) {}

    void print(std::ostream& os, int indent = 0) const override;
};
} // namespace expression

int evaluate(const expression::Expression* expr, const std::unordered_map<std::string, int>& vars);

class Parser {
public:
    explicit Parser(const std::vector<Token>& toks) : pos(0), tokens(toks) {}

    std::unique_ptr<expression::Expression> parse();

    // expression ::= equality-expression
    // equality-expression ::= additive-expression ( ( '==' | '!=' ) additive-expression ) *
    // additive-expression ::= multiplicative-expression ( ( '+' | '-' ) multiplicative-expression )
    // * multiplicative-expression ::= primary ( ( '*' | '/' ) primary ) * primary ::= '('
    // expression ')' | NUMBER | VARIABLE | '-' primary
private:
    std::unique_ptr<expression::Expression> parse_expression();
    std::unique_ptr<expression::Expression> parse_equality_expression();
    std::unique_ptr<expression::Expression> parse_additive_expression();
    std::unique_ptr<expression::Expression> parse_multiplicative_expression();
    std::unique_ptr<expression::Expression> parse_primary();

    [[nodiscard]] inline Token peek() const { return tokens[pos]; }
    [[nodiscard]] inline Token consume() { return tokens[pos++]; }
    inline void                next() { pos++; }

    size_t             pos;
    std::vector<Token> tokens;
};
