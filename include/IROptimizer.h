#pragma once

#include "IR.h"
#include <vector>

// Оптимизатор трёхадресного кода
class IROptimizer {
private:
    // Основные проходы оптимизации
    void constantFoldingPass(IRCode& code);          // Свёртка констант
    void redundantControlFlowPass(IRCode& code);     // Упрощение потока управления

    // Вспомогательные методы
    bool isLiteral(const Operand& op) const { return op.type == OperandType::LITERAL; }
    Operand evaluateConstant(IROpCode op, const Operand& arg1, const Operand& arg2); // Вычисление константных выражений

public:
    IROptimizer() = default;

    // Основной метод оптимизации
    IRCode optimize(const IRCode& code);
};