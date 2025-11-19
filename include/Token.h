// include/Token.h
#pragma once

#include <string>
#include <map>
#include <iostream>

// --- 1. TokenType: Все возможные типы лексем ---
enum class TokenType {
    // Ключевые слова
    TOKEN_INT, TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_PRINT,

    // Идентификаторы и литералы
    TOKEN_IDENTIFIER, TOKEN_INT_LITERAL,

    // Операторы
    TOKEN_ASSIGN,       // =
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_EQUAL,        // == (Реляционный)
    TOKEN_NOT_EQUAL,    // != (Реляционный)
    TOKEN_LESS,         // < (Реляционный)
    TOKEN_GREATER,      // > (Реляционный)

    // Пунктуация
    TOKEN_SEMICOLON,    // ;
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }

    // Специальные
    TOKEN_EOF,          // End of File
    TOKEN_UNKNOWN,      // Неизвестный символ
    TOKEN_ERROR         // Специальный тип для ошибок
};

// --- 2. TokenClass: Класс лексемы (для классификации и печати) ---
enum class TokenClass {
    KEYWORD,
    LITERAL,
    IDENTIFIER,
    OPERATOR,
    PUNCTUATION,
    END_OF_FILE,
    UNKNOWN // Для ошибок или неклассифицированных
};

// --- 3. Token Structure ---
struct Token {
    // Данные лексемы
    TokenType type;
    TokenClass token_class; // НОВЫЙ член, который добавил 5-й аргумент
    std::string value;
    int line;
    int position; // Позиция относительно начала строки

    // Конструктор по умолчанию (для создания нулевых токенов)
    Token() :
        type(TokenType::TOKEN_ERROR),
        token_class(TokenClass::UNKNOWN),
        line(0),
        position(0) {}

    // Основной конструктор (5 аргументов)
    Token(TokenType t, TokenClass tc, const std::string& v, int l, int p)
        : type(t), token_class(tc), value(v), line(l), position(p) {}

    // Вспомогательные функции (определения находятся в Lexer.cpp)
    // Используются для вывода в таблице токенов и в отладке парсера/визуализатора AST.
    std::string typeToString() const;
    std::string classToString() const;
};