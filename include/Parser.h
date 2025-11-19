// include/Parser.h
#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include "Token.h"
#include "Lexer.h"
#include "ErrorHandler.h"
#include "AST.h"

class Parser {
private:
    Lexer* lexer_;
    ErrorHandler* error_handler_;
    Token current_token_;

    // ----------------------------------------------------------------
    // Core Methods
    // ----------------------------------------------------------------

    // Проверяет, что текущий токен соответствует ожидаемому типу, и считывает следующий.
    void match(TokenType expected_type);

    // Проверяет, что текущий токен является одним из ожидаемых.
    bool check(TokenType type);

    // Сообщает о синтаксической ошибке.
    void parseError(const std::string& message);

    // Вспомогательная функция для считывания следующего токена (обертка для lexer_->getNextToken)
    void consume();

    // ----------------------------------------------------------------
    // Recursive Descent Methods (for Non-Terminals)
    // ----------------------------------------------------------------

    std::unique_ptr<ASTNode> parseStmtList(); // S
    std::unique_ptr<ASTNode> parseStmtListRest(std::unique_ptr<ASTNode> current_list); // S_rest
    std::unique_ptr<ASTNode> parseStmt();     // Stmt

    std::unique_ptr<ASTNode> parseCondition(); // C

    std::unique_ptr<ASTNode> parseExpr();     // E
    std::unique_ptr<ASTNode> parseExprRest(std::unique_ptr<ASTNode> left_expr); // E_rest
    std::unique_ptr<ASTNode> parseTerm();     // T
    std::unique_ptr<ASTNode> parseTermRest(std::unique_ptr<ASTNode> left_term); // T_rest
    std::unique_ptr<ASTNode> parseFactor();   // F

    std::unique_ptr<ASTNode> parseStmtElse(); // StmtElse

public:
    Parser(Lexer* lexer, ErrorHandler* handler);

    // Основной метод парсинга
    std::unique_ptr<ASTNode> parseProgram(); // P
};