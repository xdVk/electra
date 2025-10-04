#pragma once

#include "lexer.hpp"

#include <memory>

namespace expression {
struct Expression {
    virtual ~Expression() = default;

    virtual void print(std::ostream& os, size_t indent = 0) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Expression& expr) {
        expr.print(os);
        return os;
    }
};

struct Literal : Expression {
    int value;
    explicit Literal(int v) : value(v) {}

    void print(std::ostream& os, size_t indent = 0) const override;
};

struct Binary : Expression {
    std::shared_ptr<Expression> left;
    Token                       op;
    std::shared_ptr<Expression> right;
    Binary(std::shared_ptr<Expression> l, Token o, std::shared_ptr<Expression> r)
        : left(l), op(o), right(r) {}

    void print(std::ostream& os, size_t indent = 0) const override;
};

} // namespace expression

class Parser {
public:
    explicit Parser(const std::vector<Token>& toks) : pos(0), tokens(toks) {}

    std::shared_ptr<expression::Expression> parse();

private:
    std::shared_ptr<expression::Expression> parse_expression();
    std::shared_ptr<expression::Expression> parse_term();
    std::shared_ptr<expression::Expression> parse_factor();

    [[nodiscard]] inline Token peek() const { return tokens[pos]; }
    [[nodiscard]] inline Token consume() { return tokens[pos++]; }
    inline void                next() { pos++; }

    size_t             pos;
    std::vector<Token> tokens;
};
