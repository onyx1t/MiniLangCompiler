// include/AST.h
#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include "Token.h" // Убедитесь, что Token.h включен

// --- Предварительное объявление посетителя и ProgramNode ---
class ASTVisitor;
class ProgramNode; // Для ссылок в IfStmtNode и WhileStmtNode

// --- Базовый Класс ---
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// --- Вспомогательный класс для терминальных символов ---
class TerminalNode : public ASTNode {
public:
    TokenType type;
    std::string value;

    TerminalNode(TokenType t, const std::string& v = "") : type(t), value(v) {}
    void accept(ASTVisitor& visitor) override;
};

// --- 1. Узлы Выражений (Expressions) ---

class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = default;
};

// Литерал (число)
class IntLiteralNode : public ExpressionNode {
public:
    int value;
    IntLiteralNode(int v) : value(v) {}
    void accept(ASTVisitor& visitor) override;
};

// Идентификатор (переменная)
class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    IdentifierNode(const std::string& n) : name(n) {}
    void accept(ASTVisitor& visitor) override;
};

// Бинарная операция (A + B, A == B)
class BinaryOpNode : public ExpressionNode {
public:
    TokenType op; // Тип оператора (+, *, ==, <)
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType op_type, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(op_type), left(std::move(l)), right(std::move(r)) {}

    // --- ДОБАВЛЕНО: Для ASTVisualizer.cpp ---
    std::string op_type_to_string() const;

    void accept(ASTVisitor& visitor) override;
};

// --- 2. Узлы Операторов (Statements) ---

// Объявление переменной (int id;)
class VarDeclNode : public ASTNode {
public:
    std::string name;
    TokenType type; // (TOKEN_INT)

    VarDeclNode(const std::string& n, TokenType t) : name(n), type(t) {}

    // --- ДОБАВЛЕНО: Для ASTVisualizer.cpp ---
    std::string typeToString() const;

    void accept(ASTVisitor& visitor) override;
};

// Присваивание (id = expr;)
class AssignStmtNode : public ASTNode {
public:
    // Имя переменной
    std::string identifier_name;
    std::unique_ptr<ASTNode> expression;

    AssignStmtNode(const std::string& n, std::unique_ptr<ASTNode> expr)
        : identifier_name(n), expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

// Вывод (print expr;)
class PrintStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    PrintStmtNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

// Условный оператор (if (C) {S} else {S})
class IfStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ProgramNode> then_body; // Тело IF
    std::unique_ptr<ProgramNode> else_body; // Тело ELSE (может быть nullptr)

    IfStmtNode() = default;
    void accept(ASTVisitor& visitor) override;
};

// Цикл (while (C) {S})
class WhileStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ProgramNode> body;

    WhileStmtNode() = default;
    void accept(ASTVisitor& visitor) override;
};

// --- 3. Узел Программы (Корневой узел и список операторов) ---

// Корень дерева и список операторов внутри блоков {}
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void accept(ASTVisitor& visitor) override;
};

// --- Класс Посетителя (Для обхода и вывода/генерации кода) ---

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // Посещение операторов и структур
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(VarDeclNode& node) = 0;
    virtual void visit(AssignStmtNode& node) = 0;
    virtual void visit(PrintStmtNode& node) = 0;
    virtual void visit(IfStmtNode& node) = 0;
    virtual void visit(WhileStmtNode& node) = 0;

    // Посещение выражений
    virtual void visit(BinaryOpNode& node) = 0;
    virtual void visit(IntLiteralNode& node) = 0;
    virtual void visit(IdentifierNode& node) = 0;
    virtual void visit(TerminalNode& node) = 0;
};