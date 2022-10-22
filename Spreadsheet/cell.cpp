#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

void Cell::Set(std::string text) {
    if (!text.size()) {
        impl_ = std::move(std::make_unique<EmptyImpl>());
        return;
    }
    
    if (text[0] == '=' && text.size() > 1) {
        auto temp = std::make_unique<FormulaImpl>(text.substr(1));
        impl_ = std::move(temp);
    }
    else {
        impl_ = std::make_unique<TextImpl>(text);
    }
    
    referenced_cells_ = impl_->GetReferencedCells();
    CacheInvalidation();
    cache_ = impl_->GetValue(table_);
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
    if (!cache_){
        cache_ = impl_->GetValue(table_);
    }
    return cache_.value();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

void Cell::CacheInvalidation() {
    cache_ = std::nullopt;

    for (const auto& pos : dependent_cells_){
        table_.GetCell(pos)->CacheInvalidation();
    }
}