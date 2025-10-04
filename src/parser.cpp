#include "parser.hpp"

#include "lexer.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>

namespace expression {
void Primary::print(std::ostream& os, int indent) const {
    for (int i = 0; i < indent; i++) std::cout << "| ";
    os << value << "\n";
}

void Binary::print(std::ostream& os, int indent) const {
    for (int i = 0; i < indent; i++) std::cout << "| ";
    os << op << "\n";
    lhs->print(os, indent + 1);
    rhs->print(os, indent + 1);
}

void Unary::print(std::ostream& os, int indent) const {
    for (int i = 0; i < indent; ++i) os << "| ";
    os << op << "\n";
    rhs->print(os, indent + 1);
}

void Variable::print(std::ostream& os, int indent) const {
    for (int i = 0; i < indent; i++) std::cout << "| ";
    os << name << "\n";
}
}

int evaluate(const expression::Expression* expr, const std::unordered_map<std::string, int>& vars) {
    using namespace expression;
    if (auto primary = dynamic_cast<const Primary*>(expr)) {
        return primary->value;
    } else if (auto variable = dynamic_cast<const Variable*>(expr)) {
        auto it = vars.find(variable->name);
        if (it == vars.end()) throw std::runtime_error("Undefined variable: " + variable->name);
        return it->second;
    } else if (auto unary = dynamic_cast<const Unary*>(expr)) {
        int rhs_val = evaluate(unary->rhs.get(), vars);
        if (unary->op.type == TokenType::Minus) return -rhs_val;
    } else if (auto binary = dynamic_cast<const Binary*>(expr)) {
        int lhs_val = evaluate(binary->lhs.get(), vars);
        int rhs_val = evaluate(binary->rhs.get(), vars);

        switch (binary->op.type) {
        case TokenType::Plus: return lhs_val + rhs_val;
        case TokenType::Minus: return lhs_val - rhs_val;
        case TokenType::Star: return lhs_val * rhs_val;
        case TokenType::Slash: return lhs_val / rhs_val;
        case TokenType::EqualEqual: return lhs_val == rhs_val;
        case TokenType::BangEqual: return lhs_val != rhs_val;
        default: throw std::runtime_error("Unknown binary operator");
        }
    }

    throw std::runtime_error("Unknown expression type");
}

std::unique_ptr<expression::Expression> Parser::parse() { return parse_expression(); }

// expression ::= equality-expression
std::unique_ptr<expression::Expression> Parser::parse_expression() {
    return parse_equality_expression();
}

// equality-expression ::= additive-expression ( ( '==' | '!=' ) additive-expression ) *
std::unique_ptr<expression::Expression> Parser::parse_equality_expression() {
    auto lhs = parse_additive_expression();
    while (peek().type == TokenType::EqualEqual || peek().type == TokenType::BangEqual) {
        Token op = consume();
        auto  rhs = parse_additive_expression();
        lhs = std::make_unique<expression::Binary>(std::move(lhs), op, std::move(rhs));
    }

    return lhs;
}

// additive-expression ::= multiplicative-expression ( ( '+' | '-' ) multiplicative-expression ) *
std::unique_ptr<expression::Expression> Parser::parse_additive_expression() {
    auto lhs = parse_multiplicative_expression();
    while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
        Token op = consume();
        auto  rhs = parse_multiplicative_expression();
        lhs = std::make_unique<expression::Binary>(std::move(lhs), op, std::move(rhs));
    }

    return lhs;
}

// multiplicative-expression ::= primary ( ( '*' | '/' ) primary ) *
std::unique_ptr<expression::Expression> Parser::parse_multiplicative_expression() {
    auto lhs = parse_primary();
    while (peek().type == TokenType::Star || peek().type == TokenType::Slash) {
        Token op = consume();
        auto  rhs = parse_primary();
        lhs = std::make_unique<expression::Binary>(std::move(lhs), op, std::move(rhs));
    }

    return lhs;
}

// primary ::= '(' expression ')' | NUMBER | VARIABLE | '-' primary
std::unique_ptr<expression::Expression> Parser::parse_primary() {
    Token token = consume();
    if (token.type == TokenType::LParen) {
        auto  expr = parse_expression();
        Token closing = consume(); // eat ')'
        if (closing.type != TokenType::RParen) { throw std::runtime_error("Expected ')'"); }
        return expr;
    } else if (token.type == TokenType::Number) {
        return std::make_unique<expression::Primary>(std::stoi(token.value));
    } else if (token.type == TokenType::Identifier) {
        return std::make_unique<expression::Variable>(token.value);
    } else if (token.type == TokenType::Minus) {
        return std::make_unique<expression::Unary>(token, parse_primary());
    }
    throw std::runtime_error("Expected expression (number, variable, or '(' )");
}
