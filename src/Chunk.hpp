#pragma once

#include "Common.hpp"
#include "Opcode.hpp"
#include <vector>
#include <cstdint>
#include <string>

class Chunk {
public:

    void writeByte(uint8_t byte);
    void writeOpcode(Opcode op);

    uint8_t addConstant(Value value);

    void writeConstant(Value value);

    size_t emitJump(Opcode jumpOp);

    void patchJump(size_t offset);

    void emitLoop(size_t loopStart);

    const std::vector<uint8_t>& code() const { return code_; }
    const std::vector<Value>&   constants() const { return constants_; }
    size_t size() const { return code_.size(); }

    void disassemble(const std::string& name) const;

private:
    std::vector<uint8_t> code_;
    std::vector<Value>   constants_;

    size_t disassembleInstruction(size_t offset) const;
};
