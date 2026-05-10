#pragma once

#include "Common.hpp"
#include "Opcode.hpp"
#include <vector>
#include <cstdint>
#include <string>

// ── Chunk ────────────────────────────────────────────────────────────────────
// A Chunk is a contiguous array of bytecode plus a parallel constant pool.

class Chunk {
public:
    // ── Bytecode emission ──
    void writeByte(uint8_t byte);
    void writeOpcode(Opcode op);

    // Add a constant and return its index in the pool.
    uint8_t addConstant(Value value);

    // Emit OP_CONSTANT + index
    void writeConstant(Value value);

    // ── Jump helpers ──
    // Emit a jump instruction with a 16-bit placeholder; returns the offset
    // of the placeholder so it can be patched later.
    size_t emitJump(Opcode jumpOp);

    // Patch a previously emitted jump placeholder at `offset` with the
    // distance from there to the current end of code.
    void patchJump(size_t offset);

    // Emit a loop instruction that jumps back to `loopStart`.
    void emitLoop(size_t loopStart);

    // ── Accessors ──
    const std::vector<uint8_t>& code() const { return code_; }
    const std::vector<Value>&   constants() const { return constants_; }
    size_t size() const { return code_.size(); }

    // ── Debug / Disassembly ──
    void disassemble(const std::string& name) const;

private:
    std::vector<uint8_t> code_;
    std::vector<Value>   constants_;

    size_t disassembleInstruction(size_t offset) const;
};
