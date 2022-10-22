#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

double Calculator::operator()(const Position* pos_ptr) const {
    if (!pos_ptr->IsValid()){
        throw FormulaError(FormulaError::Category::Ref);
    }
    if (!table_.GetCell(*pos_ptr)){
        return 0;
    }
    
    const auto value = table_.GetCell(*pos_ptr)->GetValue();
    if (std::holds_alternative<double>(value)){
        return std::get<double>(value);
    } 
    else {
        try {
            return std::stod(std::get<std::string>(value));
        } 
        catch (std::invalid_argument& err) {
             throw FormulaError(FormulaError::Category::Value);
        }
    }
    
    throw std::get<FormulaError>(value);
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression) : ast_{ParseFormulaAST(expression)} {
    }
    
    Value Evaluate(const SheetInterface& sheet) const override {
        try {
            return ast_.Execute(Calculator(sheet));
        }
        catch(FormulaError& err) {
            return err;
        }
    } 
    
    std::string GetExpression() const override {
        std::ostringstream ost;
        ast_.PrintFormula(ost);
        return ost.str();
    }
    
    std::forward_list<Position> GetReferencedCells() const {
        return ast_.GetCells();
    }

private:
    FormulaAST ast_;
};
    
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    } 
    catch (...) {
        throw FormulaException("Parse Error");
    }
}