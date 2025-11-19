// include/ErrorHandler.h
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ostream>

struct CompilerError {
    std::string message;
    std::string type; // "Lexical", "Syntax", "Semantic"
    int line;
    int position;
};

class ErrorHandler {
private:
    std::vector<CompilerError> errors_;
    bool has_errors_;

public:
    ErrorHandler();

    // Register an error
    void registerError(const std::string& type, const std::string& message, int line, int position);

    // Check if any errors occurred
    bool hasErrors() const;

    // Get the list of errors
    const std::vector<CompilerError>& getErrors() const;

    // Output all collected errors
    void printErrors(std::ostream& os = std::cerr) const;
};