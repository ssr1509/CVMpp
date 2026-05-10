#include "VM.hpp"
#include <iostream>
#include <stdexcept>

// ── Construction ─────────────────────────────────────────────────────────────

VM::VM() : sp_(stack_), ip_(nullptr), chunk_(nullptr), globalNames_(nullptr) {}

// ── Stack operations ─────────────────────────────────────────────────────────

void VM::push(Value v) {
    if (sp_ >= stack_ + STACK_MAX) {
        throw std::runtime_error("Stack overflow.");
    }
    *sp_++ = std::move(v);
}

Value VM::pop() {
    if (sp_ <= stack_) {
        throw std::runtime_error("Stack underflow.");
    }
    return *--sp_;
}

Value VM::peek(int distance) const {
    return *(sp_ - 1 - distance);
}

// ── Byte reading ─────────────────────────────────────────────────────────────

uint8_t VM::readByte() { return *ip_++; }

uint16_t VM::readShort() {
    uint16_t hi = static_cast<uint16_t>(readByte()) << 8;
    uint16_t lo = readByte();
    return hi | lo;
}

Opcode VM::readOpcode() { return static_cast<Opcode>(readByte()); }

void VM::runtimeError(const std::string& msg) {
    size_t offset = ip_ - chunk_->code().data() - 1;
    std::cerr << "[bytecode offset " << offset << "] Runtime Error: " << msg << "\n";
}

// ── Main execution loop ─────────────────────────────────────────────────────

VM::Result VM::run(const Chunk& chunk, const std::vector<std::string>& globalNames) {
    chunk_       = &chunk;
    globalNames_ = &globalNames;
    ip_          = chunk.code().data();
    sp_          = stack_;

    // Macro helpers for binary arithmetic / comparison
    #define BINARY_OP_INT(op)                                     \
        do {                                                      \
            Value b = pop(); Value a = pop();                     \
            push(Value{(int64_t)(asInt(a) op asInt(b))});         \
        } while (false)

    #define BINARY_CMP(op)                                        \
        do {                                                      \
            Value b = pop(); Value a = pop();                     \
            push(Value{(bool)(asInt(a) op asInt(b))});            \
        } while (false)

    while (true) {
        Opcode instruction = readOpcode();

        switch (instruction) {
            // ── Constants ──
            case Opcode::OP_CONSTANT: {
                uint8_t idx = readByte();
                push(chunk.constants()[idx]);
                break;
            }
            case Opcode::OP_TRUE:  push(Value{true});  break;
            case Opcode::OP_FALSE: push(Value{false}); break;

            // ── Arithmetic ──
            case Opcode::OP_ADD: BINARY_OP_INT(+); break;
            case Opcode::OP_SUB: BINARY_OP_INT(-); break;
            case Opcode::OP_MUL: BINARY_OP_INT(*); break;
            case Opcode::OP_DIV: {
                Value b = pop(); Value a = pop();
                if (asInt(b) == 0) {
                    runtimeError("Division by zero.");
                    return Result::RUNTIME_ERROR;
                }
                push(Value{(int64_t)(asInt(a) / asInt(b))});
                break;
            }
            case Opcode::OP_NEGATE: {
                Value a = pop();
                push(Value{(int64_t)(-asInt(a))});
                break;
            }

            // ── Comparison ──
            case Opcode::OP_EQUAL: {
                Value b = pop(); Value a = pop();
                push(Value{(bool)(a == b)});
                break;
            }
            case Opcode::OP_NOT_EQUAL: {
                Value b = pop(); Value a = pop();
                push(Value{(bool)(a != b)});
                break;
            }
            case Opcode::OP_LESS:          BINARY_CMP(<);  break;
            case Opcode::OP_LESS_EQUAL:    BINARY_CMP(<=); break;
            case Opcode::OP_GREATER:       BINARY_CMP(>);  break;
            case Opcode::OP_GREATER_EQUAL: BINARY_CMP(>=); break;

            // ── Logic ──
            case Opcode::OP_NOT: {
                Value a = pop();
                push(Value{(bool)!asBool(a)});
                break;
            }
            case Opcode::OP_AND: {
                Value b = pop(); Value a = pop();
                push(Value{(bool)(asBool(a) && asBool(b))});
                break;
            }
            case Opcode::OP_OR: {
                Value b = pop(); Value a = pop();
                push(Value{(bool)(asBool(a) || asBool(b))});
                break;
            }

            // ── Global variables ──
            case Opcode::OP_DEFINE_GLOBAL: {
                uint8_t idx = readByte();
                const std::string& name = (*globalNames_)[idx];
                globals_[name] = pop();
                break;
            }
            case Opcode::OP_GET_GLOBAL: {
                uint8_t idx = readByte();
                const std::string& name = (*globalNames_)[idx];
                auto it = globals_.find(name);
                if (it == globals_.end()) {
                    runtimeError("Undefined variable '" + name + "'.");
                    return Result::RUNTIME_ERROR;
                }
                push(it->second);
                break;
            }
            case Opcode::OP_SET_GLOBAL: {
                uint8_t idx = readByte();
                const std::string& name = (*globalNames_)[idx];
                auto it = globals_.find(name);
                if (it == globals_.end()) {
                    runtimeError("Undefined variable '" + name + "'.");
                    return Result::RUNTIME_ERROR;
                }
                it->second = peek();  // leave value on stack
                break;
            }

            // ── Local variables ──
            case Opcode::OP_GET_LOCAL: {
                uint8_t slot = readByte();
                push(stack_[slot]);
                break;
            }
            case Opcode::OP_SET_LOCAL: {
                uint8_t slot = readByte();
                stack_[slot] = peek();  // leave value on stack
                break;
            }

            // ── Control flow ──
            case Opcode::OP_JUMP: {
                uint16_t offset = readShort();
                ip_ += offset;
                break;
            }
            case Opcode::OP_JUMP_IF_FALSE: {
                uint16_t offset = readShort();
                if (!asBool(peek())) {
                    ip_ += offset;
                }
                break;
            }
            case Opcode::OP_LOOP: {
                uint16_t offset = readShort();
                ip_ -= offset;
                break;
            }

            // ── I/O ──
            case Opcode::OP_PRINT: {
                Value v = pop();
                std::cout << v << "\n";
                break;
            }
            case Opcode::OP_INPUT: {
                int64_t val;
                std::cin >> val;
                push(Value{val});
                break;
            }

            // ── Stack ──
            case Opcode::OP_POP:
                pop();
                break;

            // ── Halt ──
            case Opcode::OP_RETURN:
                return Result::OK;

            default:
                runtimeError("Unknown opcode.");
                return Result::RUNTIME_ERROR;
        }
    }

    #undef BINARY_OP_INT
    #undef BINARY_CMP
}
