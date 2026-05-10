#pragma once

#include <string>

// ── Token Types ──────────────────────────────────────────────────────────────

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN,        // ( )
    LEFT_BRACE, RIGHT_BRACE,        // { }
    PLUS, MINUS, STAR, SLASH,       // + - * /
    SEMICOLON,                      // ;

    // One or two character tokens
    BANG, BANG_EQUAL,               // !  !=
    EQUAL, EQUAL_EQUAL,             // =  ==
    LESS, LESS_EQUAL,               // <  <=
    GREATER, GREATER_EQUAL,         // >  >=

    // Logical operators
    AND, OR,                        // && ||

    // Literals
    NUMBER,                         // integer literal
    IDENTIFIER,                     // variable name
    TRUE_LIT, FALSE_LIT,           // true false

    // Keywords
    LET,                            // let
    IF, ELSE,                       // if else
    WHILE,                          // while
    PRINT,                          // print
    INPUT,                          // input

    // Special
    TOKEN_EOF,                      // end of source
    TOKEN_ERROR,                    // lexer error
};

// ── Token ────────────────────────────────────────────────────────────────────

struct Token {
    TokenType   type;
    std::string lexeme;
    int         line;

    Token() : type(TokenType::TOKEN_EOF), lexeme(""), line(0) {}
    Token(TokenType t, const std::string& lex, int ln)
        : type(t), lexeme(lex), line(ln) {}
};

// Pretty-print helper (debug)
inline std::string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::LEFT_PAREN:     return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:    return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE:     return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE:    return "RIGHT_BRACE";
        case TokenType::PLUS:           return "PLUS";
        case TokenType::MINUS:          return "MINUS";
        case TokenType::STAR:           return "STAR";
        case TokenType::SLASH:          return "SLASH";
        case TokenType::SEMICOLON:      return "SEMICOLON";
        case TokenType::BANG:           return "BANG";
        case TokenType::BANG_EQUAL:     return "BANG_EQUAL";
        case TokenType::EQUAL:          return "EQUAL";
        case TokenType::EQUAL_EQUAL:    return "EQUAL_EQUAL";
        case TokenType::LESS:           return "LESS";
        case TokenType::LESS_EQUAL:     return "LESS_EQUAL";
        case TokenType::GREATER:        return "GREATER";
        case TokenType::GREATER_EQUAL:  return "GREATER_EQUAL";
        case TokenType::AND:            return "AND";
        case TokenType::OR:             return "OR";
        case TokenType::NUMBER:         return "NUMBER";
        case TokenType::IDENTIFIER:     return "IDENTIFIER";
        case TokenType::TRUE_LIT:       return "TRUE_LIT";
        case TokenType::FALSE_LIT:      return "FALSE_LIT";
        case TokenType::LET:            return "LET";
        case TokenType::IF:             return "IF";
        case TokenType::ELSE:           return "ELSE";
        case TokenType::WHILE:          return "WHILE";
        case TokenType::PRINT:          return "PRINT";
        case TokenType::INPUT:          return "INPUT";
        case TokenType::TOKEN_EOF:      return "EOF";
        case TokenType::TOKEN_ERROR:    return "ERROR";
        default:                        return "UNKNOWN";
    }
}
