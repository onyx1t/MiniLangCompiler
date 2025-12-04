#pragma once

#include <string>
#include <vector>
#include <map>
#include <ostream>
#include "Token.h"
#include "ErrorHandler.h"

// Лексический анализатор: преобразует исходный код в поток токенов
class Lexer {
private:
    const std::string source_code_;      // Исходный код программы
    std::vector<Token> tokens_;          // Список распознанных токенов
    ErrorHandler* error_handler_;        // Обработчик ошибок

    size_t current_index_ = 0;           // Текущая позиция в исходном коде
    int current_line_ = 1;               // Текущий номер строки
    size_t line_start_pos_ = 0;          // Начало текущей строки
    size_t token_index_ = 0;             // Индекс текущего токена (для парсера)

    // Вспомогательные методы для работы с исходным кодом
    char peek(size_t offset = 0) const;
    void advance(size_t count = 1);
    void skipWhitespace();
    void skipComment();

    // Методы проверки символов
    bool isAtEnd() const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;

    // Методы сканирования токенов
    void scanNumber();
    void scanIdentifierOrKeyword();
    void scanStringLiteral();
    void scanToken();

    // Таблица ключевых слов
    const std::map<std::string, TokenType> keywords_ = {
        {"int", TokenType::TOKEN_INT},
        {"if", TokenType::TOKEN_IF},
        {"else", TokenType::TOKEN_ELSE},
        {"while", TokenType::TOKEN_WHILE},
        {"print", TokenType::TOKEN_PRINT}
    };

public:
    Lexer(const std::string& source_code, ErrorHandler* handler);

    // Основной метод лексического анализа
    void runLexer();

    // Получение следующего токена (для парсера)
    Token getNextToken();

    // Вывод таблицы токенов
    void printTokenTable(std::ostream& os) const;
    void writeTokenTableToFile(const std::string& filename) const;
};