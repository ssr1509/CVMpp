#pragma once

#include <cstdint>
#include <string>

enum class Opcode : uint8_t {

    OP_CONSTANT,        
    OP_TRUE,            
    OP_FALSE,           

    OP_ADD,             
    OP_SUB,             
    OP_MUL,             
    OP_DIV,             
    OP_NEGATE,          

    OP_EQUAL,           
    OP_NOT_EQUAL,       
    OP_LESS,            
    OP_LESS_EQUAL,      
    OP_GREATER,         
    OP_GREATER_EQUAL,   
    OP_NOT,             
    OP_AND,             
    OP_OR,              

    OP_DEFINE_GLOBAL,   
    OP_GET_GLOBAL,      
    OP_SET_GLOBAL,      

    OP_GET_LOCAL,       
    OP_SET_LOCAL,       

    OP_JUMP,            
    OP_JUMP_IF_FALSE,   
    OP_LOOP,            

    OP_PRINT,           
    OP_INPUT,           

    OP_POP,             

    OP_RETURN,          
};

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
