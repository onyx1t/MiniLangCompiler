#pragma once

#include <memory>
#include "Token.h"
#include "Lexer.h"
#include "ErrorHandler.h"
#include "AST.h"

// Синтаксический анализатор (нисходящий рекурсивный парсер)
class Parser {
private:
    Lexer* lexer_;
    ErrorHandler* error_handler_;
    Token current_token_;

    // Основные методы парсера
    void match(TokenType expected_type);     // Проверка и потребление токена
    bool check(TokenType type);              // Проверка типа текущего токена
    void parseError(const std::string& message); // Сообщение об ошибке
    void consume();                          // Переход к следующему токену

    // Рекурсивные методы разбора
    std::unique_ptr<ASTNode> parseStmtList();        // Список операторов
    std::unique_ptr<ASTNode> parseStmtListRest(std::unique_ptr<ASTNode> current_list);
    std::unique_ptr<ASTNode> parseStmt();            // Оператор
    std::unique_ptr<ASTNode> parseCondition();       // Условие
    std::unique_ptr<ASTNode> parseExpr();            // Выражение
    std::unique_ptr<ASTNode> parseExprRest(std::unique_ptr<ASTNode> left_expr);
    std::unique_ptr<ASTNode> parseTerm();            // Терм
    std::unique_ptr<ASTNode> parseTermRest(std::unique_ptr<ASTNode> left_term);
    std::unique_ptr<ASTNode> parseFactor();          // Фактор
    std::unique_ptr<ASTNode> parseStmtElse();        // Else-ветка

public:
    Parser(Lexer* lexer, ErrorHandler* handler);

    // Основной метод парсинга программы
    std::unique_ptr<ASTNode> parseProgram();
};