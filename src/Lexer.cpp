#include "Lexer.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <fstream>

// Вспомогательные функции для преобразования типов токенов в строки
namespace {
    const std::map<TokenType, std::string> tokenTypeNames = {
        {TokenType::TOKEN_INT, "INT"}, {TokenType::TOKEN_IF, "IF"},
        {TokenType::TOKEN_ELSE, "ELSE"}, {TokenType::TOKEN_WHILE, "WHILE"},
        {TokenType::TOKEN_PRINT, "PRINT"}, {TokenType::TOKEN_IDENTIFIER, "IDENTIFIER"},
        {TokenType::TOKEN_INT_LITERAL, "INT_LITERAL"}, {TokenType::TOKEN_ASSIGN, "ASSIGN"},
        {TokenType::TOKEN_PLUS, "PLUS"}, {TokenType::TOKEN_MINUS, "MINUS"},
        {TokenType::TOKEN_MULTIPLY, "MULTIPLY"}, {TokenType::TOKEN_DIVIDE, "DIVIDE"},
        {TokenType::TOKEN_EQUAL, "EQUAL"}, {TokenType::TOKEN_NOT_EQUAL, "NOT_EQUAL"},
        {TokenType::TOKEN_LESS, "LESS"}, {TokenType::TOKEN_GREATER, "GREATER"},
        {TokenType::TOKEN_SEMICOLON, "SEMICOLON"}, {TokenType::TOKEN_LPAREN, "LPAREN"},
        {TokenType::TOKEN_RPAREN, "RPAREN"}, {TokenType::TOKEN_LBRACE, "LBRACE"},
        {TokenType::TOKEN_RBRACE, "RBRACE"}, {TokenType::TOKEN_EOF, "EOF"},
        {TokenType::TOKEN_UNKNOWN, "UNKNOWN"}, {TokenType::TOKEN_ERROR, "ERROR"}
    };

    const std::map<TokenClass, std::string> tokenClassNames = {
        {TokenClass::KEYWORD, "KEYWORD"},
        {TokenClass::LITERAL, "LITERAL"},
        {TokenClass::IDENTIFIER, "IDENTIFIER"},
        {TokenClass::OPERATOR, "OPERATOR"},
        {TokenClass::PUNCTUATION, "PUNCTUATION"},
        {TokenClass::END_OF_FILE, "END_OF_FILE"},
        {TokenClass::UNKNOWN, "UNKNOWN"}
    };
}

std::string Token::typeToString() const {
    auto it = tokenTypeNames.find(type);
    if (it != tokenTypeNames.end()) {
        return it->second;
    }
    return "UNKNOWN_TYPE";
}

std::string Token::classToString() const {
    auto it = tokenClassNames.find(token_class);
    if (it != tokenClassNames.end()) {
        return it->second;
    }
    return "UNKNOWN_CLASS";
}

// Реализация методов лексического анализатора
Lexer::Lexer(const std::string& source_code, ErrorHandler* handler)
    : source_code_(source_code),
      error_handler_(handler) {
}

bool Lexer::isAtEnd() const {
    return current_index_ >= source_code_.length();
}

char Lexer::peek(size_t offset) const {
    size_t index = current_index_ + offset;
    if (index >= source_code_.length()) {
        return '\0';
    }
    return source_code_[index];
}

void Lexer::advance(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        if (isAtEnd()) break;
        if (source_code_[current_index_] == '\n') {
            current_line_++;
            line_start_pos_ = current_index_ + 1;
        }
        current_index_++;
    }
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        advance(2);
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
        skipWhitespace();
    }
}

bool Lexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool Lexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

// Сканирование числового литерала
void Lexer::scanNumber() {
    size_t start_index = current_index_;
    while (isDigit(peek())) {
        advance();
    }

    std::string value = source_code_.substr(start_index, current_index_ - start_index);
    tokens_.push_back({
        TokenType::TOKEN_INT_LITERAL,
        TokenClass::LITERAL,
        value,
        current_line_,
        (int)(start_index - line_start_pos_)
    });
}

// Сканирование идентификатора или ключевого слова
void Lexer::scanIdentifierOrKeyword() {
    size_t start_index = current_index_;
    while (isAlphaNumeric(peek())) {
        advance();
    }

    std::string value = source_code_.substr(start_index, current_index_ - start_index);
    TokenType type;
    TokenClass token_class;

    if (keywords_.count(value)) {
        type = keywords_.at(value);
        token_class = TokenClass::KEYWORD;
    } else {
        type = TokenType::TOKEN_IDENTIFIER;
        token_class = TokenClass::IDENTIFIER;
    }

    tokens_.push_back({
        type,
        token_class,
        value,
        current_line_,
        (int)(start_index - line_start_pos_)
    });
}

// Сканирование одного токена
void Lexer::scanToken() {
    skipWhitespace();
    skipComment();
    skipWhitespace();

    if (isAtEnd()) {
        return;
    }

    char c = peek();
    size_t start_index = current_index_;
    TokenType type = TokenType::TOKEN_UNKNOWN;
    TokenClass token_class = TokenClass::UNKNOWN;
    size_t len = 1;

    if (isDigit(c)) {
        scanNumber();
        return;
    }
    if (isAlpha(c)) {
        scanIdentifierOrKeyword();
        return;
    }

    switch (c) {
        case '+': type = TokenType::TOKEN_PLUS; token_class = TokenClass::OPERATOR; break;
        case '-': type = TokenType::TOKEN_MINUS; token_class = TokenClass::OPERATOR; break;
        case '*': type = TokenType::TOKEN_MULTIPLY; token_class = TokenClass::OPERATOR; break;
        case '/':
            type = TokenType::TOKEN_DIVIDE;
            token_class = TokenClass::OPERATOR;
            break;
        case ';': type = TokenType::TOKEN_SEMICOLON; token_class = TokenClass::PUNCTUATION; break;
        case '(': type = TokenType::TOKEN_LPAREN; token_class = TokenClass::PUNCTUATION; break;
        case ')': type = TokenType::TOKEN_RPAREN; token_class = TokenClass::PUNCTUATION; break;
        case '{': type = TokenType::TOKEN_LBRACE; token_class = TokenClass::PUNCTUATION; break;
        case '}': type = TokenType::TOKEN_RBRACE; token_class = TokenClass::PUNCTUATION; break;

        case '=':
            if (peek(1) == '=') {
                type = TokenType::TOKEN_EQUAL;
                len = 2;
            } else {
                type = TokenType::TOKEN_ASSIGN;
            }
            token_class = TokenClass::OPERATOR;
            break;
        case '!':
            if (peek(1) == '=') {
                type = TokenType::TOKEN_NOT_EQUAL;
                len = 2;
            } else {
                type = TokenType::TOKEN_UNKNOWN;
            }
            token_class = TokenClass::OPERATOR;
            break;
        case '<':
            type = TokenType::TOKEN_LESS;
            token_class = TokenClass::OPERATOR;
            break;
        case '>':
            type = TokenType::TOKEN_GREATER;
            token_class = TokenClass::OPERATOR;
            break;

        default:
            type = TokenType::TOKEN_UNKNOWN;
            token_class = TokenClass::UNKNOWN;
            break;
    }

    if (type == TokenType::TOKEN_UNKNOWN) {
        if (error_handler_) {
            error_handler_->registerError(
                "Lexical",
                "Unknown symbol: '" + std::string(1, c) + "'",
                current_line_,
                (int)(current_index_ - line_start_pos_)
            );
        }
        advance(len);
        return;
    }

    std::string value = source_code_.substr(start_index, len);
    tokens_.push_back({
        type,
        token_class,
        value,
        current_line_,
        (int)(start_index - line_start_pos_)
    });

    advance(len);
}

// Основной метод лексического анализа
void Lexer::runLexer() {
    token_index_ = 0;
    tokens_.clear();

    while (!isAtEnd()) {
        scanToken();
    }

    tokens_.push_back({
        TokenType::TOKEN_EOF,
        TokenClass::END_OF_FILE,
        "EOF",
        current_line_,
        (int)(current_index_ - line_start_pos_)
    });
}

// Получение следующего токена для парсера
Token Lexer::getNextToken() {
    if (token_index_ < tokens_.size()) {
        return tokens_[token_index_++];
    }

    if (!tokens_.empty()) {
        return tokens_.back();
    }

    return Token(TokenType::TOKEN_ERROR, TokenClass::UNKNOWN, "Empty Token Stream", 0, 0);
}

// Вывод таблицы токенов
void Lexer::printTokenTable(std::ostream& os) const {
    os << "## Token Table \n\n";
    os << "| Номер Строки | Позиция | Имя Лексемы | Класс Лексемы | Значение |\n";
    os << "| :----------: | :-----: | :---------: | :-----------: | :------: |\n";

    const int LINE_WIDTH = 12;
    const int POS_WIDTH = 7;
    const int TYPE_WIDTH = 11;
    const int CLASS_WIDTH = 13;
    const int VALUE_CELL_WIDTH = 8;

    os << std::left;

    for (const auto& token : tokens_) {
        std::string value = token.value.empty() ? "\u00A0" : token.value;

        os << "| " << std::setw(LINE_WIDTH) << std::right << token.line
           << " | " << std::setw(POS_WIDTH) << token.position
           << " | " << std::setw(TYPE_WIDTH) << std::left << token.typeToString()
           << " | " << std::setw(CLASS_WIDTH) << token.classToString()
           << " | `" << std::setw(VALUE_CELL_WIDTH - 2) << value << "` |\n";
    }

    os << std::right;
}

// Запись таблицы токенов в файл
void Lexer::writeTokenTableToFile(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (ofs.is_open()) {
        printTokenTable(ofs);
        ofs.close();
    } else {
        std::cerr << "Error: Could not open output file for token table: " << filename << std::endl;
    }
}