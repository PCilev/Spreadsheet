#pragma once

#include "common.h"

#include <memory>

class Calculator {
public:
    Calculator(const SheetInterface& table) : table_{table} {
    }
    
    double operator()(const Position* pos_ptr) const;
private:
    const SheetInterface& table_;
};

// Формула, позволяющая вычислять и обновлять арифметическое выражение.
// Поддерживаемые возможности:
// * Простые бинарные операции и числа, скобки: 1+2*3, 2.5*(2+3.5/7)
// * Значения ячеек в качестве переменных: A1+B2*C3
// Ячейки, указанные в формуле, могут быть как формулами, так и текстом. Если это
// текст, но он представляет число, тогда его нужно трактовать как число. Пустая
// ячейка или ячейка с пустым текстом трактуется как число ноль.
class FormulaInterface {
public:
    using Value = std::variant<double, FormulaError>;

    virtual ~FormulaInterface() = default;

    virtual Value Evaluate(const SheetInterface& sheet) const = 0;

    // Возвращает выражение, которое описывает формулу.
    // Не содержит пробелов и лишних скобок.
    virtual std::string GetExpression() const = 0;

    // Возвращает список ячеек, которые непосредственно задействованы в вычислении
    // формулы. Список отсортирован по возрастанию и не содержит повторяющихся
    // ячеек.
    virtual std::forward_list<Position> GetReferencedCells() const = 0;
};

// Парсит переданное выражение и возвращает объект формулы.
// Бросает FormulaException в случае, если формула синтаксически некорректна.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);