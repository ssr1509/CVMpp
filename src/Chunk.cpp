#include "Chunk.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>

// ── Bytecode emission ────────────────────────────────────────────────────────

void Chunk::writeByte(uint8_t byte) {
    code_.push_back(byte);
}

void Chunk::writeOpcode(Opcode op) {
    writeByte(static_cast<uint8_t>(op));
}

uint8_t Chunk::addConstant(Value value) {
    constants_.push_back(std::move(value));
    if (constants_.size() > 256) {
        throw std::runtime_error("Too many constants in one chunk (max 256).");
    }
    return static_cast<uint8_t>(constants_.size() - 1);
}

void Chunk::writeConstant(Value value) {
    uint8_t idx = addConstant(std::move(value));
    writeOpcode(Opcode::OP_CONSTANT);
    writeByte(idx);
}

// ── Jump helpers ─────────────────────────────────────────────────────────────

size_t Chunk::emitJump(Opcode jumpOp) {
    writeOpcode(jumpOp);
    // 16-bit placeholder (big-endian)
    writeByte(0xff);
    writeByte(0xff);
    return code_.size() - 2;   // offset of the placeholder
}

void Chunk::patchJump(size_t offset) {
    // distance from just after the placeholder to the current end
    size_t jump = code_.size() - offset - 2;
    if (jump > UINT16_MAX) {
        throw std::runtime_error("Too much code to jump over.");
    }
    code_[offset]     = static_cast<uint8_t>((jump >> 8) & 0xff);
    code_[offset + 1] = static_cast<uint8_t>(jump & 0xff);
}

void Chunk::emitLoop(size_t loopStart) {
    writeOpcode(Opcode::OP_LOOP);
    // +2 for the two operand bytes we are about to emit
    size_t offset = code_.size() - loopStart + 2;
    if (offset > UINT16_MAX) {
        throw std::runtime_error("Loop body too large.");
    }
    writeByte(static_cast<uint8_t>((offset >> 8) & 0xff));
    writeByte(static_cast<uint8_t>(offset & 0xff));
}

// ── Disassembler ─────────────────────────────────────────────────────────────

void Chunk::disassemble(const std::string& name) const {
    std::cout << "== " << name << " ==\n";
    for (size_t offset = 0; offset < code_.size(); ) {
        offset = disassembleInstruction(offset);
    }
}

size_t Chunk::disassembleInstruction(size_t offset) const {
    std::cout << std::setw(4) << std::setfill('0') << offset << " ";

    Opcode op = static_cast<Opcode>(code_[offset]);
    switch (op) {
        // Simple (no operand)
        case Opcode::OP_ADD:
        case Opcode::OP_SUB:
        case Opcode::OP_MUL:
        case Opcode::OP_DIV:
        case Opcode::OP_NEGATE:
        case Opcode::OP_EQUAL:
        case Opcode::OP_NOT_EQUAL:
        case Opcode::OP_LESS:
        case Opcode::OP_LESS_EQUAL:
        case Opcode::OP_GREATER:
        case Opcode::OP_GREATER_EQUAL:
        case Opcode::OP_NOT:
        case Opcode::OP_AND:
        case Opcode::OP_OR:
        case Opcode::OP_TRUE:
        case Opcode::OP_FALSE:
        case Opcode::OP_PRINT:
        case Opcode::OP_INPUT:
        case Opcode::OP_POP:
        case Opcode::OP_RETURN:
            std::cout << opcodeName(op) << "\n";
            return offset + 1;

        // Byte operand
        case Opcode::OP_CONSTANT: {
            uint8_t idx = code_[offset + 1];
            std::cout << opcodeName(op) << "  " << (int)idx
                      << "  (" << constants_[idx] << ")\n";
            return offset + 2;
        }
        case Opcode::OP_DEFINE_GLOBAL:
        case Opcode::OP_GET_GLOBAL:
        case Opcode::OP_SET_GLOBAL: {
            uint8_t idx = code_[offset + 1];
            // the constant at idx is the variable name stored as int — we
            // just print the index for brevity
            std::cout << opcodeName(op) << "  " << (int)idx << "\n";
            return offset + 2;
        }
        case Opcode::OP_GET_LOCAL:
        case Opcode::OP_SET_LOCAL: {
            uint8_t slot = code_[offset + 1];
            std::cout << opcodeName(op) << "  slot " << (int)slot << "\n";
            return offset + 2;
        }

        // 16-bit operand (jump offset)
        case Opcode::OP_JUMP:
        case Opcode::OP_JUMP_IF_FALSE: {
            uint16_t jump = static_cast<uint16_t>(code_[offset + 1] << 8)
                          | code_[offset + 2];
            std::cout << opcodeName(op) << "  +" << jump
                      << "  -> " << (offset + 3 + jump) << "\n";
            return offset + 3;
        }
        case Opcode::OP_LOOP: {
            uint16_t jump = static_cast<uint16_t>(code_[offset + 1] << 8)
                          | code_[offset + 2];
            std::cout << opcodeName(op) << "  -" << jump
                      << "  -> " << (offset + 3 - jump) << "\n";
            return offset + 3;
        }

        default:
            std::cout << "UNKNOWN opcode " << (int)code_[offset] << "\n";
            return offset + 1;
    }
}
