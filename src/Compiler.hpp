#pragma once

#include "AST.hpp"
#include "Chunk.hpp"
#include <string>
#include <vector>

// ── Compiler ─────────────────────────────────────────────────────────────────
// Walks the AST and emits bytecode into a Chunk.

class Compiler {
public:
    Compiler();

    // Compile a list of statements; returns true on success.
    bool compile(const std::vector<StmtPtr>& program);

    // Access the compiled chunk (valid after compile() returns true).
    Chunk& chunk() { return chunk_; }

private:
    Chunk chunk_;

    // ── Scope tracking for local variables ──
    struct Local {
        std::string name;
        int         depth;
    };
    std::vector<Local> locals_;
    int scopeDepth_ = 0;

    void beginScope();
    void endScope();
    void addLocal(const std::string& name);
    int  resolveLocal(const std::string& name);
    int  resolveGlobal(const std::string& name);

    // ── Global variable name table ──
    // Maps constant-pool index → variable name string for the VM.
    std::vector<std::string> globalNames_;

public:
    const std::vector<std::string>& globalNames() const { return globalNames_; }

private:
    // ── Statement visitors ──
    void compileStmt(const Stmt* stmt);
    void compileExpressionStmt(const ExpressionStmt* s);
    void compilePrintStmt(const PrintStmt* s);
    void compileVarDeclStmt(const VarDeclStmt* s);
    void compileBlockStmt(const BlockStmt* s);
    void compileIfStmt(const IfStmt* s);
    void compileWhileStmt(const WhileStmt* s);

    // ── Expression visitors ──
    void compileExpr(const Expr* expr);
    void compileLiteral(const LiteralExpr* e);
    void compileUnary(const UnaryExpr* e);
    void compileBinary(const BinaryExpr* e);
    void compileVariable(const VariableExpr* e);
    void compileAssign(const AssignExpr* e);
    void compileInput(const InputExpr* e);
    void compileGroup(const GroupExpr* e);
};
