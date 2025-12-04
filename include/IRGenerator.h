#pragma once

#include "AST.h"
#include "IR.h"
#include "ErrorHandler.h"

// Генератор трёхадресного кода из AST
class IRGenerator : public ASTVisitor {
private:
    IRCode ir_code_;                 // Сгенерированный IR-код
    ErrorHandler* error_handler_;    // Обработчик ошибок
    int temp_counter_ = 0;           // Счётчик временных переменных
    int label_counter_ = 0;          // Счётчик меток
    Operand result_operand_;         // Результат последнего посещённого выражения

    // Вспомогательные методы
    Operand makeTemp();              // Создание временной переменной
    Operand makeLabel();             // Создание метки
    void emit(IROpCode op, const Operand& result, const Operand& arg1 = {}, const Operand& arg2 = {}); // Добавление инструкции
    IROpCode tokenTypeToIROpCode(TokenType type) const; // Конвертация типа токена в IR-операцию

public:
    IRGenerator(ErrorHandler* handler);

    // Основной метод генерации
    IRCode generate(ASTNode* root);

    // Методы обхода AST
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