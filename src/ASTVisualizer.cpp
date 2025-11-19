// src/ASTVisualizer.cpp (ИСПРАВЛЕННЫЙ КОД)
#include "ASTVisualizer.h"
#include <iostream>
#include <sstream>

using std::cout;

// ASCII символы для визуализации
const std::string V_BRANCH = "|   "; // Вертикальная линия (для ветвления)
const std::string H_BRANCH = "|-- "; // Горизонтальная ветвь (для не-последнего ребенка)
const std::string L_BRANCH = "`-- "; // Угловая ветвь (для последнего ребенка)
const std::string NO_BRANCH = "    "; // Нет ветви (для потомков последнего ребенка)


// ---------------------------------------------------------------------
// Core Node Visitors
// ---------------------------------------------------------------------

void ASTVisualizer::visit(ProgramNode& node) {
    // ... (код ProgramNode остается прежним)
    cout << "PROGRAM_BLOCK (" << node.statements.size() << " statements)\n";

    std::string original_indent = indent_;

    for (size_t i = 0; i < node.statements.size(); ++i) {
        bool is_last = (i == node.statements.size() - 1);

        std::string branch_symbol = is_last ? L_BRANCH : H_BRANCH;
        std::string inner_indent_symbol = is_last ? NO_BRANCH : V_BRANCH;

        cout << original_indent << branch_symbol;

        indent_ = original_indent + inner_indent_symbol;

        node.statements[i]->accept(*this);

        indent_ = original_indent;
    }
}

void ASTVisualizer::visit(VarDeclNode& node) {
    // ИСПРАВЛЕНИЕ: Использовать node.typeToString() и node.name
    cout << "VAR_DECL (" << node.typeToString() << " " << node.name << ")\n";
}

void ASTVisualizer::visit(AssignStmtNode& node) {
    // ИСПРАВЛЕНИЕ: Использовать node.identifier_name
    cout << "ASSIGN_STMT (ID: " << node.identifier_name << ")\n";

    std::string original_indent = indent_;

    cout << original_indent << L_BRANCH << "Expression:\n";

    indent_ = original_indent + NO_BRANCH;

    cout << indent_ << L_BRANCH;

    indent_ = original_indent + NO_BRANCH + NO_BRANCH;

    node.expression->accept(*this);

    indent_ = original_indent;
}

void ASTVisualizer::visit(PrintStmtNode& node) {
    // ... (код PrintStmtNode остается прежним)
    cout << "PRINT_STMT\n";

    std::string original_indent = indent_;

    cout << original_indent << L_BRANCH << "Expression:\n";

    indent_ = original_indent + NO_BRANCH;

    cout << indent_ << L_BRANCH;

    indent_ = original_indent + NO_BRANCH + NO_BRANCH;

    node.expression->accept(*this);

    indent_ = original_indent;
}

void ASTVisualizer::visit(IfStmtNode& node) {
    // ... (код IfStmtNode остается прежним)
    cout << "IF_STMT\n";

    std::string original_indent = indent_;
    bool has_else = node.else_body != nullptr;

    cout << original_indent << H_BRANCH << "Condition:\n";

    indent_ = original_indent + V_BRANCH;

    cout << indent_ << L_BRANCH;
    std::string inner_indent = indent_;
    indent_ = inner_indent + NO_BRANCH;
    node.condition->accept(*this);

    indent_ = original_indent;

    std::string then_branch = has_else ? H_BRANCH : L_BRANCH;
    std::string then_inner_indent = has_else ? V_BRANCH : NO_BRANCH;

    cout << original_indent << then_branch << "THEN_BLOCK:\n";

    indent_ = original_indent + then_inner_indent;
    node.then_body->accept(*this);

    indent_ = original_indent;

    if (has_else) {
        cout << original_indent << L_BRANCH << "ELSE_BLOCK:\n";
        indent_ = original_indent + NO_BRANCH;
        node.else_body->accept(*this);
        indent_ = original_indent;
    }
}

void ASTVisualizer::visit(WhileStmtNode& node) {
    // ... (код WhileStmtNode остается прежним)
    cout << "WHILE_STMT\n";

    std::string original_indent = indent_;

    cout << original_indent << H_BRANCH << "Condition:\n";

    indent_ = original_indent + V_BRANCH;

    cout << indent_ << L_BRANCH;
    std::string inner_indent = indent_;
    indent_ = inner_indent + NO_BRANCH;
    node.condition->accept(*this);

    indent_ = original_indent;

    cout << original_indent << L_BRANCH << "BODY_BLOCK:\n";

    indent_ = original_indent + NO_BRANCH;
    node.body->accept(*this);

    indent_ = original_indent;
}

void ASTVisualizer::visit(BinaryOpNode& node) {
    // ИСПРАВЛЕНИЕ: Использовать node.op_type_to_string()
    cout << "BINARY_OP (" << node.op_type_to_string() << ")\n";

    std::string original_indent = indent_;

    // 1. Left (Первый ребенок)
    cout << original_indent << H_BRANCH << "Left:\n";
    indent_ = original_indent + V_BRANCH;

    cout << indent_ << L_BRANCH;
    node.left->accept(*this);

    // 2. Right (Последний ребенок)
    indent_ = original_indent;
    cout << original_indent << L_BRANCH << "Right:\n";
    indent_ = original_indent + NO_BRANCH;

    cout << indent_ << L_BRANCH;
    node.right->accept(*this);

    indent_ = original_indent;
}

// ---------------------------------------------------------------------
// Leaf Node Visitors
// ---------------------------------------------------------------------

void ASTVisualizer::visit(IntLiteralNode& node) {
    cout << "INT_LITERAL (" << node.value << ")\n";
}

void ASTVisualizer::visit(IdentifierNode& node) {
    cout << "IDENTIFIER (" << node.name << ")\n";
}

void ASTVisualizer::visit(TerminalNode& node) {
    // Просто выводим тип токена, если он используется как терминальный узел
    Token temp_token(node.type, TokenClass::UNKNOWN, node.value, 0, 0);
    cout << "TERMINAL (" << temp_token.typeToString() << ": '" << node.value << "')\n";
}