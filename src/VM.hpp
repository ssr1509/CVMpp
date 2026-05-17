#pragma once

#include "Chunk.hpp"
#include "Common.hpp"
#include <vector>
#include <string>
#include <unordered_map>

class VM {
public:
    enum class Result { OK, COMPILE_ERROR, RUNTIME_ERROR };

    VM();

    Result run(const Chunk& chunk, const std::vector<std::string>& globalNames);

private:
    static constexpr size_t STACK_MAX = 256;

    Value   stack_[STACK_MAX];
    Value*  sp_;               

    const uint8_t* ip_;        
    const Chunk*   chunk_;

    std::unordered_map<std::string, Value> globals_;

    const std::vector<std::string>* globalNames_;

    void   push(Value v);
    Value  pop();
    Value  peek(int distance = 0) const;

    uint8_t  readByte();
    uint16_t readShort();
    Opcode   readOpcode();

    void runtimeError(const std::string& msg);
};
