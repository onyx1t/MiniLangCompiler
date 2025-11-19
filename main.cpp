// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Lexer.h"
#include "ErrorHandler.h"
// #include <filesystem> // Можно использовать для C++17+, но пока избегаем

// --- Configuration ---
// Использование ".." предполагает, что исполняемый файл находится в подпапке (например, cmake-build-debug),
// а папки input и output на уровень выше.
const std::string INPUT_FILE = "../input/test_input.txt";
const std::string OUTPUT_TOKEN_FILE = "../output/tokens_table.md";

int main() {
    // 1. Read the input file
    std::ifstream ifs(INPUT_FILE);
    if (!ifs.is_open()) {
        // Вывод пути для отладки
        std::cerr << "Error: Could not open input file: " << INPUT_FILE << std::endl;
        std::cerr << "Please ensure the file exists and the path is correct relative to the execution directory." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string source_code = buffer.str();

    std::cout << "--- LTLab Compiler Startup ---\n";
    std::cout << "Starting Lexical Analysis on " << INPUT_FILE << "\n\n";

    // 2. Initialize error handler
    ErrorHandler error_handler;

    // 3. Lexical Analysis
    Lexer lexer(source_code, &error_handler);
    lexer.runLexer(); // Run the lexer to generate all tokens

    // 4. Output Results

    // Print token table to console and file
    lexer.printTokenTable(std::cout);
    lexer.writeTokenTableToFile(OUTPUT_TOKEN_FILE);
    std::cout << "\nToken table saved to: " << OUTPUT_TOKEN_FILE << "\n";

    // Print any lexical errors
    if (error_handler.hasErrors()) {
        error_handler.printErrors(std::cerr);
        std::cout << "\nLexical analysis completed with errors.\n";
        return 2; // Return error code for compilation failure
    }

    std::cout << "\nLexical analysis successfully completed without errors.\n";

    // --- Next step: Parser (not yet implemented) ---
    // Here we would call the Parser:
    // Parser parser(&lexer, &error_handler);
    // ASTNode* root = parser.parseProgram();

    return 0;
}