#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>
#include <cmath>

struct PosHash {
    size_t operator() (Position pos) const {
        return p_hash_(pos.row) * static_cast<size_t>(std::pow(59, 4)) + p_hash_(pos.col) * 59;
    } 
private:
    std::hash<int> p_hash_;
};

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

private:
    std::unordered_map<Position, std::unique_ptr<Cell>, PosHash> table_;
    Position min_curr_pos = Position::NONE;
    Position max_curr_pos = Position::NONE;
    bool is_empty = true;
    
    Position FindNewMin();
    Position FindNewMax();

    void CheckCircularDependence(CellInterface* tmp, Position pos);
};