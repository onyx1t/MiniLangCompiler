#pragma once

#include <string>

// Типы лексем
enum class TokenType {
    // Ключевые слова
    TOKEN_INT, TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_PRINT,

    // Идентификаторы и литералы
    TOKEN_IDENTIFIER, TOKEN_INT_LITERAL,

    // Операторы
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,

    // Пунктуация
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,

    // Специальные
    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_ERROR
};

// Классы лексем для классификации
enum class TokenClass {
    KEYWORD,
    LITERAL,
    IDENTIFIER,
    OPERATOR,
    PUNCTUATION,
    END_OF_FILE,
    UNKNOWN
};

// Структура лексемы
struct Token {
    TokenType type;
    TokenClass token_class;
    std::string value;
    int line;
    int position;

    Token() : type(TokenType::TOKEN_ERROR), token_class(TokenClass::UNKNOWN), line(0), position(0) {}

    Token(TokenType t, TokenClass tc, const std::string& v, int l, int p)
        : type(t), token_class(tc), value(v), line(l), position(p) {}

    // Вспомогательные методы для вывода
    std::string typeToString() const;
    std::string classToString() const;
};