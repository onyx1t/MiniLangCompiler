// src/Parser.cpp
#include "Parser.h"
#include <stdexcept>
#include <sstream>
#include <iostream>

using std::move;

// --------------------------------------------------------------------------
// I. Core Utilities (Обслуживание токенов и ошибок)
// --------------------------------------------------------------------------

// Перегрузка оператора << для удобного вывода TokenType (Устранение ошибки компиляции)
std::ostream& operator<<(std::ostream& os, const TokenType& type) {
    // --- ИСПРАВЛЕНИЕ: Добавлен пятый аргумент TokenClass::UNKNOWN ---
    // Создаем временный токен для использования вспомогательной функции typeToString
    Token temp_token(type, TokenClass::UNKNOWN, "", 0, 0);
    os << temp_token.typeToString();
    return os;
}

Parser::Parser(Lexer* lexer, ErrorHandler* handler)
    : lexer_(lexer), error_handler_(handler) {

    current_token_ = lexer_->getNextToken(); // Загружаем первый токен
}

void Parser::consume() {
    current_token_ = lexer_->getNextToken();
}

bool Parser::check(TokenType type) {
    return current_token_.type == type;
}

void Parser::match(TokenType expected_type) {
    if (check(expected_type)) {
        consume();
    } else {
        std::stringstream ss;

        // Теперь operator<<(expected_type) работает корректно
        ss << "Expected token type " << expected_type
           << " but found " << current_token_.type
           << " ('" << current_token_.value << "') at line "
           << current_token_.line << ", pos " << current_token_.position;

        parseError(ss.str());
    }
}

void Parser::parseError(const std::string& message) {
    std::string err_msg = "Syntax Error: " + message;
    error_handler_->registerError("Syntax", err_msg, current_token_.line, current_token_.position);
    // Для остановки парсинга и предотвращения каскадных ошибок
    throw std::runtime_error("Parsing aborted due to Syntax Error.");
}

// --------------------------------------------------------------------------
// II. Recursive Descent Implementation
// --------------------------------------------------------------------------

// P -> S EOF
std::unique_ptr<ASTNode> Parser::parseProgram() {
    try {
        std::cout << "[INFO] Starting Recursive Descent Parsing...\n";

        // S (StmtList)
        std::unique_ptr<ASTNode> statement_list = parseStmtList();

        // EOF
        if (check(TokenType::TOKEN_EOF)) {
            match(TokenType::TOKEN_EOF);
            std::cout << "[INFO] Parsing completed successfully (EOF matched).\n";
            return statement_list;
        } else {
            parseError("Expected EOF, but found more tokens.");
            return nullptr;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Parser error caught: " << e.what() << "\n";
        return nullptr;
    }
}

// F -> (E) | ID | INT_LITERAL
std::unique_ptr<ASTNode> Parser::parseFactor() {
    Token token = current_token_;

    if (check(TokenType::TOKEN_LPAREN)) { // F -> (E)
        match(TokenType::TOKEN_LPAREN);
        std::unique_ptr<ASTNode> expr = parseExpr();
        match(TokenType::TOKEN_RPAREN);
        return expr;
    }
    else if (check(TokenType::TOKEN_IDENTIFIER)) { // F -> ID
        consume();
        return std::make_unique<IdentifierNode>(token.value);
    }
    else if (check(TokenType::TOKEN_INT_LITERAL)) { // F -> INT_LITERAL
        consume();
        return std::make_unique<IntLiteralNode>(std::stoi(token.value));
    }
    else {
        parseError("Expected factor (ID, INT_LITERAL, or '(')");
        return nullptr;
    }
}

// T -> F T_rest
std::unique_ptr<ASTNode> Parser::parseTerm() {
    std::unique_ptr<ASTNode> left_term = parseFactor();
    return parseTermRest(move(left_term));
}

// T_rest -> * F T_rest | epsilon
std::unique_ptr<ASTNode> Parser::parseTermRest(std::unique_ptr<ASTNode> left_term) {
    if (check(TokenType::TOKEN_MULTIPLY)) { // T_rest -> * F T_rest
        Token op = current_token_;
        match(TokenType::TOKEN_MULTIPLY);

        std::unique_ptr<ASTNode> right_factor = parseFactor();

        std::unique_ptr<ASTNode> new_left = std::make_unique<BinaryOpNode>(
            op.type,
            move(left_term),
            move(right_factor)
        );
        return parseTermRest(move(new_left));
    }
    // T_rest -> epsilon
    return left_term;
}

// E -> T E_rest
std::unique_ptr<ASTNode> Parser::parseExpr() {
    std::unique_ptr<ASTNode> left_expr = parseTerm();
    return parseExprRest(move(left_expr));
}

// E_rest -> + T E_rest | - T E_rest | epsilon
std::unique_ptr<ASTNode> Parser::parseExprRest(std::unique_ptr<ASTNode> left_expr) {
    if (check(TokenType::TOKEN_PLUS) || check(TokenType::TOKEN_MINUS)) { // E_rest -> (+|-) T E_rest
        Token op = current_token_;
        consume(); // Match PLUS or MINUS

        std::unique_ptr<ASTNode> right_term = parseTerm();

        std::unique_ptr<ASTNode> new_left = std::make_unique<BinaryOpNode>(
            op.type,
            move(left_expr),
            move(right_term)
        );
        return parseExprRest(move(new_left));
    }
    // E_rest -> epsilon
    return left_expr;
}

// C -> E RELOP E
std::unique_ptr<ASTNode> Parser::parseCondition() {
    std::unique_ptr<ASTNode> left_expr = parseExpr();

    // RELOP (==, <, >, !=)
    if (check(TokenType::TOKEN_EQUAL) || check(TokenType::TOKEN_LESS) ||
        check(TokenType::TOKEN_GREATER) || check(TokenType::TOKEN_NOT_EQUAL)) {

        Token op = current_token_;
        consume(); // Match RELOP

        std::unique_ptr<ASTNode> right_expr = parseExpr();

        return std::make_unique<BinaryOpNode>(
            op.type,
            move(left_expr),
            move(right_expr)
        );
    }
    else {
        parseError("Expected relational operator ('==', '!=', '<', or '>') in condition.");
        return nullptr;
    }
}

// StmtElse -> ELSE {S} | epsilon
std::unique_ptr<ASTNode> Parser::parseStmtElse() {
    if (check(TokenType::TOKEN_ELSE)) {
        match(TokenType::TOKEN_ELSE);
        match(TokenType::TOKEN_LBRACE);
        std::unique_ptr<ASTNode> else_body = parseStmtList(); // S
        match(TokenType::TOKEN_RBRACE);

        // Возвращаем тело ELSE
        return else_body;
    }
    // StmtElse -> epsilon
    return nullptr;
}

// Stmt -> INT ID ; | ID = E ; | PRINT E ; | IF... | WHILE...
std::unique_ptr<ASTNode> Parser::parseStmt() {
    if (check(TokenType::TOKEN_INT)) { // Stmt -> INT ID ; (VarDecl)
        match(TokenType::TOKEN_INT);

        Token id_token = current_token_;
        match(TokenType::TOKEN_IDENTIFIER);
        match(TokenType::TOKEN_SEMICOLON);

        return std::make_unique<VarDeclNode>(id_token.value, TokenType::TOKEN_INT);
    }
    else if (check(TokenType::TOKEN_IDENTIFIER)) { // Stmt -> ID = E ; (Assignment)
        Token id_token = current_token_;
        match(TokenType::TOKEN_IDENTIFIER);
        match(TokenType::TOKEN_ASSIGN);

        std::unique_ptr<ASTNode> expr = parseExpr();
        match(TokenType::TOKEN_SEMICOLON);

        return std::make_unique<AssignStmtNode>(id_token.value, move(expr));
    }
    else if (check(TokenType::TOKEN_PRINT)) { // Stmt -> PRINT E ;
        match(TokenType::TOKEN_PRINT);
        std::unique_ptr<ASTNode> expr = parseExpr();
        match(TokenType::TOKEN_SEMICOLON);
        return std::make_unique<PrintStmtNode>(move(expr));
    }
    else if (check(TokenType::TOKEN_IF)) { // Stmt -> IF (C) {S} StmtElse
        match(TokenType::TOKEN_IF);
        match(TokenType::TOKEN_LPAREN);
        std::unique_ptr<ASTNode> condition = parseCondition(); // C
        match(TokenType::TOKEN_RPAREN);
        match(TokenType::TOKEN_LBRACE);

        // S - Тело IF
        // Используем dynamic_cast<ProgramNode*> для получения тела блока
        std::unique_ptr<ProgramNode> then_body(dynamic_cast<ProgramNode*>(parseStmtList().release()));
        match(TokenType::TOKEN_RBRACE);

        std::unique_ptr<ASTNode> else_body_ast = parseStmtElse();

        std::unique_ptr<IfStmtNode> if_node = std::make_unique<IfStmtNode>();
        if_node->condition = move(condition);
        if_node->then_body = move(then_body);
        // Преобразуем ASTNode в ProgramNode для else_body
        if_node->else_body.reset(dynamic_cast<ProgramNode*>(else_body_ast.release()));

        return if_node;
    }
    else if (check(TokenType::TOKEN_WHILE)) { // Stmt -> WHILE (C) {S}
        match(TokenType::TOKEN_WHILE);
        match(TokenType::TOKEN_LPAREN);
        std::unique_ptr<ASTNode> condition = parseCondition(); // C
        match(TokenType::TOKEN_RPAREN);
        match(TokenType::TOKEN_LBRACE);

        // S - Тело WHILE
        std::unique_ptr<ProgramNode> body(dynamic_cast<ProgramNode*>(parseStmtList().release()));
        match(TokenType::TOKEN_RBRACE);

        std::unique_ptr<WhileStmtNode> while_node = std::make_unique<WhileStmtNode>();
        while_node->condition = move(condition);
        while_node->body = move(body);
        return while_node;
    }
    else {
        parseError("Expected statement (INT, ID, PRINT, IF, or WHILE)");
        return nullptr;
    }
}

// S -> Stmt S_rest (Используется для парсинга содержимого {S} и корневого уровня)
std::unique_ptr<ASTNode> Parser::parseStmtList() {
    // Проверяем, что список не пустой
    if (check(TokenType::TOKEN_RBRACE) || check(TokenType::TOKEN_EOF)) {
        // Пустой список (если пустой блок {} или EOF)
        return std::make_unique<ProgramNode>();
    }

    // Stmt
    std::unique_ptr<ASTNode> first_stmt = parseStmt();

    // Создаем корневой узел списка программы (ProgramNode)
    std::unique_ptr<ProgramNode> list_node = std::make_unique<ProgramNode>();
    if (first_stmt) {
        list_node->statements.push_back(move(first_stmt));
    }

    // S_rest -> Stmt S_rest | epsilon
    return parseStmtListRest(move(list_node));
}

// S_rest -> Stmt S_rest | epsilon
std::unique_ptr<ASTNode> Parser::parseStmtListRest(std::unique_ptr<ASTNode> current_list) {
    // FIRST(Stmt) = { INT, ID, PRINT, IF, WHILE }
    // Если видим один из этих токенов, продолжаем парсить Stmt
    if (check(TokenType::TOKEN_INT) || check(TokenType::TOKEN_IDENTIFIER) ||
        check(TokenType::TOKEN_PRINT) || check(TokenType::TOKEN_IF) || check(TokenType::TOKEN_WHILE)) {

        std::unique_ptr<ASTNode> next_stmt = parseStmt();

        // Добавляем оператор в текущий список (ProgramNode)
        ProgramNode* list_ptr = dynamic_cast<ProgramNode*>(current_list.get());
        if (list_ptr && next_stmt) {
            list_ptr->statements.push_back(move(next_stmt));
        }

        // Рекурсивный вызов S_rest
        return parseStmtListRest(move(current_list));
    }

    // S_rest -> epsilon (Если видим Follow(S) = EOF, RBRACE)
    return current_list;
}