#include "Lexer.hpp"
#include <cctype>

// ── Construction ─────────────────────────────────────────────────────────────

Lexer::Lexer(const std::string& source) : source_(source) {}

// ── Public API ───────────────────────────────────────────────────────────────

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token t = scanToken();
        tokens.push_back(t);
        if (t.type == TokenType::TOKEN_EOF || t.type == TokenType::TOKEN_ERROR)
            break;
    }
    return tokens;
}

// ── Helpers ──────────────────────────────────────────────────────────────────

bool Lexer::isAtEnd() const { return current_ >= source_.size(); }

char Lexer::advance() { return source_[current_++]; }

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_];
}

char Lexer::peekNext() const {
    if (current_ + 1 >= source_.size()) return '\0';
    return source_[current_ + 1];
}

bool Lexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    current_++;
    return true;
}

Token Lexer::makeToken(TokenType type) const {
    std::string lexeme = source_.substr(start_, current_ - start_);
    return Token(type, lexeme, line_);
}

Token Lexer::errorToken(const std::string& msg) const {
    return Token(TokenType::TOKEN_ERROR, msg, line_);
}

// ── Whitespace & Comments ────────────────────────────────────────────────────

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                line_++;
                advance();
                break;
            case '/':
                // single-line comment: // ...
                if (peekNext() == '/') {
                    while (!isAtEnd() && peek() != '\n') advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

// ── Number Literal ───────────────────────────────────────────────────────────

Token Lexer::number() {
    while (std::isdigit(peek())) advance();
    return makeToken(TokenType::NUMBER);
}

// ── Identifier / Keyword ─────────────────────────────────────────────────────

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();
    return makeToken(identifierType());
}

TokenType Lexer::identifierType() const {
    std::string text = source_.substr(start_, current_ - start_);
    if (text == "let")    return TokenType::LET;
    if (text == "if")     return TokenType::IF;
    if (text == "else")   return TokenType::ELSE;
    if (text == "while")  return TokenType::WHILE;
    if (text == "print")  return TokenType::PRINT;
    if (text == "input")  return TokenType::INPUT;
    if (text == "true")   return TokenType::TRUE_LIT;
    if (text == "false")  return TokenType::FALSE_LIT;
    return TokenType::IDENTIFIER;
}

// ── Main Scan ────────────────────────────────────────────────────────────────

Token Lexer::scanToken() {
    skipWhitespace();
    start_ = current_;

    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    char c = advance();

    // Numbers
    if (std::isdigit(c)) return number();

    // Identifiers / keywords
    if (std::isalpha(c) || c == '_') return identifier();

    // Single & multi-character tokens
    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case '+': return makeToken(TokenType::PLUS);
        case '-': return makeToken(TokenType::MINUS);
        case '*': return makeToken(TokenType::STAR);
        case '/': return makeToken(TokenType::SLASH);
        case ';': return makeToken(TokenType::SEMICOLON);

        case '!': return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=': return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<': return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>': return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);

        case '&':
            if (match('&')) return makeToken(TokenType::AND);
            return errorToken("Unexpected character '&'. Did you mean '&&'?");
        case '|':
            if (match('|')) return makeToken(TokenType::OR);
            return errorToken("Unexpected character '|'. Did you mean '||'?");

        default:
            return errorToken("Unexpected character '" + std::string(1, c) + "'");
    }
}
