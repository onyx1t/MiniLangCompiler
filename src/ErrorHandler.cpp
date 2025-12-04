#include "ErrorHandler.h"
#include <iostream>

ErrorHandler::ErrorHandler() : has_errors_(false) {}

void ErrorHandler::registerError(const std::string& type, const std::string& message, int line, int position) {
    errors_.push_back({
        message,
        type,
        line,
        position
    });
    has_errors_ = true;
}

bool ErrorHandler::hasErrors() const {
    return has_errors_;
}

const std::vector<CompilerError>& ErrorHandler::getErrors() const {
    return errors_;
}

void ErrorHandler::printErrors(std::ostream& os) const {
    if (!has_errors_) {
        os << "Compilation successful. No errors detected.\n";
        return;
    }

    os << "\n========================================\n";
    os << "         COMPILATION ERROR REPORT\n";
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