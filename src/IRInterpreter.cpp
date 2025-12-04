#include "IRInterpreter.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

using std::to_string;
using std::runtime_error;

// Получение значения операнда
int IRInterpreter::getValue(const Operand& op) const {
    switch (op.type) {
        case OperandType::LITERAL:
            return op.value;
        case OperandType::VARIABLE:
        case OperandType::TEMPORARY: {
            auto it = memory_.find(op.name);
            if (it != memory_.end()) {
                return it->second;
            }
            throw runtime_error("Runtime Error: Variable/Temp '" + op.name + "' used before assignment.");
        }
        case OperandType::NONE:
        case OperandType::LABEL:
        default:
            throw runtime_error("Runtime Error: Attempt to read value from invalid operand type (" + operandTypeToString(op.type) + ").");
    }
}

// Сохранение значения в переменную
void IRInterpreter::setValue(const Operand& target, int value) {
    if (target.type != OperandType::VARIABLE && target.type != OperandType::TEMPORARY) {
        throw runtime_error("Runtime Error: Attempt to write value to invalid operand type (" + operandTypeToString(target.type) + ").");
    }
    memory_[target.name] = value;
}

// Построение карты меток для переходов
void IRInterpreter::buildLabelMap(const IRCode& code) {
    label_map_.clear();
    for (size_t i = 0; i < code.size(); ++i) {
        const Instruction& instr = code[i];
        if (instr.op == IROpCode::LABEL) {
            if (instr.arg1.type == OperandType::LABEL) {
                label_map_[instr.arg1.name] = (int)i + 1;
            } else {
                throw runtime_error("Interpreter Error: LABEL instruction missing label name.");
            }
        }
    }
}

// Основной цикл выполнения IR-кода
void IRInterpreter::execute(const IRCode& code) {
    if (code.empty()) {
        std::cout << "[INTERPRETER] IR Code is empty. Nothing to execute.\n";
        return;
    }

    std::cout << "\n========================================\n";
    std::cout << "IR INTERPRETER START\n";
    std::cout << "========================================\n";

    try {
        buildLabelMap(code);
    } catch (const runtime_error& e) {
        std::cerr << "Label Map Building Failed: " << e.what() << "\n";
        return;
    }

    int pc = 0;
    while (pc < (int)code.size()) {
        const Instruction& instr = code[pc];
        int next_pc = pc + 1;

        std::cout << "PC " << pc << ": Executing " << instr.toString() << "\n";

        try {
            switch (instr.op) {
                case IROpCode::ADD:
                case IROpCode::SUB:
                case IROpCode::MUL:
                case IROpCode::DIV: {
                    int val1 = getValue(instr.arg1);
                    int val2 = getValue(instr.arg2);
                    int result;

                    if (instr.op == IROpCode::DIV && val2 == 0) {
                        throw runtime_error("Division by zero at runtime.");
                    }

                    if (instr.op == IROpCode::ADD) result = val1 + val2;
                    else if (instr.op == IROpCode::SUB) result = val1 - val2;
                    else if (instr.op == IROpCode::MUL) result = val1 * val2;
                    else result = val1 / val2;

                    setValue(instr.result, result);
                    break;
                }

                case IROpCode::CMP_EQ:
                case IROpCode::CMP_NE:
                case IROpCode::CMP_LT:
                case IROpCode::CMP_GT: {
                    int val1 = getValue(instr.arg1);
                    int val2 = getValue(instr.arg2);
                    int result = 0;

                    if (instr.op == IROpCode::CMP_EQ) result = (val1 == val2);
                    else if (instr.op == IROpCode::CMP_NE) result = (val1 != val2);
                    else if (instr.op == IROpCode::CMP_LT) result = (val1 < val2);
                    else if (instr.op == IROpCode::CMP_GT) result = (val1 > val2);

                    setValue(instr.result, result);
                    break;
                }

                case IROpCode::ASSIGN: {
                    int val = getValue(instr.arg1);
                    setValue(instr.result, val);
                    break;
                }
                case IROpCode::LOAD_IMM: {
                    int val = getValue(instr.arg1);
                    setValue(instr.result, val);
                    break;
                }

                case IROpCode::LABEL: {
                    break;
                }
                case IROpCode::JMP: {
                    auto it = label_map_.find(instr.arg1.name);
                    if (it == label_map_.end()) {
                        throw runtime_error("Undefined label target for JMP: " + instr.arg1.name);
                    }
                    next_pc = it->second;
                    break;
                }
                case IROpCode::JMP_IF_ZERO: {
                    int condition_val = getValue(instr.arg1);
                    if (condition_val == 0) {
                        auto it = label_map_.find(instr.arg2.name);
                        if (it == label_map_.end()) {
                            throw runtime_error("Undefined label target for JMP_IF_ZERO: " + instr.arg2.name);
                        }
                        next_pc = it->second;
                    }
                    break;
                }

                case IROpCode::PRINT: {
                    int val = getValue(instr.arg1);
                    std::cout << ">>> PRINT OUTPUT: " << val << "\n";
                    break;
                }

                default:
                    throw runtime_error("Unknown IROpCode encountered: " + opCodeToString(instr.op));
            }
        } catch (const runtime_error& e) {
            std::cerr << "\nRuntime Error at index " << instr.index << ": " << e.what() << "\n";
            std::cerr << "Execution Aborted.\n";
            return;
        }

        pc = next_pc;
    }

    std::cout << "\n========================================\n";
    std::cout << "IR INTERPRETER FINISHED (EOF)\n";
    std::cout << "========================================\n";
}