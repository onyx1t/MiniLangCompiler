#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <memory>
#include <iomanip>
#include <vector>
#include <filesystem>

// Заголовочные файлы компилятора
#include "Lexer.h"
#include "Parser.h"
#include "ErrorHandler.h"
#include "AST.h"
#include "ASTVisualizer.h"
#include "IR.h"
#include "IRGenerator.h"
#include "IROptimizer.h"
#include "IRInterpreter.h"

// Конфигурация путей
const std::string INPUT_DIR = "../input/";
const std::string OUTPUT_BASE_DIR = "../output/";

// Создание выходной директории при необходимости
void create_directory_if_not_exists(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
    std::cout << "[INFO] Ensuring output directory exists: " << path << " (Manual check might be required)\n";
}

// Перенаправитель вывода в лог-файл
struct OutputRedirector {
    std::ofstream log_file;
    std::streambuf* original_cout_buf;
    std::streambuf* original_cerr_buf;

    OutputRedirector(const std::string& log_path) {
        log_file.open(log_path);
        if (log_file.is_open()) {
            original_cout_buf = std::cout.rdbuf();
            original_cerr_buf = std::cerr.rdbuf();
            std::cout.rdbuf(log_file.rdbuf());
            std::cerr.rdbuf(log_file.rdbuf());
        }
    }

    ~OutputRedirector() {
        if (log_file.is_open()) {
            std::cout.rdbuf(original_cout_buf);
            std::cerr.rdbuf(original_cerr_buf);
            log_file.close();
        }
    }

    bool is_open() const {
        return log_file.is_open();
    }
};

// Обработка одного файла через все этапы компиляции
int process_file(const std::string& input_filename, const std::string& output_folder_name) {
    // Определение путей к файлам
    const std::string INPUT_FILE = INPUT_DIR + input_filename;
    const std::string OUTPUT_DIR = OUTPUT_BASE_DIR + output_folder_name + "/";

    // Имена выходных файлов
    const std::string OUTPUT_ERROR_LOG = OUTPUT_DIR + "execution_errors.log";
    const std::string OUTPUT_INTERPRETER_LOG = OUTPUT_DIR + "interpreter_output.log";
    const std::string OUTPUT_TOKEN_FILE = OUTPUT_DIR + "tokens_table.md";
    const std::string OUTPUT_AST_FILE = OUTPUT_DIR + "ast_structure.txt";
    const std::string OUTPUT_IR_FILE = OUTPUT_DIR + "generated_ir.txt";
    const std::string OUTPUT_IR_OPT_FILE = OUTPUT_DIR + "optimized_ir.txt";

    create_directory_if_not_exists(OUTPUT_DIR);

    std::cout << "\n\n================================================\n";
    std::cout << ">>> STARTING COMPILATION FOR: " << input_filename << " <<<\n";
    std::cout << ">>> RESULTS TO FOLDER: " << output_folder_name << " <<<\n";
    std::cout << "================================================\n";

    // Чтение исходного кода
    std::ifstream ifs(INPUT_FILE);
    if (!ifs.is_open()) {
        std::cerr << "Error: Could not open input file: " << INPUT_FILE << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    std::string source_code = buffer.str();

    // Инициализация обработчика ошибок
    ErrorHandler error_handler;

    // Лексический анализ
    std::cout << "\n========================================\n";
    std::cout << "1. STARTING LEXICAL ANALYSIS\n";
    std::cout << "========================================\n";
    Lexer lexer(source_code, &error_handler);
    lexer.runLexer();

    // Сохранение таблицы токенов
    std::cout << "[INFO] Saving Token Table to: " << OUTPUT_TOKEN_FILE << "\n";
    lexer.writeTokenTableToFile(OUTPUT_TOKEN_FILE);

    if (error_handler.hasErrors()) {
        {
            OutputRedirector error_redirector(OUTPUT_ERROR_LOG);
            if (error_redirector.is_open()) {
                error_handler.printErrors(std::cerr);
                std::cerr << "\n[FATAL] Lexical analysis failed for " << input_filename << ". Aborting compilation.\n";
            }
        }
        std::cerr << "\n[FATAL] Lexical analysis failed for " << input_filename << ". See " << OUTPUT_ERROR_LOG << " for details.\n";
        return 2;
    }
    std::cout << "[INFO] Lexical analysis completed successfully.\n";

    // Синтаксический анализ
    std::unique_ptr<ASTNode> ast_root = nullptr;
    try {
        std::cout << "\n========================================\n";
        std::cout << "2. STARTING SYNTAX ANALYSIS (Parsing AST)\n";
        std::cout << "========================================\n";
        Parser parser(&lexer, &error_handler);
        ast_root = parser.parseProgram();

        if (error_handler.hasErrors()) {
            {
                OutputRedirector error_redirector(OUTPUT_ERROR_LOG);
                if (error_redirector.is_open()) {
                    error_handler.printErrors(std::cerr);
                    std::cerr << "\n[FATAL] Parsing aborted for " << input_filename << " due to Syntax Errors.\n";
                }
            }
            std::cerr << "\n[FATAL] Parsing aborted for " << input_filename << ". See " << OUTPUT_ERROR_LOG << " for details.\n";
            return 3;
        }

        if (ast_root) {
            std::cout << "[INFO] Parsing completed successfully.\n";

            // Визуализация AST
            std::cout << "[INFO] Saving Abstract Syntax Tree (AST) to: " << OUTPUT_AST_FILE << "\n";
            std::ofstream ofs_ast(OUTPUT_AST_FILE);
            if (!ofs_ast.is_open()) {
                std::cerr << "[WARNING] Could not open file for AST visualization: " << OUTPUT_AST_FILE << "\n";
            } else {
                std::streambuf* cout_buf = std::cout.rdbuf();
                std::cout.rdbuf(ofs_ast.rdbuf());
                ASTVisualizer visualizer;
                ast_root->accept(visualizer);
                std::cout.rdbuf(cout_buf);
                ofs_ast.close();
            }

            // Генерация промежуточного кода
            std::cout << "\n========================================\n";
            std::cout << "3. STARTING INTERMEDIATE CODE GENERATION\n";
            std::cout << "========================================\n";
            IRGenerator ir_generator(&error_handler);
            IRCode generated_code = ir_generator.generate(ast_root.get());

            std::cout << "[INFO] Saving Generated 3-Address Code (3AC) to: " << OUTPUT_IR_FILE << "\n";
            std::ofstream ofs_gen(OUTPUT_IR_FILE);
            if (ofs_gen.is_open()) {
                for (const auto& instr : generated_code) {
                    ofs_gen << instr.toString() << "\n";
                }
                ofs_gen.close();
            } else {
                std::cerr << "[WARNING] Could not open file for IR generation: " << OUTPUT_IR_FILE << "\n";
            }

            // Оптимизация промежуточного кода
            std::cout << "\n========================================\n";
            std::cout << "4. STARTING IR OPTIMIZATION\n";
            std::cout << "========================================\n";
            IROptimizer ir_optimizer;
            IRCode optimized_code = ir_optimizer.optimize(generated_code);

            std::cout << "[INFO] Saving Optimized 3-Address Code (3AC) to: " << OUTPUT_IR_OPT_FILE << "\n";
            std::ofstream ofs_opt(OUTPUT_IR_OPT_FILE);
            if (ofs_opt.is_open()) {
                for (const auto& instr : optimized_code) {
                    ofs_opt << instr.toString() << "\n";
                }
                ofs_opt.close();
            } else {
                std::cerr << "[WARNING] Could not open file for IR optimization: " << OUTPUT_IR_OPT_FILE << "\n";
            }

            // Интерпретация оптимизированного кода
            std::cout << "\n========================================\n";
            std::cout << "5. STARTING IR CODE INTERPRETATION\n";
            std::cout << "========================================\n";
            {
                OutputRedirector interpreter_redirector(OUTPUT_INTERPRETER_LOG);
                if (interpreter_redirector.is_open()) {
                    std::cout << "\n*** INTERPRETER OUTPUT for " << input_filename << " ***\n";
                    IRInterpreter interpreter;
                    interpreter.execute(optimized_code);
                    std::cout << "*** INTERPRETATION FINISHED for " << input_filename << " ***\n";
                } else {
                    std::cerr << "[WARNING] Could not open interpreter log file: " << OUTPUT_INTERPRETER_LOG << "\n";
                    std::cout << "\n*** INTERPRETER OUTPUT for " << input_filename << " ***\n";
                    IRInterpreter interpreter;
                    interpreter.execute(optimized_code);
                    std::cout << "*** INTERPRETATION FINISHED for " << input_filename << " ***\n";
                }
            }
            std::cout << "[INFO] Interpreter output saved to: " << OUTPUT_INTERPRETER_LOG << "\n";
        }

    } catch (const std::runtime_error& e) {
        {
            OutputRedirector error_redirector(OUTPUT_ERROR_LOG);
            if (error_redirector.is_open()) {
                std::cerr << "\n[FATAL] Runtime Error during compilation/execution for " << input_filename << ": " << e.what() << "\n";
                error_handler.printErrors(std::cerr);
            }
        }
        std::cerr << "\n[FATAL] Runtime Error during compilation/execution for " << input_filename << ". See " << OUTPUT_ERROR_LOG << " for details.\n";
        return 4;
    }

    std::cout << "\n>>> FINISHED COMPILATION FOR: " << input_filename << " <<<\n";
    return 0;
}

// Точка входа программы
int main() {
    std::cout << "--- LTLab Compiler Startup ---\n";

    // Списки входных файлов и соответствующих выходных папок
    const std::vector<std::string> input_files = {
        "test_1.txt", "test_2.txt", "test_3.txt", "test_4.txt",
        "test_5.txt", "test_6.txt", "test_7.txt", "test_8.txt",
        "test_9.txt"
    };
    const std::vector<std::string> output_folders = {
        "test1", "test2", "test3", "test4", "test5",
        "test6", "test7", "test8", "test9"
    };

    if (input_files.size() != output_folders.size()) {
        std::cerr << "[FATAL] Configuration error: Input file count does not match output folder count.\n";
        return 1;
    }

    int overall_return_code = 0;

    // Обработка всех тестовых файлов
    for (size_t i = 0; i < input_files.size(); ++i) {
        int result = process_file(input_files[i], output_folders[i]);
        if (result != 0) {
            std::cerr << "[ERROR] Compilation failed for file: " << input_files[i] << " with code " << result << "\n";
            overall_return_code = result;
        }
    }

    std::cout << "\n--- LTLab Compiler Shutdown ---\n";
    if (overall_return_code != 0) {
        std::cout << "[WARNING] One or more files failed to compile/execute.\n";
    } else {
        std::cout << "[INFO] All files processed successfully (or with recoverable errors).\n";
    }

    return overall_return_code;
}