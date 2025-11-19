// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

// Включение заголовочных файлов
#include "Lexer.h"
#include "Parser.h"
#include "ErrorHandler.h"
#include "AST.h"
#include "ASTVisualizer.h"

// --- Configuration ---
const std::string INPUT_FILE = "../input/test_input.txt";
const std::string OUTPUT_TOKEN_FILE = "../output/tokens_table.md";

int main() {
    // 1. Read the input file
    std::ifstream ifs(INPUT_FILE);
    if (!ifs.is_open()) {
        std::cerr << "Error: Could not open input file: " << INPUT_FILE << std::endl;
        std::cerr << "Please ensure the file exists and the path is correct relative to the execution directory." << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string source_code = buffer.str();

    std::cout << "--- LTLab Compiler Startup ---\n";
    std::cout << "Starting Lexical and Syntax Analysis on " << INPUT_FILE << "\n";

    // 2. Initialize error handler
    ErrorHandler error_handler;

    // 3. Lexical Analysis
    Lexer lexer(source_code, &error_handler);
    lexer.runLexer(); // Запуск лексера для генерации всех токенов

    // 4. Вывод результатов лексического анализа (ВОЗВРАЩЕНЫ)

    // Вывод таблицы токенов в консоль
    lexer.printTokenTable(std::cout);

    // Сохранение таблицы токенов в файл .md
    lexer.writeTokenTableToFile(OUTPUT_TOKEN_FILE);
    std::cout << "\n[INFO] Token table saved to: " << OUTPUT_TOKEN_FILE << "\n";


    if (error_handler.hasErrors()) {
        error_handler.printErrors(std::cerr);
        std::cout << "\nLexical analysis completed with errors. Aborting compilation.\n";
        return 2;
    }

    // 5. Syntax Analysis (Parsing)
    std::unique_ptr<ASTNode> ast_root = nullptr;

    try {
        std::cout << "\n[INFO] Starting Recursive Descent Parsing...\n";
        Parser parser(&lexer, &error_handler);

        // Запуск парсера
        ast_root = parser.parseProgram();
        std::cout << "[INFO] Parsing completed successfully (EOF matched).\n";


        // Если парсинг не бросил исключения и вернул корневой узел
        if (ast_root && !error_handler.hasErrors()) {
            std::cout << "\n========================================\n";
            std::cout << "PARSING SUCCESSFULLY COMPLETED.\n";
            std::cout << "========================================\n";

            // 6. AST Visualization
            std::cout << "\n| AST Visualization:\n";
            ASTVisualizer visualizer;

            // Запускаем визуализацию с корневого узла
            ast_root->accept(visualizer);

        } else if (error_handler.hasErrors()) {
            // Если парсер регистрировал ошибки
            error_handler.printErrors(std::cerr);
            std::cout << "\nParsing aborted due to Syntax Errors.\n";
            return 3;
        }

    } catch (const std::runtime_error& e) {
        // Ловим исключение, брошенное из Parser::parseError
        std::cerr << "\nParser Runtime Error: " << e.what() << "\n";
        error_handler.printErrors(std::cerr); // Печатаем собранные ошибки
        return 3;
    }

    return 0;
}