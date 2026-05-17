#pragma once

#include "Token.hpp"
#include "AST.hpp"
#include <vector>
#include <string>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::vector<StmtPtr> parse();

    bool hadError() const { return hadError_; }

private:
    std::vector<Token> tokens_;
    size_t current_ = 0;
    bool   hadError_ = false;

    const Token& peek() const;
    const Token& previous() const;
    bool  isAtEnd() const;
    Token advance();
    bool  check(TokenType type) const;
    bool  match(TokenType type);
    Token consume(TokenType type, const std::string& msg);
    void  error(const Token& token, const std::string& msg);
    void  synchronize();

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
