// src/ErrorHandler.cpp
#include "ErrorHandler.h"
#include <iostream>

// Constructor
ErrorHandler::ErrorHandler() : has_errors_(false) {}

// Method to register an error
void ErrorHandler::registerError(const std::string& type, const std::string& message, int line, int position) {
    errors_.push_back({
        message,
        type,
        line,
        position
    });
    // Set the flag
    has_errors_ = true;
}

// Check for errors
bool ErrorHandler::hasErrors() const {
    return has_errors_;
}

// Get the list of errors
const std::vector<CompilerError>& ErrorHandler::getErrors() const {
    return errors_;
}

// Helper method to output all collected errors
void ErrorHandler::printErrors(std::ostream& os) const {
    if (!has_errors_) {
        os << "Compilation successful. No errors detected.\n";
        return;
    }

    os << "\n========================================\n";
    os << "       🚨 COMPILATION ERROR REPORT\n";
    os << "========================================\n";

    for (const auto& err : errors_) {
        os << "[" << err.type << " Error] "
           << "Line " << err.line
           << ", Position " << err.position
           << ": " << err.message << "\n";
    }

    os << "========================================\n";
    os << "Total errors found: " << errors_.size() << "\n";
}