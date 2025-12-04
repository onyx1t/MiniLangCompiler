#include "IRGenerator.h"
#include <stdexcept>

using std::to_string;
using std::move;

// Конструктор и основной метод генерации
IRGenerator::IRGenerator(ErrorHandler* handler)
    : error_handler_(handler) {
}

IRCode IRGenerator::generate(ASTNode* root) {
    if (!root) {
        return {};
    }

    // Сброс состояния генератора
    ir_code_.clear();
    temp_counter_ = 0;
    label_counter_ = 0;

    root->accept(*this);

    // Назначение индексов инструкциям
    for (size_t i = 0; i < ir_code_.size(); ++i) {
        ir_code_[i].index = (int)i;
    }

    return move(ir_code_);
}

// Создание временной переменной
Operand IRGenerator::makeTemp() {
    temp_counter_++;
    return Operand(OperandType::TEMPORARY, "T" + to_string(temp_counter_));
}

// Создание метки
Operand IRGenerator::makeLabel() {
    label_counter_++;
    return Operand(OperandType::LABEL, "L" + to_string(label_counter_));
}

// Добавление инструкции в IR-код
void IRGenerator::emit(IROpCode op, const Operand& result, const Operand& arg1, const Operand& arg2) {
    ir_code_.emplace_back(op, result, arg1, arg2);
}

// Конвертация типа токена в IR-операцию
IROpCode IRGenerator::tokenTypeToIROpCode(TokenType type) const {
    switch (type) {
        case TokenType::TOKEN_PLUS: return IROpCode::ADD;
        case TokenType::TOKEN_MINUS: return IROpCode::SUB;
        case TokenType::TOKEN_MULTIPLY: return IROpCode::MUL;
        case TokenType::TOKEN_DIVIDE: return IROpCode::DIV;

        case TokenType::TOKEN_EQUAL: return IROpCode::CMP_EQ;
        case TokenType::TOKEN_NOT_EQUAL: return IROpCode::CMP_NE;
        case TokenType::TOKEN_LESS: return IROpCode::CMP_LT;
        case TokenType::TOKEN_GREATER: return IROpCode::CMP_GT;

        default:
            throw std::runtime_error("IR Generation Error: Unknown operator token type in expression.");
    }
}

// Обработка целочисленного литерала
void IRGenerator::visit(IntLiteralNode& node) {
    result_operand_ = Operand(node.value);
}

// Обработка идентификатора
void IRGenerator::visit(IdentifierNode& node) {
    result_operand_ = Operand(OperandType::VARIABLE, node.name);
}

// Обработка бинарной операции
void IRGenerator::visit(BinaryOpNode& node) {
    node.left->accept(*this);
    Operand left_op = result_operand_;

    node.right->accept(*this);
    Operand right_op = result_operand_;

    IROpCode op_code = tokenTypeToIROpCode(node.op);
    Operand result_temp = makeTemp();

    emit(op_code, result_temp, left_op, right_op);
    result_operand_ = result_temp;
}

// Неожиданное посещение терминального узла
void IRGenerator::visit(TerminalNode& node) {
    error_handler_->registerError("IR Generation", "Visiting TerminalNode unexpectedly.", 0, 0);
    result_operand_ = {};
}

// Обработка блока программы
void IRGenerator::visit(ProgramNode& node) {
    for (const auto& stmt : node.statements) {
        stmt->accept(*this);
    }
}

// Объявление переменной не требует IR-инструкции
void IRGenerator::visit(VarDeclNode& node) {
}

// Генерация кода для оператора присваивания
void IRGenerator::visit(AssignStmtNode& node) {
    node.expression->accept(*this);
    Operand rhs_op = result_operand_;
    Operand target_var = Operand(OperandType::VARIABLE, node.identifier_name);

    if (rhs_op.type == OperandType::LITERAL) {
        emit(IROpCode::LOAD_IMM, target_var, rhs_op);
    } else {
        emit(IROpCode::ASSIGN, target_var, rhs_op);
    }
}

// Генерация кода для оператора вывода
void IRGenerator::visit(PrintStmtNode& node) {
    node.expression->accept(*this);
    Operand print_op = result_operand_;
    emit(IROpCode::PRINT, {}, print_op);
}

// Генерация кода для условного оператора
void IRGenerator::visit(IfStmtNode& node) {
    Operand label_else = makeLabel();
    Operand label_end = makeLabel();

    node.condition->accept(*this);
    Operand condition_temp = result_operand_;

    emit(IROpCode::JMP_IF_ZERO, {}, condition_temp, label_else);
    node.then_body->accept(*this);

    if (node.else_body) {
        emit(IROpCode::JMP, {}, label_end);
    }

    emit(IROpCode::LABEL, {}, label_else);
    if (node.else_body) {
        node.else_body->accept(*this);
    }

    emit(IROpCode::LABEL, {}, label_end);
}

// Генерация кода для цикла while
void IRGenerator::visit(WhileStmtNode& node) {
    Operand label_start = makeLabel();
    Operand label_end = makeLabel();

    emit(IROpCode::LABEL, {}, label_start);
    node.condition->accept(*this);
    Operand condition_temp = result_operand_;

    emit(IROpCode::JMP_IF_ZERO, {}, condition_temp, label_end);
    node.body->accept(*this);
    emit(IROpCode::JMP, {}, label_start);
    emit(IROpCode::LABEL, {}, label_end);
}