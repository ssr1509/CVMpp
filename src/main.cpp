#include "Lexer.hpp"
#include "Parser.hpp"
#include "Compiler.hpp"
#include "VM.hpp"
#include "AST.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

// ── Flags ────────────────────────────────────────────────────────────────────

struct Options {
    bool showTokens   = false;
    bool showAST      = false;
    bool showBytecode = false;
    std::string filename;
};

// ── Run a source string through the full pipeline ────────────────────────────

static int runSource(const std::string& source, const Options& opts) {
    // 1. Lex
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    if (opts.showTokens) {
        std::cout << "== Tokens ==\n";
        for (auto& t : tokens) {
            std::cout << "  " << tokenTypeName(t.type)
                      << "  '" << t.lexeme << "'  (line " << t.line << ")\n";
        }
        std::cout << "\n";
    }

    // Check for lexer errors
    for (auto& t : tokens) {
        if (t.type == TokenType::TOKEN_ERROR) {
            std::cerr << "[line " << t.line << "] Lexer error: " << t.lexeme << "\n";
            return 1;
        }
    }

    // 2. Parse
    Parser parser(tokens);
    auto program = parser.parse();

    if (parser.hadError()) return 1;

    if (opts.showAST) {
        std::cout << "== AST ==\n";
        for (auto& s : program) {
            printAST(s.get(), 1);
        }
        std::cout << "\n";
    }

    // 3. Compile
    Compiler compiler;
    if (!compiler.compile(program)) {
        std::cerr << "Compilation failed.\n";
        return 1;
    }

    if (opts.showBytecode) {
        compiler.chunk().disassemble("main");
        std::cout << "\n";
    }

    // 4. Execute
    VM vm;
    auto result = vm.run(compiler.chunk(), compiler.globalNames());
    if (result != VM::Result::OK) return 1;

    return 0;
}

// ── REPL ─────────────────────────────────────────────────────────────────────

static void repl(const Options& opts) {
    std::cout << "CVM++ REPL  (type 'exit' to quit)\n";
    std::string line;
    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "exit") break;
        if (line.empty()) continue;

        // If the line doesn't end with ';' and doesn't start with a keyword
        // that produces a statement, wrap it as `print <expr>;` for convenience.
        // Otherwise run it verbatim.
        runSource(line, opts);
    }
}

// ── File runner ──────────────────────────────────────────────────────────────

static int runFile(const std::string& path, const Options& opts) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file '" << path << "'.\n";
        return 1;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return runSource(ss.str(), opts);
}

// ── Entry point ──────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    Options opts;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--show-tokens") == 0)    opts.showTokens   = true;
        else if (std::strcmp(argv[i], "--show-ast") == 0)  opts.showAST      = true;
        else if (std::strcmp(argv[i], "--show-bytecode") == 0) opts.showBytecode = true;
        else if (std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0) {
            std::cout << "Usage: cvm [options] [script.cvm]\n"
                      << "Options:\n"
                      << "  --show-tokens     Show lexer output\n"
                      << "  --show-ast        Show parsed AST\n"
                      << "  --show-bytecode   Show compiled bytecode\n"
                      << "  --help, -h        Show this help\n";
            return 0;
        }
        else opts.filename = argv[i];
    }

    if (opts.filename.empty()) {
        repl(opts);
        return 0;
    }

    return runFile(opts.filename, opts);
}
