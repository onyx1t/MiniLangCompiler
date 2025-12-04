#pragma once

#include "IR.h"
#include <map>
#include <string>
#include <iostream>

// Интерпретатор трёхадресного кода
class IRInterpreter {
private:
    std::map<std::string, int> memory_;      // Память для переменных
    std::map<std::string, int> label_map_;   // Карта меток

    // Получение значения операнда
    int getValue(const Operand& op) const;

    // Сохранение значения в переменную
    void setValue(const Operand& target, int value);

    // Построение карты меток
    void buildLabelMap(const IRCode& code);

public:
    IRInterpreter() = default;

    // Выполнение IR-кода
    void execute(const IRCode& code);
};