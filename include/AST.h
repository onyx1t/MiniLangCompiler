#pragma once

#include <vector>
#include <memory>
#include <string>
#include "Token.h"

class ASTVisitor;
class ProgramNode;

// Базовый класс для всех узлов AST
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// Узел для лексемы
class TerminalNode : public ASTNode {
public:
    TokenType type;
    std::string value;

    TerminalNode(TokenType t, const std::string& v = "") : type(t), value(v) {}
    void accept(ASTVisitor& visitor) override;
};

// Базовый класс для выражений
class ExpressionNode : public ASTNode {
public:
    virtual ~ExpressionNode() = default;
};

// Узел для целочисленного литерала
class IntLiteralNode : public ExpressionNode {
public:
    int value;
    IntLiteralNode(int v) : value(v) {}
    void accept(ASTVisitor& visitor) override;
};

// Узел для идентификатора
class IdentifierNode : public ExpressionNode {
public:
    std::string name;
    IdentifierNode(const std::string& n) : name(n) {}
    void accept(ASTVisitor& visitor) override;
};

// Узел для бинарной операции
class BinaryOpNode : public ExpressionNode {
public:
    TokenType op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;

    BinaryOpNode(TokenType op_type, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(op_type), left(std::move(l)), right(std::move(r)) {}

    std::string op_type_to_string() const;
    void accept(ASTVisitor& visitor) override;
};

// Узел для объявления переменной
class VarDeclNode : public ASTNode {
public:
    std::string name;
    TokenType type;

    VarDeclNode(const std::string& n, TokenType t) : name(n), type(t) {}
    std::string typeToString() const;
    void accept(ASTVisitor& visitor) override;
};

// Узел для оператора присваивания
class AssignStmtNode : public ASTNode {
public:
    std::string identifier_name;
    std::unique_ptr<ASTNode> expression;

    AssignStmtNode(const std::string& n, std::unique_ptr<ASTNode> expr)
        : identifier_name(n), expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

// Узел для оператора вывода
class PrintStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;
    PrintStmtNode(std::unique_ptr<ASTNode> expr) : expression(std::move(expr)) {}
    void accept(ASTVisitor& visitor) override;
};

// Узел для условного оператора
class IfStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ProgramNode> then_body;
    std::unique_ptr<ProgramNode> else_body;

    IfStmtNode() = default;
    void accept(ASTVisitor& visitor) override;
};

// Узел для оператора цикла
class WhileStmtNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ProgramNode> body;

    WhileStmtNode() = default;
    void accept(ASTVisitor& visitor) override;
};

// Узел для блока программы
class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    void accept(ASTVisitor& visitor) override;
};

// Интерфейс посетителя AST
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // Методы для узлов-операторов
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(VarDeclNode& node) = 0;
    virtual void visit(AssignStmtNode& node) = 0;
    virtual void visit(PrintStmtNode& node) = 0;
    virtual void visit(IfStmtNode& node) = 0;
    virtual void visit(WhileStmtNode& node) = 0;

    // Методы для узлов-выражений
    virtual void visit(BinaryOpNode& node) = 0;
    virtual void visit(IntLiteralNode& node) = 0;
    virtual void visit(IdentifierNode& node) = 0;
    virtual void visit(TerminalNode& node) = 0;
};