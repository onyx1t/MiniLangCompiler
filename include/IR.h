#pragma once

#include <string>
#include <vector>
#include <ostream>

// Типы операндов в трёхадресном коде
enum class OperandType {
    NONE,           // Без операнда
    VARIABLE,       // Переменная исходного кода
    TEMPORARY,      // Временная переменная
    LITERAL,        // Целочисленный литерал
    LABEL           // Метка управления потоком
};

// Типы операций трёхадресного кода
enum class IROpCode {
    // Арифметические операции
    ADD,
    SUB,
    MUL,
    DIV,

    // Операции сравнения
    CMP_EQ,
    CMP_NE,
    CMP_LT,
    CMP_GT,

    // Присваивание и загрузка
    ASSIGN,
    LOAD_IMM,

    // Управление потоком
    LABEL,
    JMP,
    JMP_IF_ZERO,

    // Ввод/вывод
    PRINT
};

// Вспомогательные функции для вывода
std::string opCodeToString(IROpCode op);
std::string operandTypeToString(OperandType type);

// Структура операнда
struct Operand {
    OperandType type = OperandType::NONE;
    std::string name;    // Имя переменной/метки
    int value = 0;       // Значение литерала

    Operand() = default;

    // Конструктор для переменных и меток
    Operand(OperandType t, const std::string& n) : type(t), name(n) {
        if (t != OperandType::VARIABLE && t != OperandType::TEMPORARY && t != OperandType::LABEL) {
        }
    }

    // Конструктор для литералов
    Operand(int v) : type(OperandType::LITERAL), value(v) {}

    bool isNone() const { return type == OperandType::NONE; }
    std::string toString() const;
};

// Структура инструкции трёхадресного кода
struct Instruction {
    IROpCode op;         // Код операции
    Operand result;      // Результат
    Operand arg1;        // Первый аргумент
    Operand arg2;        // Второй аргумент
    int index = -1;      // Индекс инструкции

    Instruction(IROpCode op_code, const Operand& res, const Operand& a1, const Operand& a2 = {})
        : op(op_code), result(res), arg1(a1), arg2(a2) {}

    std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const Instruction& instr);
};

// Контейнер для IR-кода
using IRCode = std::vector<Instruction>;