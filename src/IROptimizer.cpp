#include "IROptimizer.h"
#include <stdexcept>
#include <algorithm>
#include <set>
#include <iostream>

using std::to_string;
using std::move;

// Главный метод оптимизации
IRCode IROptimizer::optimize(const IRCode& code) {
    IRCode optimized_code = code;

    // Последовательное выполнение проходов оптимизации
    std::cout << "\n[OPTIMIZER] Starting Constant Folding Pass...\n";
    constantFoldingPass(optimized_code);

    std::cout << "[OPTIMIZER] Starting Redundant Control Flow Pass...\n";
    redundantControlFlowPass(optimized_code);

    // Перенумерация инструкций после удалений
    for (size_t i = 0; i < optimized_code.size(); ++i) {
        optimized_code[i].index = (int)i;
    }

    return optimized_code;
}

// Вычисление константного выражения
Operand IROptimizer::evaluateConstant(IROpCode op, const Operand& arg1, const Operand& arg2) {
    int val1 = arg1.value;
    int val2 = arg2.value;
    int result = 0;

    switch (op) {
        case IROpCode::ADD: result = val1 + val2; break;
        case IROpCode::SUB: result = val1 - val2; break;
        case IROpCode::MUL: result = val1 * val2; break;
        case IROpCode::DIV:
            if (val2 == 0) {
                throw std::runtime_error("Division by zero in Constant Folding.");
            }
            result = val1 / val2; break;

        case IROpCode::CMP_EQ: result = (val1 == val2); break;
        case IROpCode::CMP_NE: result = (val1 != val2); break;
        case IROpCode::CMP_LT: result = (val1 < val2); break;
        case IROpCode::CMP_GT: result = (val1 > val2); break;

        default:
            throw std::runtime_error("Unsupported operation in evaluateConstant.");
    }

    return Operand(result);
}

// Проход свёртки констант
void IROptimizer::constantFoldingPass(IRCode& code) {
    for (Instruction& instr : code) {
        bool is_binary_op = (instr.op == IROpCode::ADD || instr.op == IROpCode::SUB ||
                             instr.op == IROpCode::MUL || instr.op == IROpCode::DIV ||
                             instr.op == IROpCode::CMP_EQ || instr.op == IROpCode::CMP_NE ||
                             instr.op == IROpCode::CMP_LT || instr.op == IROpCode::CMP_GT);

        // Замена бинарной операции с константами на LOAD_IMM
        if (is_binary_op && isLiteral(instr.arg1) && isLiteral(instr.arg2)) {
            try {
                Operand constant_result = evaluateConstant(instr.op, instr.arg1, instr.arg2);
                instr.op = IROpCode::LOAD_IMM;
                instr.arg1 = constant_result;
                instr.arg2 = {};
                std::cout << "[CF] Optimized instruction at index " << instr.index << ".\n";
            } catch (const std::runtime_error& e) {
            }
        }
    }
}

// Оптимизация потока управления: удаление неиспользуемых меток
void IROptimizer::redundantControlFlowPass(IRCode& code) {
    std::set<std::string> used_labels;

    // Сбор используемых меток из переходов
    for (const auto& instr : code) {
        if (instr.op == IROpCode::JMP || instr.op == IROpCode::JMP_IF_ZERO) {
            const Operand& label_op = (instr.op == IROpCode::JMP) ? instr.arg1 : instr.arg2;
            if (label_op.type == OperandType::LABEL) {
                used_labels.insert(label_op.name);
            }
        }
    }

    // Создание нового кода без неиспользуемых меток
    IRCode new_code;
    for (const auto& instr : code) {
        if (instr.op == IROpCode::LABEL) {
            if (used_labels.count(instr.arg1.name) == 0) {
                std::cout << "[CFlow] Removed unused label: " << instr.arg1.name << " at index " << instr.index << ".\n";
                continue;
            }
        }
        new_code.push_back(instr);
    }

    code = move(new_code);
}