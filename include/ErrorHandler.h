#pragma once

#include <iostream>
#include <string>
#include <vector>

// Структура для хранения информации об ошибке
struct CompilerError {
    std::string message;
    std::string type;
    int line;
    int position;
};

// Обработчик ошибок компиляции
class ErrorHandler {
private:
    std::vector<CompilerError> errors_;
    bool has_errors_;

public:
    ErrorHandler();
    void registerError(const std::string& type, const std::string& message, int line, int position);
    bool hasErrors() const;
    const std::vector<CompilerError>& getErrors() const;
    void printErrors(std::ostream& os = std::cerr) const;
};