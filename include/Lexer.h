// include/Lexer.h
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "Token.h"
#include "ErrorHandler.h" // Needed for error handling

class Lexer {
private:
    std::string source_code_; // Source code input
    size_t current_pos_;      // Current position in the code
    int current_line_;        // Current line number
    int line_start_pos_;      // Position of the start of the current line

    std::vector<Token> tokens_; // Collection of all recognized tokens
    ErrorHandler* error_handler_; // Pointer to the error handler

    // Keyword map for quick lookup
    const std::map<std::string, TokenType> keywords_ = {
        {"int", TokenType::TOKEN_INT},
        {"if", TokenType::TOKEN_IF},
        {"else", TokenType::TOKEN_ELSE},
        {"while", TokenType::TOKEN_WHILE},
        {"print", TokenType::TOKEN_PRINT}
    };

    // Helper methods
    char peek(size_t offset = 0) const; // Look ahead without moving
    void advance();                     // Move current_pos_
    void skipWhitespace();              // Skip whitespace characters

    // --- State Machine / Automaton Methods ---
    Token processIdentifierOrKeyword(); // Handles identifiers and keywords
    Token processNumber();              // Handles integer literals
    Token processOperator();            // Handles operators and punctuation
    void processComment();              // Handles comments (//...)
    void skipBOM();                     // Skips UTF-8 Byte Order Mark (BOM)

    // Error registration wrapper
    void error(const std::string& message);

public:
    Lexer(const std::string& source_code, ErrorHandler* handler);

    // Main method to run the lexer and populate tokens_
    void runLexer();

    // Method for the Parser to retrieve the next token
    Token getNextToken();

    // Output methods
    void printTokenTable(std::ostream& os = std::cout) const;
    void writeTokenTableToFile(const std::string& filename) const;
};