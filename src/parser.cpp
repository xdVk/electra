#include "parser.hpp"

#include "lexer.hpp"

#include <memory>
#include <stdexcept>

namespace expression {
void Literal::print(std::ostream& os, size_t indent) const {
    os << std::string(indent, ' ') << value << "\n";
}

void Binary::print(std::ostream& os, size_t indent) const {
    os << std::string(indent, ' ') << op << "\n";
    left->print(os, indent + 2);
    right->print(os, indent + 2);
}
}

std::shared_ptr<expression::Expression> Parser::parse() { return parse_expression(); }

// expression -> term (( "+" | "-" ) term)*
std::shared_ptr<expression::Expression> Parser::parse_expression() {
    auto lhs = parse_term();
    while (peek().type == TokenType::Plus || peek().type == TokenType::Minus) {
        Token token = consume();
        auto  rhs   = parse_term();
        lhs         = std::make_shared<expression::Binary>(lhs, token, rhs);
    }
    return lhs;
}

// term       -> factor (( "*" | "/" ) factor)*
std::shared_ptr<expression::Expression> Parser::parse_term() {
    auto lhs = parse_factor();
    while (peek().type == TokenType::Star || peek().type == TokenType::Slash) {
        Token token = consume();
        auto  rhs   = parse_factor();
        lhs         = std::make_shared<expression::Binary>(lhs, token, rhs);
    }
    return lhs;
}

// factor     -> NUMBER | "(" expression ")"
std::shared_ptr<expression::Expression> Parser::parse_factor() {
    Token t = consume();
    if (t.type == TokenType::Number)
        return std::make_shared<expression::Literal>(std::stoi(t.value));
    else if (t.type == TokenType::LParen) {
        auto expr = parse_expression();
        if (t.type != TokenType::LParen) { throw std::runtime_error("Expected ')'"); }
        return expr;
    }
    throw std::runtime_error("Expected number or '('");
}
