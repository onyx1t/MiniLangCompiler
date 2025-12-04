#include "IR.h"
#include <sstream>
#include <iomanip>
#include <map>

// --- Вспомогательные функции для преобразования в строку ---

namespace {
    // Карта для IROpCode
    const std::map<IROpCode, std::string> opcodeNames = {
        {IROpCode::ADD, "ADD"}, {IROpCode::SUB, "SUB"},
        {IROpCode::MUL, "MUL"}, {IROpCode::DIV, "DIV"},

        {IROpCode::CMP_EQ, "CMP_EQ"}, {IROpCode::CMP_NE, "CMP_NE"},
        {IROpCode::CMP_LT, "CMP_LT"}, {IROpCode::CMP_GT, "CMP_GT"},

        {IROpCode::ASSIGN, "ASSIGN"}, {IROpCode::LOAD_IMM, "LOAD_IMM"},

        {IROpCode::LABEL, "LABEL"}, {IROpCode::JMP, "JMP"},
        {IROpCode::JMP_IF_ZERO, "JMP_IF_ZERO"},

        {IROpCode::PRINT, "PRINT"}
    };

    // Карта для OperandType
    const std::map<OperandType, std::string> operandTypeNames = {
        {OperandType::NONE, "NONE"}, {OperandType::VARIABLE, "VAR"},
        {OperandType::TEMPORARY, "TEMP"}, {OperandType::LITERAL, "LIT"},
        {OperandType::LABEL, "LABEL"}
    };
}

// Преобразование IROpCode в строку
std::string opCodeToString(IROpCode op) {
    auto it = opcodeNames.find(op);
    if (it != opcodeNames.end()) {
        return it->second;
    }
    return "UNKNOWN_OP";
}

// Преобразование OperandType в строку
std::string operandTypeToString(OperandType type) {
    auto it = operandTypeNames.find(type);
    if (it != operandTypeNames.end()) {
        return it->second;
    }
    return "UNKNOWN_TYPE";
}


// --- Реализация методов Operand ---

// Метод для форматированного вывода операнда
std::string Operand::toString() const {
    switch (type) {
        case OperandType::VARIABLE:
            return name; // Имя переменной исходного кода
        case OperandType::TEMPORARY:
            return name; // Имя временной переменной (например, T1)
        case OperandType::LITERAL:
            return std::to_string(value); // Целочисленная константа
        case OperandType::LABEL:
            return name; // Имя метки (например, L_START)
        case OperandType::NONE:
        default:
            return ""; // Пустая строка
    }
}


// --- Реализация методов Instruction ---

// Метод для форматированного вывода инструкции
std::string Instruction::toString() const {
    std::stringstream ss;

    // Заголовок (например, L1: или 001:)
    if (op == IROpCode::LABEL) {
        // Метка должна быть напечатана первой: L1: LABEL
        ss << arg1.name << ": " << opCodeToString(op);
        return ss.str();
    }

    // Вывод индекса
    ss << std::setw(3) << std::setfill('0') << index << ": ";

    switch (op) {
        case IROpCode::ADD:
        case IROpCode::SUB:
        case IROpCode::MUL:
        case IROpCode::DIV:
            // R = Arg1 OP Arg2 (T1 = A + 10)
            ss << result.toString() << " = " << arg1.toString() << " " << opCodeToString(op) << " " << arg2.toString();
            break;

        case IROpCode::CMP_EQ:
        case IROpCode::CMP_NE:
        case IROpCode::CMP_LT:
        case IROpCode::CMP_GT:
            // R = Arg1 CMP_OP Arg2 (T1 = A < B)
            ss << result.toString() << " = " << arg1.toString() << " " << opCodeToString(op) << " " << arg2.toString();
            break;

        case IROpCode::ASSIGN:
            // R = Arg1 (count = T1)
            ss << result.toString() << " = " << arg1.toString();
            break;

        case IROpCode::LOAD_IMM:
            // R = Literal (count = 10)
            // Мы используем arg1 для хранения литерала
            ss << result.toString() << " = " << arg1.toString();
            break;

        case IROpCode::JMP:
            // JMP L1 (переход к метке)
            ss << opCodeToString(op) << " " << arg1.name;
            break;

        case IROpCode::JMP_IF_ZERO:
            // JMP_IF_ZERO Arg1, L1
            ss << opCodeToString(op) << " " << arg1.toString() << ", " << arg2.name;
            break;

        case IROpCode::PRINT:
            // PRINT Arg1
            ss << opCodeToString(op) << " " << arg1.toString();
            break;

        case IROpCode::LABEL:
            // Должен быть обработан выше
            break;

        default:
            ss << "UNKNOWN INSTRUCTION";
            break;
    }
    return ss.str();
}

// Перегрузка оператора вывода для std::cout << Instruction
std::ostream& operator<<(std::ostream& os, const Instruction& instr) {
    os << instr.toString();
    return os;
}