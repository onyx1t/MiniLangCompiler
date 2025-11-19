#pragma once
#include "AST.h"
#include <iostream>
#include <string>

// Класс для визуализации АСТ в виде иерархического текста.
// Наследуется от ASTVisitor.
class ASTVisualizer : public ASTVisitor {
private:
    std::string indent_; // Текущая строка отступа (для ветвления)

public:
    ASTVisualizer() : indent_("") {}

    // Методы Посетителя (должны соответствовать узлам в AST.h)
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