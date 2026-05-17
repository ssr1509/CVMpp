CVMpp - Custom C++ Virtual Machine
===================================

CVMpp is a custom, fully functional Virtual Machine written in C++ (standard C++17). It compiles a custom scripting language down to bytecode and executes it using a stack-based virtual machine architecture.

HOW TO BUILD
------------
The project uses a standard Makefile. To compile the project, open your terminal in the project root directory and run:

    make

This will compile the source files inside src/ and output the `cvm` executable.
To clean the build files, run:

    make clean

HOW TO RUN
----------
You can run the VM in three ways:

1. Interactive REPL Mode
Run the executable without any arguments to start the interactive Prompt (REPL).

    ./cvm

2. Execute a Script
Provide the path to a .cvm script file to execute it.

    ./cvm examples/calculator.cvm

3. Debug Flags (Show Bytecode, AST, Tokens)
You can inspect the internal compilation pipeline by passing flags before the script name.
* --show-tokens: View the lexed tokens.
* --show-ast: View the parsed Abstract Syntax Tree.
* --show-bytecode: View the disassembled compiled bytecode.

Example: Compiling code to bytecode and running it
    ./cvm --show-bytecode examples/truth_machine.cvm
