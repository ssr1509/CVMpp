# Technical Report: CVMpp (Custom Virtual Machine in C++)

## Overview
CVMpp is a custom, fully functional Virtual Machine implementation written in C++. The project consists of a complete pipeline that takes raw source code written in a custom scripting language, tokenizes it, parses it into an Abstract Syntax Tree (AST), compiles it down into low-level bytecode, and finally executes it on a stack-based virtual machine.

The architecture is split into two primary phases: the **Frontend** (Lexing and Parsing) and the **Backend** (Compiling and Virtual Machine execution).

## 1. The Lexer (Tokenization)
The first stage of the pipeline is the Lexer (`Lexer.cpp`). 
The Lexer takes the raw string input of the source file (e.g., `let x = 1;`) and performs lexical analysis to break the text down into a sequence of meaningful chunks known as `Tokens`. It ignores whitespace and generates tokens such as `TokenType::IDENTIFIER`, `TokenType::NUMBER`, `TokenType::LET`, and operator tokens like `TokenType::PLUS` or `TokenType::EQUAL`. 

## 2. The Parser (AST Generation)
Once the code has been tokenized, the stream of tokens is fed into the Parser (`Parser.cpp`).
The Parser applies syntactic analysis based on the grammar of the custom language. It uses Recursive Descent Parsing to structure the linear token sequence into a hierarchical Abstract Syntax Tree (AST) (`AST.hpp`). For example, an `if` statement becomes a tree node containing the condition expression, the "then" branch, and the "else" branch. This phase also catches syntax errors and ensures the code is structurally valid.

## 3. The Compiler (Bytecode Emission)
The Compiler (`Compiler.cpp`) bridges the gap between the high-level AST and the low-level machine code.
It traverses the generated AST using the Visitor pattern. As it visits each node, it emits equivalent bytecode instructions into a `Chunk` (`Chunk.cpp`). A Chunk is essentially a contiguous array of raw bytes.
* **Opcodes:** The compiler emits specific 1-byte opcodes defined in `Opcode.hpp` (e.g., `OP_ADD`, `OP_PRINT`, `OP_JUMP`).
* **Constants:** When the compiler encounters literal numbers or strings, it stores them in a constant pool and emits an instruction to load them by index.
* **Control Flow:** For loops and conditionals, the compiler calculates jump offsets and emits jump instructions to manipulate the execution flow.

By the end of this phase, the high-level code has been successfully reduced to an array of executable bytecode.

## 4. The Virtual Machine (Execution)
The final stage is the Virtual Machine (`VM.cpp`), which acts as an emulator executing the compiled bytecode.
* **Stack-Based Architecture:** The VM uses a Last-In-First-Out (LIFO) stack to evaluate expressions. Instead of using registers, operations pop operands off the stack, compute the result, and push the result back onto the stack.
* **Instruction Dispatch:** The VM runs an infinite loop containing a `switch` statement that reads the bytecode array one byte at a time. It executes the logic for the given opcode (e.g., popping two values for an `OP_ADD`, adding them, and pushing the result).
* **Environment:** The VM manages global variables and tracks the instruction pointer (`ip_`) to support branching and loops.

## 5. Language Grammar
The custom language parsed by CVMpp follows a structured context-free grammar, closely resembling C or JavaScript.
It supports the following language constructs:

* **Statements**: Block statements (`{ ... }`), `if` / `else` conditionals, `while` loops, and `print` statements.
* **Declarations**: Variable declarations using `let identifier = expression;`.
* **Expressions**:
  * **Logical**: `&&`, `||`, `!`
  * **Equality**: `==`, `!=`
  * **Comparison**: `<`, `<=`, `>`, `>=`
  * **Arithmetic**: `+`, `-`, `*`, `/`
  * **Primary**: Numbers, `true`, `false`, identifiers, and `input`.

*Example Grammar Snippet (EBNF-like):*
```text
program     -> declaration* EOF
declaration -> varDecl | statement
varDecl     -> "let" IDENTIFIER "=" expression ";"
statement   -> ifStmt | whileStmt | printStmt | block | exprStmt
```

## 6. Instruction Set Architecture (ISA)
The Virtual Machine executes a custom Instruction Set Architecture (ISA). The instructions (opcodes) are 1-byte wide and are defined in `Opcode.hpp`. They can be grouped logically as follows:

* **Constants & Literals**: `OP_CONSTANT`, `OP_TRUE`, `OP_FALSE`
* **Arithmetic & Math**: `OP_ADD`, `OP_SUB`, `OP_MUL`, `OP_DIV`, `OP_NEGATE`
* **Comparison & Logic**: `OP_EQUAL`, `OP_LESS`, `OP_GREATER`, `OP_NOT`, `OP_AND`, `OP_OR` (and their inverses)
* **Variables (Global/Local)**: `OP_DEFINE_GLOBAL`, `OP_GET_GLOBAL`, `OP_SET_GLOBAL`, `OP_GET_LOCAL`, `OP_SET_LOCAL`
* **Control Flow**: `OP_JUMP`, `OP_JUMP_IF_FALSE`, `OP_LOOP`
* **I/O & Stack**: `OP_PRINT`, `OP_INPUT`, `OP_POP`, `OP_RETURN`

Because it is a stack-based ISA, arithmetic instructions like `OP_ADD` do not take register arguments. Instead, they implicitly pop the top two values off the evaluation stack, add them together, and push the result back onto the stack.

## Conclusion
CVMpp successfully demonstrates the full lifecycle of a programming language. By abstracting the execution through a Virtual Machine, the system achieves portability and clear separation of concerns, allowing for complex control flow, variable tracking, and arithmetic operations to be executed efficiently.
