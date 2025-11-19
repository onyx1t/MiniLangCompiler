// src/Lexer.cpp
#include "Lexer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm> // For std::find_if

// ********** Start of Token helper functions (required for Lexer.cpp compilation) **********
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
        {TokenType::TOKEN_UNKNOWN, "UNKNOWN"}
    };

    const std::map<TokenClass, std::string> tokenClassNames = {
        {TokenClass::KEYWORD, "KEYWORD"}, {TokenClass::LITERAL, "LITERAL"},
        {TokenClass::IDENTIFIER, "IDENTIFIER"}, {TokenClass::OPERATOR, "OPERATOR"},
        {TokenClass::PUNCTUATION, "PUNCTUATION"}, {TokenClass::END_OF_FILE, "END_OF_FILE"},
        {TokenClass::UNKNOWN, "UNKNOWN"}
    };
}

std::string Token::typeToString() const {
    if (tokenTypeNames.count(type)) return tokenTypeNames.at(type);
    return "UNKNOWN_TYPE";
}

std::string Token::classToString() const {
    if (tokenClassNames.count(token_class)) return tokenClassNames.at(token_class);
    return "UNKNOWN_CLASS";
}
// ********** End of Token helper functions **********


// Constructor
Lexer::Lexer(const std::string& source_code, ErrorHandler* handler)
    : source_code_(source_code), current_pos_(0), current_line_(1), line_start_pos_(0), error_handler_(handler) {}

// --- Helper Methods ---

char Lexer::peek(size_t offset) const {
    if (current_pos_ + offset >= source_code_.length()) {
        return '\0'; // End of file
    }
    return source_code_[current_pos_ + offset];
}

void Lexer::advance() {
    if (peek() == '\n') {
        current_line_++;
        line_start_pos_ = current_pos_ + 1;
    }
    current_pos_++;
}

void Lexer::skipWhitespace() {
    while (current_pos_ < source_code_.length() && std::isspace(peek())) {
        advance();
    }
}

void Lexer::error(const std::string& message) {
    int position_in_line = (int)(current_pos_ - line_start_pos_);
    // Using English for error type
    error_handler_->registerError("Lexical", message, current_line_, position_in_line);
}

// --- State Machine / Automaton Methods ---

// Handles single-line comments (//...)
void Lexer::processComment() {
    if (peek() == '/' && peek(1) == '/') {
        // Skip //
        advance();
        advance();
        // Skip all characters until end of line or EOF
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
        // If we hit '\n', advance() will update the line number
        if (peek() == '\n') {
            advance();
        }
    }
}

// Handles identifiers and keywords
Token Lexer::processIdentifierOrKeyword() {
    int start_pos = current_pos_;
    int start_line = current_line_;

    // Identifier start: letter or '_'
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string value = source_code_.substr(start_pos, current_pos_ - start_pos);

    // Check if the value is a keyword
    if (keywords_.count(value)) {
        return {
            keywords_.at(value),
            TokenClass::KEYWORD,
            value,
            start_line,
            (int)(start_pos - line_start_pos_)
        };
    } else {
        return {
            TokenType::TOKEN_IDENTIFIER,
            TokenClass::IDENTIFIER,
            value,
            start_line,
            (int)(start_pos - line_start_pos_)
        };
    }
}

// Handles integer literals
Token Lexer::processNumber() {
    int start_pos = current_pos_;
    int start_line = current_line_;

    // Number start
    while (std::isdigit(peek())) {
        advance();
    }

    std::string value = source_code_.substr(start_pos, current_pos_ - start_pos);

    return {
        TokenType::TOKEN_INT_LITERAL,
        TokenClass::LITERAL,
        value,
        start_line,
        (int)(start_pos - line_start_pos_)
    };
}

// Handles operators and punctuation
Token Lexer::processOperator() {
    char c = peek();
    int start_pos = current_pos_;
    int start_line = current_line_;

    TokenType type = TokenType::TOKEN_UNKNOWN;
    TokenClass token_class = TokenClass::UNKNOWN;
    std::string value = std::string(1, c);

    advance(); // Consume the first character

    switch (c) {
        case '+': type = TokenType::TOKEN_PLUS; token_class = TokenClass::OPERATOR; break;
        case '-': type = TokenType::TOKEN_MINUS; token_class = TokenClass::OPERATOR; break;
        case '*': type = TokenType::TOKEN_MULTIPLY; token_class = TokenClass::OPERATOR; break;
        case '/': type = TokenType::TOKEN_DIVIDE; token_class = TokenClass::OPERATOR; break;

        case ';': type = TokenType::TOKEN_SEMICOLON; token_class = TokenClass::PUNCTUATION; break;
        case '(': type = TokenType::TOKEN_LPAREN; token_class = TokenClass::PUNCTUATION; break;
        case ')': type = TokenType::TOKEN_RPAREN; token_class = TokenClass::PUNCTUATION; break;
        case '{': type = TokenType::TOKEN_LBRACE; token_class = TokenClass::PUNCTUATION; break;
        case '}': type = TokenType::TOKEN_RBRACE; token_class = TokenClass::PUNCTUATION; break;

        // Double-character operators
        case '=':
            if (peek() == '=') { advance(); value += "="; type = TokenType::TOKEN_EQUAL; }
            else { type = TokenType::TOKEN_ASSIGN; }
            token_class = TokenClass::OPERATOR;
            break;

        case '!':
            if (peek() == '=') { advance(); value += "="; type = TokenType::TOKEN_NOT_EQUAL; token_class = TokenClass::OPERATOR; }
            else { error("Unexpected character '!'"); }
            break;

        case '<':
            type = TokenType::TOKEN_LESS; token_class = TokenClass::OPERATOR;
            break;

        case '>':
            type = TokenType::TOKEN_GREATER; token_class = TokenClass::OPERATOR;
            break;

        default:
            // Return UNKNOWN to let runLexer handle the error
            return {TokenType::TOKEN_UNKNOWN, TokenClass::UNKNOWN, value, start_line, (int)(start_pos - line_start_pos_)};
    }

    return {
        type,
        token_class,
        value,
        start_line,
        (int)(start_pos - line_start_pos_)
    };
}


// --- Main Automaton Loop (Populates tokens_) ---
void Lexer::skipBOM() {
    // BOM для UTF-8: 0xEF, 0xBB, 0xBF
    if (source_code_.length() >= 3 &&
        (unsigned char)source_code_[0] == 0xEF &&
        (unsigned char)source_code_[1] == 0xBB &&
        (unsigned char)source_code_[2] == 0xBF) {

        current_pos_ += 3; // Сдвигаем позицию на 3 байта
        // Важно: BOM находится на первой строке, поэтому line_start_pos_ не меняется
        }
}

// src/Lexer.cpp (Обновленный фрагмент Lexer::runLexer)
void Lexer::runLexer() {
    tokens_.clear();
    current_pos_ = 0;
    current_line_ = 1;
    line_start_pos_ = 0;

    // --- НОВОЕ: Сначала пропускаем BOM ---
    skipBOM();
    // --- Конец НОВОЕ ---

    while (current_pos_ < source_code_.length()) {

        // 1. Пропускаем все, что не является токеном
        skipWhitespace();

        // Повторно проверяем после skipWhitespace
        if (current_pos_ >= source_code_.length()) break;

        // Проверяем комментарий
        if (peek() == '/' && peek(1) == '/') {
            processComment();
            continue;
        }

        // После пропуска пробелов/комментариев, читаем текущий символ
        char c = peek();

        if (c == '\0') break;

        // 2. Расширяющийся автомат: Определение стартового символа
        if (std::isalpha(c) || c == '_') {
            tokens_.push_back(processIdentifierOrKeyword());
        }
        else if (std::isdigit(c)) {
            tokens_.push_back(processNumber());
        }
        else if (c != '\0') {
            // Если символ - не буква, не число и не пробел/комментарий, это оператор/пунктуация
            Token token = processOperator();
            if (token.type == TokenType::TOKEN_UNKNOWN) {
                // Лексическая ошибка (неизвестный символ)
                error("Unknown symbol");
                advance(); // Consume the unknown character
            } else {
                tokens_.push_back(token);
            }
        }
    }

    // Append EOF token
    tokens_.push_back({
        TokenType::TOKEN_EOF,
        TokenClass::END_OF_FILE,
        "EOF",
        current_line_,
        (int)(current_pos_ - line_start_pos_)
    });
}

// --- Output Methods ---

// Method for the Parser (retrieves tokens in sequence)
Token Lexer::getNextToken() {
    static size_t token_index = 0;
    if (token_index < tokens_.size()) {
        return tokens_[token_index++];
    }
    // Should return the last token (EOF) if requested beyond the end
    return tokens_.back();
}

void Lexer::printTokenTable(std::ostream& os) const {
    // English titles here
    os << "## 📄 Token Table (LTLab)\n\n";

    // Russian column headers as requested
    os << "| Номер Строки | Позиция | Имя Лексемы | Класс Лексемы | Значение |\n";
    os << "| :---: | :---: | :--- | :--- | :--- |\n";

    for (const auto& token : tokens_) {
        os << "| " << std::setw(13) << token.line
           << " | " << std::setw(7) << token.position
           << " | " << token.typeToString()
           << " | " << token.classToString()
           << " | `" << token.value << "` |\n";
    }
}

void Lexer::writeTokenTableToFile(const std::string& filename) const {
    std::ofstream ofs(filename);
    if (ofs.is_open()) {
        printTokenTable(ofs);
        ofs.close();
    } else {
        std::cerr << "Error: Could not open file to write token table: " << filename << std::endl;
    }
}