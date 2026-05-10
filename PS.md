CVM++
Stack-Based Virtual Machine & Custom Compiler

Problem Statement: Most developers use high-level languages like Python,
JavaScript, or Java without deeply understanding how raw text is translated
into instructions that a computer can actually execute. This project
demystifies the compilers, bytecode, and runtime environments by building
them from scratch. This project aims to build a lightweight, custom scripting
language in C++ that compiles down to a proprietary bytecode, which is then
executed by a custom-built, stack-based Virtual Machine (VM).
Goals
Design a simple language grammar and an Instruction Set Architecture (ISA).

Build a Lexer to convert raw source code strings into Tokens (e.g., NUMBER,
PLUS, IDENTIFIER).

Build a Parser to arrange Tokens into an Abstract Syntax Tree (AST).

Implement a Compiler to flatten the AST into an array of raw Bytecode
instructions (Opcodes).

Build a Virtual Machine (VM) with a stack-based execution loop to execute the
Bytecode.
Deliverables
C++ program/library with:

Lexer and Parser modules.

Bytecode Compiler module.

Virtual Machine execution engine.

CLI-based REPL (Read-Eval-Print Loop) or File Runner demo:

Input custom language script (e.g., script.cvm).

Show the generated AST (optional debug mode).

Show the compiled Bytecode output.

Print the final execution result from the VM.

Sample test scripts demonstrating language features.

TechStack / Frameworks:
Language: C++ 

Build Tool: CMake (optional)

Concepts:

Lexical Analysis (Tokenization)

Recursive Descent Parsing and Abstract Syntax Trees (AST)

Bytecode generation & Operation Codes (Opcodes)

Stack-based execution architecture

Low-level memory management and fast dispatch loops (e.g.,
utilizing std::vector<uint8_t>)
Resources / References:
Crafting Interpreters by Robert Nystrom (The definitive guide for
this project architecture)


Tutorials

“Writing a Lexer in C++”

“Understanding Stack-Based Virtual Machines”
Scope (Keep It Simple):
Support basic data types: Integers, Booleans.

Support simple operations: +, -, *, /, ==, <.

Support basic variables and assignment (e.g., let x = 10).

Support basic control flow (e.g., simple if/else or while loops).

Provide a built-in input keyword to take inputs and a print keyword
to see outputs.