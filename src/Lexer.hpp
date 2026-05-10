#pragma once

#include "Token.hpp"
#include <string>
#include <vector>

// ── Lexer ────────────────────────────────────────────────────────────────────
// Converts a raw source string into a flat vector of Tokens.

class Lexer {
public:
    explicit Lexer(const std::string& source);

    // Tokenize the entire source, ending with TOKEN_EOF.
    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t      start_   = 0;   // start of current lexeme
    size_t      current_ = 0;   // scan cursor
    int         line_    = 1;

    bool  isAtEnd() const;
    char  advance();
    char  peek() const;
    char  peekNext() const;
    bool  match(char expected);

    void  skipWhitespace();
    Token makeToken(TokenType type) const;
    Token errorToken(const std::string& msg) const;
    Token scanToken();
    Token number();
    Token identifier();
    TokenType identifierType() const;
};
