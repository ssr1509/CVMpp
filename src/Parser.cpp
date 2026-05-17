#include "Parser.hpp"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

std::vector<StmtPtr> Parser::parse() {
    std::vector<StmtPtr> stmts;
    while (!isAtEnd()) {
        auto d = declaration();
        if (d) stmts.push_back(std::move(d));
    }
    return stmts;
}

const Token& Parser::peek() const       { return tokens_[current_]; }
const Token& Parser::previous() const   { return tokens_[current_ - 1]; }
bool Parser::isAtEnd() const             { return peek().type == TokenType::TOKEN_EOF; }

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

Token Parser::consume(TokenType type, const std::string& msg) {
    if (check(type)) return advance();
    error(peek(), msg);
    throw std::runtime_error(msg);
}

void Parser::error(const Token& token, const std::string& msg) {
    hadError_ = true;
    std::cerr << "[line " << token.line << "] Error at '"
              << token.lexeme << "': " << msg << "\n";
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::LET:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::PRINT:
                return;
            default:
                break;
        }
        advance();
    }
}

StmtPtr Parser::declaration() {
    try {
        if (match(TokenType::LET)) return varDeclaration();
        return statement();
    } catch (const std::runtime_error&) {
        synchronize();
        return nullptr;
    }
}

StmtPtr Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name after 'let'.");
    ExprPtr initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    return std::make_unique<VarDeclStmt>(name.lexeme, std::move(initializer));
}

StmtPtr Parser::statement() {
    if (match(TokenType::PRINT))      return printStatement();
    if (match(TokenType::IF))         return ifStatement();
    if (match(TokenType::WHILE))      return whileStatement();
    if (match(TokenType::LEFT_BRACE)) return blockStatement();
    return expressionStatement();
}

StmtPtr Parser::printStatement() {
    ExprPtr value = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after print value.");
    return std::make_unique<PrintStmt>(std::move(value));
}

StmtPtr Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition.");

    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }
    return std::make_unique<IfStmt>(std::move(condition),
                                     std::move(thenBranch),
                                     std::move(elseBranch));
}

StmtPtr Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");
    ExprPtr condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition.");

    StmtPtr body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

StmtPtr Parser::blockStatement() {
    std::vector<StmtPtr> stmts;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto d = declaration();
        if (d) stmts.push_back(std::move(d));
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");
    return std::make_unique<BlockStmt>(std::move(stmts));
}

StmtPtr Parser::expressionStatement() {
    ExprPtr expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

ExprPtr Parser::expression()  { return assignment(); }

ExprPtr Parser::assignment() {
    ExprPtr expr = logicOr();

    if (match(TokenType::EQUAL)) {
        ExprPtr value = assignment();   

        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            std::string name = var->name;
            return std::make_unique<AssignExpr>(name, std::move(value));
        }
        error(previous(), "Invalid assignment target.");
    }
    return expr;
}

ExprPtr Parser::logicOr() {
    ExprPtr left = logicAnd();
    while (match(TokenType::OR)) {
        TokenType op = previous().type;
        ExprPtr right = logicAnd();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::logicAnd() {
    ExprPtr left = equality();
    while (match(TokenType::AND)) {
        TokenType op = previous().type;
        ExprPtr right = equality();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::equality() {
    ExprPtr left = comparison();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL)) {
        TokenType op = previous().type;
        ExprPtr right = comparison();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::comparison() {
    ExprPtr left = term();
    while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL)) {
        TokenType op = previous().type;
        ExprPtr right = term();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::term() {
    ExprPtr left = factor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        TokenType op = previous().type;
        ExprPtr right = factor();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::factor() {
    ExprPtr left = unary();
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        TokenType op = previous().type;
        ExprPtr right = unary();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }
    return left;
}

ExprPtr Parser::unary() {
    if (match(TokenType::MINUS) || match(TokenType::BANG)) {
        TokenType op = previous().type;
        ExprPtr operand = unary();
        return std::make_unique<UnaryExpr>(op, std::move(operand));
    }
    return primary();
}

ExprPtr Parser::primary() {
    if (match(TokenType::NUMBER)) {
        int64_t val = std::stoll(previous().lexeme);
        return std::make_unique<LiteralExpr>(Value{val});
    }
    if (match(TokenType::TRUE_LIT))  return std::make_unique<LiteralExpr>(Value{true});
    if (match(TokenType::FALSE_LIT)) return std::make_unique<LiteralExpr>(Value{false});

    if (match(TokenType::INPUT)) return std::make_unique<InputExpr>();

    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(previous().lexeme);
    }

    if (match(TokenType::LEFT_PAREN)) {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression.");
        return std::make_unique<GroupExpr>(std::move(expr));
    }

    error(peek(), "Expected expression.");
    throw std::runtime_error("Expected expression.");
}
