#include "Compiler.hpp"
#include <iostream>
#include <stdexcept>

// ── Construction ─────────────────────────────────────────────────────────────

Compiler::Compiler() {}

bool Compiler::compile(const std::vector<StmtPtr>& program) {
    for (auto& stmt : program) {
        compileStmt(stmt.get());
    }
    chunk_.writeOpcode(Opcode::OP_RETURN);
    return true;
}

// ── Scope management ─────────────────────────────────────────────────────────

void Compiler::beginScope() { scopeDepth_++; }

void Compiler::endScope() {
    scopeDepth_--;
    // Pop locals that belong to the scope we just left
    while (!locals_.empty() && locals_.back().depth > scopeDepth_) {
        chunk_.writeOpcode(Opcode::OP_POP);
        locals_.pop_back();
    }
}

void Compiler::addLocal(const std::string& name) {
    locals_.push_back({name, scopeDepth_});
}

int Compiler::resolveLocal(const std::string& name) {
    // Walk backwards so inner scopes shadow outer ones
    for (int i = static_cast<int>(locals_.size()) - 1; i >= 0; i--) {
        if (locals_[i].name == name) return i;
    }
    return -1;  // not a local → treat as global
}

int Compiler::resolveGlobal(const std::string& name) {
    for (int i = 0; i < (int)globalNames_.size(); i++) {
        if (globalNames_[i] == name) return i;
    }
    // If not found, allocate a new global slot
    globalNames_.push_back(name);
    return static_cast<int>(globalNames_.size() - 1);
}

// ── Statement compilation ────────────────────────────────────────────────────

void Compiler::compileStmt(const Stmt* stmt) {
    if (auto* s = dynamic_cast<const ExpressionStmt*>(stmt))
        return compileExpressionStmt(s);
    if (auto* s = dynamic_cast<const PrintStmt*>(stmt))
        return compilePrintStmt(s);
    if (auto* s = dynamic_cast<const VarDeclStmt*>(stmt))
        return compileVarDeclStmt(s);
    if (auto* s = dynamic_cast<const BlockStmt*>(stmt))
        return compileBlockStmt(s);
    if (auto* s = dynamic_cast<const IfStmt*>(stmt))
        return compileIfStmt(s);
    if (auto* s = dynamic_cast<const WhileStmt*>(stmt))
        return compileWhileStmt(s);

    throw std::runtime_error("Compiler: unknown statement type.");
}

void Compiler::compileExpressionStmt(const ExpressionStmt* s) {
    compileExpr(s->expression.get());
    chunk_.writeOpcode(Opcode::OP_POP);  // discard the value
}

void Compiler::compilePrintStmt(const PrintStmt* s) {
    compileExpr(s->expression.get());
    chunk_.writeOpcode(Opcode::OP_PRINT);
}

void Compiler::compileVarDeclStmt(const VarDeclStmt* s) {
    if (s->initializer) {
        compileExpr(s->initializer.get());
    } else {
        // default to 0
        chunk_.writeConstant(Value{(int64_t)0});
    }

    if (scopeDepth_ > 0) {
        // Local variable: the value is already on the stack at the right slot
        addLocal(s->name);
    } else {
        // Global variable: register the name and use its index
        int idx = resolveGlobal(s->name);
        chunk_.writeOpcode(Opcode::OP_DEFINE_GLOBAL);
        chunk_.writeByte(static_cast<uint8_t>(idx));
    }
}

void Compiler::compileBlockStmt(const BlockStmt* s) {
    beginScope();
    for (auto& st : s->statements) {
        compileStmt(st.get());
    }
    endScope();
}

void Compiler::compileIfStmt(const IfStmt* s) {
    // Compile condition
    compileExpr(s->condition.get());

    // Emit conditional jump (skip then-branch if false)
    size_t thenJump = chunk_.emitJump(Opcode::OP_JUMP_IF_FALSE);
    chunk_.writeOpcode(Opcode::OP_POP);  // pop condition from stack

    // Compile then-branch
    compileStmt(s->thenBranch.get());

    // Emit unconditional jump over else-branch
    size_t elseJump = chunk_.emitJump(Opcode::OP_JUMP);

    // Patch the conditional jump to land here
    chunk_.patchJump(thenJump);
    chunk_.writeOpcode(Opcode::OP_POP);  // pop condition from stack

    // Compile else-branch (if present)
    if (s->elseBranch) {
        compileStmt(s->elseBranch.get());
    }

    // Patch unconditional jump
    chunk_.patchJump(elseJump);
}

void Compiler::compileWhileStmt(const WhileStmt* s) {
    size_t loopStart = chunk_.size();

    // Compile condition
    compileExpr(s->condition.get());

    // Jump out if false
    size_t exitJump = chunk_.emitJump(Opcode::OP_JUMP_IF_FALSE);
    chunk_.writeOpcode(Opcode::OP_POP);  // pop condition

    // Compile body
    compileStmt(s->body.get());

    // Loop back to condition
    chunk_.emitLoop(loopStart);

    // Patch exit jump
    chunk_.patchJump(exitJump);
    chunk_.writeOpcode(Opcode::OP_POP);  // pop condition
}

// ── Expression compilation ───────────────────────────────────────────────────

void Compiler::compileExpr(const Expr* expr) {
    if (auto* e = dynamic_cast<const LiteralExpr*>(expr))
        return compileLiteral(e);
    if (auto* e = dynamic_cast<const UnaryExpr*>(expr))
        return compileUnary(e);
    if (auto* e = dynamic_cast<const BinaryExpr*>(expr))
        return compileBinary(e);
    if (auto* e = dynamic_cast<const VariableExpr*>(expr))
        return compileVariable(e);
    if (auto* e = dynamic_cast<const AssignExpr*>(expr))
        return compileAssign(e);
    if (auto* e = dynamic_cast<const InputExpr*>(expr))
        return compileInput(e);
    if (auto* e = dynamic_cast<const GroupExpr*>(expr))
        return compileGroup(e);

    throw std::runtime_error("Compiler: unknown expression type.");
}

void Compiler::compileLiteral(const LiteralExpr* e) {
    if (auto* b = std::get_if<bool>(&e->value)) {
        chunk_.writeOpcode(*b ? Opcode::OP_TRUE : Opcode::OP_FALSE);
    } else {
        chunk_.writeConstant(e->value);
    }
}

void Compiler::compileUnary(const UnaryExpr* e) {
    compileExpr(e->operand.get());
    switch (e->op) {
        case TokenType::MINUS: chunk_.writeOpcode(Opcode::OP_NEGATE); break;
        case TokenType::BANG:  chunk_.writeOpcode(Opcode::OP_NOT);    break;
        default:
            throw std::runtime_error("Compiler: unknown unary operator.");
    }
}

void Compiler::compileBinary(const BinaryExpr* e) {
    compileExpr(e->left.get());
    compileExpr(e->right.get());

    switch (e->op) {
        case TokenType::PLUS:          chunk_.writeOpcode(Opcode::OP_ADD);           break;
        case TokenType::MINUS:         chunk_.writeOpcode(Opcode::OP_SUB);           break;
        case TokenType::STAR:          chunk_.writeOpcode(Opcode::OP_MUL);           break;
        case TokenType::SLASH:         chunk_.writeOpcode(Opcode::OP_DIV);           break;
        case TokenType::EQUAL_EQUAL:   chunk_.writeOpcode(Opcode::OP_EQUAL);         break;
        case TokenType::BANG_EQUAL:    chunk_.writeOpcode(Opcode::OP_NOT_EQUAL);     break;
        case TokenType::LESS:          chunk_.writeOpcode(Opcode::OP_LESS);          break;
        case TokenType::LESS_EQUAL:    chunk_.writeOpcode(Opcode::OP_LESS_EQUAL);    break;
        case TokenType::GREATER:       chunk_.writeOpcode(Opcode::OP_GREATER);       break;
        case TokenType::GREATER_EQUAL: chunk_.writeOpcode(Opcode::OP_GREATER_EQUAL); break;
        case TokenType::AND:           chunk_.writeOpcode(Opcode::OP_AND);           break;
        case TokenType::OR:            chunk_.writeOpcode(Opcode::OP_OR);            break;
        default:
            throw std::runtime_error("Compiler: unknown binary operator.");
    }
}

void Compiler::compileVariable(const VariableExpr* e) {
    int slot = resolveLocal(e->name);
    if (slot != -1) {
        chunk_.writeOpcode(Opcode::OP_GET_LOCAL);
        chunk_.writeByte(static_cast<uint8_t>(slot));
    } else {
        // Global: find name index
        int idx = resolveGlobal(e->name);
        chunk_.writeOpcode(Opcode::OP_GET_GLOBAL);
        chunk_.writeByte(static_cast<uint8_t>(idx));
    }
}

void Compiler::compileAssign(const AssignExpr* e) {
    compileExpr(e->value.get());

    int slot = resolveLocal(e->name);
    if (slot != -1) {
        chunk_.writeOpcode(Opcode::OP_SET_LOCAL);
        chunk_.writeByte(static_cast<uint8_t>(slot));
    } else {
        int idx = resolveGlobal(e->name);
        chunk_.writeOpcode(Opcode::OP_SET_GLOBAL);
        chunk_.writeByte(static_cast<uint8_t>(idx));
    }
}

void Compiler::compileInput(const InputExpr*) {
    chunk_.writeOpcode(Opcode::OP_INPUT);
}

void Compiler::compileGroup(const GroupExpr* e) {
    compileExpr(e->expression.get());
}
