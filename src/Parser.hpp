#pragma once

#include "Token.hpp"
#include "AST.hpp"
#include <vector>
#include <string>

// ── Parser ───────────────────────────────────────────────────────────────────
// Recursive-descent parser that transforms a token stream into an AST.

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    // Parse the full program → list of statements.
    std::vector<StmtPtr> parse();

    bool hadError() const { return hadError_; }

private:
    std::vector<Token> tokens_;
    size_t current_ = 0;
    bool   hadError_ = false;

    // ── Token helpers ──
    const Token& peek() const;
    const Token& previous() const;
    bool  isAtEnd() const;
    Token advance();
    bool  check(TokenType type) const;
    bool  match(TokenType type);
    Token consume(TokenType type, const std::string& msg);
    void  error(const Token& token, const std::string& msg);
    void  synchronize();

    // ── Grammar rules (recursive descent) ──
    StmtPtr declaration();
    StmtPtr varDeclaration();
    StmtPtr statement();
    StmtPtr printStatement();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr blockStatement();
    StmtPtr expressionStatement();

    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr logicOr();
    ExprPtr logicAnd();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr primary();
};
