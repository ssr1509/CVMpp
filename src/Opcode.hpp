#pragma once

#include <cstdint>
#include <string>

// ── Instruction Set Architecture ─────────────────────────────────────────────
// Every opcode is a single byte.  Operands (if any) follow immediately after
// the opcode in the bytecode stream.

enum class Opcode : uint8_t {
    // ── Constants & Literals ──
    OP_CONSTANT,        // [op][index]       push constants[index]
    OP_TRUE,            // [op]              push true
    OP_FALSE,           // [op]              push false

    // ── Arithmetic ──
    OP_ADD,             // [op]  pop b, pop a, push a+b
    OP_SUB,             // [op]  pop b, pop a, push a-b
    OP_MUL,             // [op]  pop b, pop a, push a*b
    OP_DIV,             // [op]  pop b, pop a, push a/b
    OP_NEGATE,          // [op]  pop a, push -a

    // ── Comparison / Logic ──
    OP_EQUAL,           // [op]  pop b, pop a, push a==b
    OP_NOT_EQUAL,       // [op]  pop b, pop a, push a!=b
    OP_LESS,            // [op]  pop b, pop a, push a<b
    OP_LESS_EQUAL,      // [op]  pop b, pop a, push a<=b
    OP_GREATER,         // [op]  pop b, pop a, push a>b
    OP_GREATER_EQUAL,   // [op]  pop b, pop a, push a>=b
    OP_NOT,             // [op]  pop a, push !a
    OP_AND,             // [op]  pop b, pop a, push a&&b
    OP_OR,              // [op]  pop b, pop a, push a||b

    // ── Global Variables ──
    OP_DEFINE_GLOBAL,   // [op][index]  define globals[constants[index]] = pop()
    OP_GET_GLOBAL,      // [op][index]  push globals[constants[index]]
    OP_SET_GLOBAL,      // [op][index]  globals[constants[index]] = peek(); pop()

    // ── Local Variables ──
    OP_GET_LOCAL,       // [op][slot]   push stack[slot]
    OP_SET_LOCAL,       // [op][slot]   stack[slot] = peek()

    // ── Control Flow ──
    OP_JUMP,            // [op][hi][lo]       ip += offset  (unconditional)
    OP_JUMP_IF_FALSE,   // [op][hi][lo]       if !pop() then ip += offset
    OP_LOOP,            // [op][hi][lo]       ip -= offset  (loop back)

    // ── I/O ──
    OP_PRINT,           // [op]  pop and print to stdout
    OP_INPUT,           // [op]  read integer from stdin, push

    // ── Stack manipulation ──
    OP_POP,             // [op]  discard top of stack

    // ── Halt ──
    OP_RETURN,          // [op]  end execution
};

// Pretty-print an opcode name (useful for debug/disassembly)
inline std::string opcodeName(Opcode op) {
    switch (op) {
        case Opcode::OP_CONSTANT:       return "OP_CONSTANT";
        case Opcode::OP_TRUE:           return "OP_TRUE";
        case Opcode::OP_FALSE:          return "OP_FALSE";
        case Opcode::OP_ADD:            return "OP_ADD";
        case Opcode::OP_SUB:            return "OP_SUB";
        case Opcode::OP_MUL:            return "OP_MUL";
        case Opcode::OP_DIV:            return "OP_DIV";
        case Opcode::OP_NEGATE:         return "OP_NEGATE";
        case Opcode::OP_EQUAL:          return "OP_EQUAL";
        case Opcode::OP_NOT_EQUAL:      return "OP_NOT_EQUAL";
        case Opcode::OP_LESS:           return "OP_LESS";
        case Opcode::OP_LESS_EQUAL:     return "OP_LESS_EQUAL";
        case Opcode::OP_GREATER:        return "OP_GREATER";
        case Opcode::OP_GREATER_EQUAL:  return "OP_GREATER_EQUAL";
        case Opcode::OP_NOT:            return "OP_NOT";
        case Opcode::OP_AND:            return "OP_AND";
        case Opcode::OP_OR:             return "OP_OR";
        case Opcode::OP_DEFINE_GLOBAL:  return "OP_DEFINE_GLOBAL";
        case Opcode::OP_GET_GLOBAL:     return "OP_GET_GLOBAL";
        case Opcode::OP_SET_GLOBAL:     return "OP_SET_GLOBAL";
        case Opcode::OP_GET_LOCAL:      return "OP_GET_LOCAL";
        case Opcode::OP_SET_LOCAL:      return "OP_SET_LOCAL";
        case Opcode::OP_JUMP:           return "OP_JUMP";
        case Opcode::OP_JUMP_IF_FALSE:  return "OP_JUMP_IF_FALSE";
        case Opcode::OP_LOOP:           return "OP_LOOP";
        case Opcode::OP_PRINT:          return "OP_PRINT";
        case Opcode::OP_INPUT:          return "OP_INPUT";
        case Opcode::OP_POP:            return "OP_POP";
        case Opcode::OP_RETURN:         return "OP_RETURN";
        default:                        return "UNKNOWN";
    }
}
