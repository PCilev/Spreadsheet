#pragma once

#include "common.h"
#include "formula.h"

#include <optional>

class Impl {
public:
    virtual std::string GetText() const = 0;
    virtual CellInterface::Value GetValue(const SheetInterface& sheet) const = 0;
    virtual std::forward_list<Position> GetReferencedCells() const = 0;
    virtual ~Impl() = default;
};

class EmptyImpl : public Impl {
public:
    std::string GetText() const override {
        return "";
    }
    CellInterface::Value GetValue(const SheetInterface& sheet) const override {
        return 0.0;
    }
    std::forward_list<Position> GetReferencedCells() const {
        return {};
    }
};

class TextImpl : public Impl {
public:
    TextImpl(const std::string& text) : text_{text} {
    }
    
    std::string GetText() const override {
        return text_;
    }
    
    CellInterface::Value GetValue(const SheetInterface& sheet) const override {
        if (text_[0] == '\'') {
            return text_.substr(1); 
        }
        return text_;
    }
    
    std::forward_list<Position> GetReferencedCells() const {
        return {};
    }
    
private:
    std::string text_;
};

class FormulaImpl : public Impl {
public:
    FormulaImpl(const std::string& formula_txt) : formula_{ParseFormula(formula_txt)} {
    }
    
    std::string GetText() const override {
        return "=" + formula_->GetExpression();
    }
    
    CellInterface::Value GetValue(const SheetInterface& sheet) const override {
        auto result = formula_->Evaluate(sheet);
        if (std::holds_alternative<double>(result)) {
            return std::get<double>(result);
        }
        else {
            return std::get<FormulaError>(result);
        }
    }
    
    std::forward_list<Position> GetReferencedCells() const {
        return formula_->GetReferencedCells();
    }
    
private:
    std::unique_ptr<FormulaInterface> formula_;
};

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet) : impl_{std::make_unique<EmptyImpl>()}, table_{sheet} {
    }
    
    ~Cell() = default;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::forward_list<Position> GetReferencedCells() const override {
        return referenced_cells_;
    }
    std::forward_list<Position> GetDependentCells() const {
        return dependent_cells_;
    }
    void AddDependentCell(Position pos) override {
        if (pos.IsValid()){
            dependent_cells_.push_front(pos);
        }
    }
    
    void CacheInvalidation() override;

private:
    std::unique_ptr<Impl> impl_;
    SheetInterface& table_;
    mutable std::optional<Value> cache_;
    std::forward_list<Position> dependent_cells_;
    std::forward_list<Position> referenced_cells_;

};