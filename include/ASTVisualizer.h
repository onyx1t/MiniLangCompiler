#pragma once
#include "AST.h"
#include <string>

// Визуализатор AST: печатает дерево в удобочитаемом формате
class ASTVisualizer : public ASTVisitor {
private:
    std::string indent_;  // Текущий отступ для вложенных узлов
public:
    ASTVisualizer() : indent_("") {}

    // Методы обхода узлов AST
    void visit(ProgramNode& node) override;
    void visit(VarDeclNode& node) override;
    void visit(AssignStmtNode& node) override;
    void visit(PrintStmtNode& node) override;
    void visit(IfStmtNode& node) override;
    void visit(WhileStmtNode& node) override;
    void visit(BinaryOpNode& node) override;
    void visit(IntLiteralNode& node) override;
    void visit(IdentifierNode& node) override;
    void visit(TerminalNode& node) override;
};