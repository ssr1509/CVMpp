#pragma once

#include "AST.hpp"
#include "Chunk.hpp"
#include <string>
#include <vector>

class Compiler {
public:
    Compiler();

    bool compile(const std::vector<StmtPtr>& program);

    Chunk& chunk() { return chunk_; }

private:
    Chunk chunk_;

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

    std::vector<std::string> globalNames_;

public:
    const std::vector<std::string>& globalNames() const { return globalNames_; }

private:

    void compileStmt(const Stmt* stmt);
    void compileExpressionStmt(const ExpressionStmt* s);
    void compilePrintStmt(const PrintStmt* s);
    void compileVarDeclStmt(const VarDeclStmt* s);
    void compileBlockStmt(const BlockStmt* s);
    void compileIfStmt(const IfStmt* s);
    void compileWhileStmt(const WhileStmt* s);

    void compileExpr(const Expr* expr);
    void compileLiteral(const LiteralExpr* e);
    void compileUnary(const UnaryExpr* e);
    void compileBinary(const BinaryExpr* e);
    void compileVariable(const VariableExpr* e);
    void compileAssign(const AssignExpr* e);
    void compileInput(const InputExpr* e);
    void compileGroup(const GroupExpr* e);
};
