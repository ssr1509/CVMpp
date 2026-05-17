#pragma once

#include "Common.hpp"
#include "Token.hpp"
#include <memory>
#include <vector>
#include <string>

struct Expr;
struct Stmt;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

struct Expr {
    virtual ~Expr() = default;
};

struct LiteralExpr : Expr {
    Value value;
    explicit LiteralExpr(Value v) : value(std::move(v)) {}
};

struct UnaryExpr : Expr {
    TokenType  op;
    ExprPtr    operand;
    UnaryExpr(TokenType o, ExprPtr e) : op(o), operand(std::move(e)) {}
};

struct BinaryExpr : Expr {
    TokenType  op;
    ExprPtr    left;
    ExprPtr    right;
    BinaryExpr(TokenType o, ExprPtr l, ExprPtr r)
        : op(o), left(std::move(l)), right(std::move(r)) {}
};

struct VariableExpr : Expr {
    std::string name;
    explicit VariableExpr(const std::string& n) : name(n) {}
};

struct AssignExpr : Expr {
    std::string name;
    ExprPtr     value;
    AssignExpr(const std::string& n, ExprPtr v) : name(n), value(std::move(v)) {}
};

struct InputExpr : Expr {};

struct GroupExpr : Expr {
    ExprPtr expression;
    explicit GroupExpr(ExprPtr e) : expression(std::move(e)) {}
};

struct Stmt {
    virtual ~Stmt() = default;
};

struct ExpressionStmt : Stmt {
    ExprPtr expression;
    explicit ExpressionStmt(ExprPtr e) : expression(std::move(e)) {}
};

struct PrintStmt : Stmt {
    ExprPtr expression;
    explicit PrintStmt(ExprPtr e) : expression(std::move(e)) {}
};

struct VarDeclStmt : Stmt {
    std::string name;
    ExprPtr     initializer;
    VarDeclStmt(const std::string& n, ExprPtr init)
        : name(n), initializer(std::move(init)) {}
};

struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
    explicit BlockStmt(std::vector<StmtPtr> stmts)
        : statements(std::move(stmts)) {}
};

struct IfStmt : Stmt {
    ExprPtr  condition;
    StmtPtr  thenBranch;
    StmtPtr  elseBranch;   
    IfStmt(ExprPtr cond, StmtPtr t, StmtPtr e)
        : condition(std::move(cond)),
          thenBranch(std::move(t)),
          elseBranch(std::move(e)) {}
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
    WhileStmt(ExprPtr cond, StmtPtr b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

inline void printAST(const Expr* expr, int indent = 0);
inline void printAST(const Stmt* stmt, int indent = 0);

inline std::string ind(int n) { return std::string(n * 2, ' '); }

inline void printAST(const Expr* expr, int indent) {
    if (!expr) { std::cout << ind(indent) << "<null>\n"; return; }

    if (auto* e = dynamic_cast<const LiteralExpr*>(expr)) {
        std::cout << ind(indent) << "Literal(" << e->value << ")\n";
    } else if (auto* e = dynamic_cast<const UnaryExpr*>(expr)) {
        std::cout << ind(indent) << "Unary(" << tokenTypeName(e->op) << ")\n";
        printAST(e->operand.get(), indent + 1);
    } else if (auto* e = dynamic_cast<const BinaryExpr*>(expr)) {
        std::cout << ind(indent) << "Binary(" << tokenTypeName(e->op) << ")\n";
        printAST(e->left.get(), indent + 1);
        printAST(e->right.get(), indent + 1);
    } else if (auto* e = dynamic_cast<const VariableExpr*>(expr)) {
        std::cout << ind(indent) << "Var(" << e->name << ")\n";
    } else if (auto* e = dynamic_cast<const AssignExpr*>(expr)) {
        std::cout << ind(indent) << "Assign(" << e->name << ")\n";
        printAST(e->value.get(), indent + 1);
    } else if (dynamic_cast<const InputExpr*>(expr)) {
        std::cout << ind(indent) << "Input()\n";
    } else if (auto* e = dynamic_cast<const GroupExpr*>(expr)) {
        std::cout << ind(indent) << "Group\n";
        printAST(e->expression.get(), indent + 1);
    }
}

inline void printAST(const Stmt* stmt, int indent) {
    if (!stmt) { std::cout << ind(indent) << "<null>\n"; return; }

    if (auto* s = dynamic_cast<const ExpressionStmt*>(stmt)) {
        std::cout << ind(indent) << "ExprStmt\n";
        printAST(s->expression.get(), indent + 1);
    } else if (auto* s = dynamic_cast<const PrintStmt*>(stmt)) {
        std::cout << ind(indent) << "PrintStmt\n";
        printAST(s->expression.get(), indent + 1);
    } else if (auto* s = dynamic_cast<const VarDeclStmt*>(stmt)) {
        std::cout << ind(indent) << "VarDecl(" << s->name << ")\n";
        printAST(s->initializer.get(), indent + 1);
    } else if (auto* s = dynamic_cast<const BlockStmt*>(stmt)) {
        std::cout << ind(indent) << "Block\n";
        for (auto& st : s->statements) printAST(st.get(), indent + 1);
    } else if (auto* s = dynamic_cast<const IfStmt*>(stmt)) {
        std::cout << ind(indent) << "If\n";
        printAST(s->condition.get(), indent + 1);
        std::cout << ind(indent) << "Then\n";
        printAST(s->thenBranch.get(), indent + 1);
        if (s->elseBranch) {
            std::cout << ind(indent) << "Else\n";
            printAST(s->elseBranch.get(), indent + 1);
        }
    } else if (auto* s = dynamic_cast<const WhileStmt*>(stmt)) {
        std::cout << ind(indent) << "While\n";
        printAST(s->condition.get(), indent + 1);
        std::cout << ind(indent) << "Body\n";
        printAST(s->body.get(), indent + 1);
    }
}
