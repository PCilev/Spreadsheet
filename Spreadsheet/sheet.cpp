#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <set>

using namespace std::literals;

struct ValuePrinter {  
    std::ostream& ost;
    
    void operator()(double d) const {
        ost << d;
    }
    void operator()(const std::string& str) const {
        ost << str;
    }
    void operator()(const FormulaError& err) const {
        ost << err;
    }
};

Sheet::~Sheet() = default;

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position"s);
    }
    
    auto temp = std::make_unique<Cell>(*this);
    temp->Set(text);
    CheckCircularDependence(temp.get(), pos);
    
    for (const auto& ref_pos : temp->GetReferencedCells()){
        CellInterface* referenced_cell = GetCell(ref_pos);
        referenced_cell->AddDependentCell(pos);
    }

    table_[pos] = std::move(temp);
    
    if (is_empty) {
        is_empty = false;
        min_curr_pos = pos;
        max_curr_pos = pos;
        return;
    }

    if (pos.row < min_curr_pos.row) {
        min_curr_pos.row = pos.row;
    }
    if (pos.col < min_curr_pos.col) {
        min_curr_pos.col = pos.col;
    }
    if (pos.row > max_curr_pos.row) {
        max_curr_pos.row = pos.row;
    }
    if (pos.col > max_curr_pos.col) {
        max_curr_pos.col = pos.col;
    }
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()){
        throw InvalidPositionException("Invalid position"s);
    }
    
    if (!table_.count(pos)) {
        return nullptr;
    }
    return table_.at(pos).get();
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()){
        throw InvalidPositionException("Invalid position"s);
    }
    
    if (!table_.count(pos)) {
        return nullptr;
    }
    return table_.at(pos).get();
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()){
        throw InvalidPositionException("Invalid position"s);
    }
    if (!table_.count(pos)) {
        return;
    }
    
    table_.erase(pos);
    
    if (!table_.size()) {
        is_empty = true;
        min_curr_pos = Position::NONE;
        max_curr_pos = Position::NONE;;
        return;
    }
    
    if (pos == min_curr_pos) {
        min_curr_pos = FindNewMin();
    }
    if (pos == max_curr_pos) {
        max_curr_pos = FindNewMax();
    } 
}

Size Sheet::GetPrintableSize() const {
    if (is_empty) {
        return {0, 0};
    }
    return {max_curr_pos.row - min_curr_pos.row + 1, max_curr_pos.col - min_curr_pos.col + 1};
}

void Sheet::PrintValues(std::ostream& output) const {
    if (is_empty) {
        return;
    }
    
    for (int i = min_curr_pos.row; i <= max_curr_pos.row; ++i) {
        for (int j = min_curr_pos.col; j <= max_curr_pos.col; ++j) {
            if (table_.count({i, j})) {
                std::visit(ValuePrinter{output}, table_.at({i,j})->GetValue());
            }
            if (j != max_curr_pos.col) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

void Sheet::PrintTexts(std::ostream& output) const {
    if (is_empty) {
        return;
    }
    
    for (int i = min_curr_pos.row; i <= max_curr_pos.row; ++i) {
        for (int j = min_curr_pos.col; j <= max_curr_pos.col; ++j) {
            if (table_.count({i, j})) {
                output << table_.at({i,j})->GetText();
            }
            if (j != max_curr_pos.col) {
                output << '\t';
            }
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}

Position Sheet::FindNewMin() {
    Position new_pos = table_.begin()->first;
    
    for (const auto& [position, cell] : table_) {
        if (position > new_pos){
            new_pos = position;
        }
    }
    
    return new_pos;
}

Position Sheet::FindNewMax() {
    Position new_pos = table_.begin()->first;
    
    for (const auto& [position, cell] : table_) {
        if (new_pos < position){
            new_pos = position;
        }
    }
    
    return new_pos;
}

void Sheet::CheckCircularDependence(CellInterface* tmp, Position pos) {
    for (const auto& curr_pos : tmp->GetReferencedCells()) {
        if (curr_pos == pos){
            throw CircularDependencyException("");
        }
        if (GetCell(pos)) {
            CheckCircularDependence(GetCell(pos), pos);
        }
    }
}