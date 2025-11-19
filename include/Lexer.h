#pragma once

#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "Token.h"      // Содержит определение Token, TokenType, TokenClass
#include "ErrorHandler.h" // Для регистрации ошибок

class Lexer {
private:
    const std::string source_code_;
    std::vector<Token> tokens_;
    ErrorHandler* error_handler_;

    // Управление позицией в исходном коде
    size_t current_index_ = 0;
    int current_line_ = 1;
    size_t line_start_pos_ = 0; // Индекс начала текущей строки

    // Управление позицией в списке токенов для Parser (ИСПРАВЛЕНИЕ)
    // Индекс должен быть членом класса, а не статическим в getNextToken().
    size_t token_index_ = 0;

    // Вспомогательные методы
    char peek(size_t offset = 0) const;
    void advance(size_t count = 1);
    void skipWhitespace();
    void skipComment();

    bool isAtEnd() const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;

    // Методы сканирования лексем
    void scanNumber();
    void scanIdentifierOrKeyword();
    void scanStringLiteral(); // Если поддерживаются строковые литералы
    void scanToken();

    // Карта ключевых слов для быстрого поиска
    const std::map<std::string, TokenType> keywords_ = {
        {"int", TokenType::TOKEN_INT},
        {"if", TokenType::TOKEN_IF},
        {"else", TokenType::TOKEN_ELSE},
        {"while", TokenType::TOKEN_WHILE},
        {"print", TokenType::TOKEN_PRINT}
    };

public:
    Lexer(const std::string& source_code, ErrorHandler* handler);

    // Основная функция запуска лексического анализатора
    void runLexer();

    // Метод для Parser, извлекающий следующий токен
    Token getNextToken();

    // Методы вывода
    void printTokenTable(std::ostream& os) const;
    void writeTokenTableToFile(const std::string& filename) const;
};