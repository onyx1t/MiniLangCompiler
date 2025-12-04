#include "ASTVisualizer.h"
#include <iostream>
#include <sstream>

using std::cout;

// Символы для визуализации дерева
const std::string V_BRANCH = "|   ";
const std::string H_BRANCH = "|-- ";
const std::string L_BRANCH = "`-- ";
const std::string NO_BRANCH = "    ";

// Обработка блока программы
void ASTVisualizer::visit(ProgramNode& node) {
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

// Обработка объявления переменной
void ASTVisualizer::visit(VarDeclNode& node) {
    cout << "VAR_DECL (" << node.typeToString() << " " << node.name << ")\n";
}

// Обработка оператора присваивания
void ASTVisualizer::visit(AssignStmtNode& node) {
    cout << "ASSIGN_STMT (ID: " << node.identifier_name << ")\n";
    std::string original_indent = indent_;
    cout << original_indent << L_BRANCH << "Expression:\n";
    indent_ = original_indent + NO_BRANCH;
    cout << indent_ << L_BRANCH;
    indent_ = original_indent + NO_BRANCH + NO_BRANCH;
    node.expression->accept(*this);
    indent_ = original_indent;
}

// Обработка оператора вывода
void ASTVisualizer::visit(PrintStmtNode& node) {
    cout << "PRINT_STMT\n";
    std::string original_indent = indent_;
    cout << original_indent << L_BRANCH << "Expression:\n";
    indent_ = original_indent + NO_BRANCH;
    cout << indent_ << L_BRANCH;
    indent_ = original_indent + NO_BRANCH + NO_BRANCH;
    node.expression->accept(*this);
    indent_ = original_indent;
}

// Обработка условного оператора
void ASTVisualizer::visit(IfStmtNode& node) {
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

// Обработка цикла while
void ASTVisualizer::visit(WhileStmtNode& node) {
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

// Обработка бинарной операции
void ASTVisualizer::visit(BinaryOpNode& node) {
    cout << "BINARY_OP (" << node.op_type_to_string() << ")\n";
    std::string original_indent = indent_;

    cout << original_indent << H_BRANCH << "Left:\n";
    indent_ = original_indent + V_BRANCH;
    cout << indent_ << L_BRANCH;
    node.left->accept(*this);
    indent_ = original_indent;

    cout << original_indent << L_BRANCH << "Right:\n";
    indent_ = original_indent + NO_BRANCH;
    cout << indent_ << L_BRANCH;
    node.right->accept(*this);
    indent_ = original_indent;
}

// Обработка листьев дерева
void ASTVisualizer::visit(IntLiteralNode& node) {
    cout << "INT_LITERAL (" << node.value << ")\n";
}

void ASTVisualizer::visit(IdentifierNode& node) {
    cout << "IDENTIFIER (" << node.name << ")\n";
}

void ASTVisualizer::visit(TerminalNode& node) {
    Token temp_token(node.type, TokenClass::UNKNOWN, node.value, 0, 0);
    cout << "TERMINAL (" << temp_token.typeToString() << ": '" << node.value << "')\n";
}