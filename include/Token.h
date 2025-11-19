// include/Token.h
#pragma once

#include <string>
#include <map>

// 1. Классы лексем (Lexeme Class/Subclass)
enum class TokenClass {
    KEYWORD,      // Ключевое слово (if, while, int, print)
    LITERAL,      // Литералы (числа, строки - если есть)
    IDENTIFIER,   // Идентификаторы (имена переменных)
    OPERATOR,     // Операторы (+, -, =, ==, <, > и т.д.)
    PUNCTUATION,  // Знаки препинания (;, (, ), {, })
    END_OF_FILE,  // Конец файла
    UNKNOWN       // Неизвестный символ или ошибка
};

// 2. Имена лексем (Lexeme Name)
enum class TokenType {
    // Ключевые слова
    TOKEN_INT, TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_PRINT,

    // Идентификаторы и Литералы
    TOKEN_IDENTIFIER,
    TOKEN_INT_LITERAL,

    // Операторы
    TOKEN_ASSIGN,       // =
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_EQUAL,        // ==
    TOKEN_NOT_EQUAL,    // !=
    TOKEN_LESS,         // <
    TOKEN_GREATER,      // >

    // Знаки препинания
    TOKEN_SEMICOLON,    // ;
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }

    // Специальные
    TOKEN_EOF,
    TOKEN_UNKNOWN
};

// 3. Структура Лексемы (Token)
struct Token {
    TokenType type;
    TokenClass token_class;
    std::string value; // Значение лексемы (например, "42" или "x")
    int line;          // Номер строки
    int position;      // Позиция в строке

    // Вспомогательная функция для получения строкового имени типа
    std::string typeToString() const;
    // Вспомогательная функция для получения строкового имени класса
    std::string classToString() const;
};