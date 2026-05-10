#pragma once

#include "Chunk.hpp"
#include "Common.hpp"
#include <vector>
#include <string>
#include <unordered_map>

// ── Virtual Machine ──────────────────────────────────────────────────────────
// Stack-based bytecode interpreter.

class VM {
public:
    enum class Result { OK, COMPILE_ERROR, RUNTIME_ERROR };

    VM();

    // Execute a compiled chunk.  `globalNames` maps constant-pool indices
    // used by DEFINE/GET/SET_GLOBAL to their string names.
    Result run(const Chunk& chunk, const std::vector<std::string>& globalNames);

private:
    static constexpr size_t STACK_MAX = 256;

    Value   stack_[STACK_MAX];
    Value*  sp_;               // stack pointer (points one past top)

    const uint8_t* ip_;        // instruction pointer
    const Chunk*   chunk_;

    // Global variable storage: name → value
    std::unordered_map<std::string, Value> globals_;

    // Name lookup table for the current chunk
    const std::vector<std::string>* globalNames_;

    // ── Stack operations ──
    void   push(Value v);
    Value  pop();
    Value  peek(int distance = 0) const;

    // ── Helpers ──
    uint8_t  readByte();
    uint16_t readShort();
    Opcode   readOpcode();

    void runtimeError(const std::string& msg);
};
