#include "symbol_table.h"

#include <iostream>

bool SymbolTable::insert(const std::string& name, const std::string& type, int declaredLine) {
    if (exists(name)) {
        return false;
    }

    table[name] = Symbol{name, type, 0.0, declaredLine};
    return true;
}

bool SymbolTable::exists(const std::string& name) const {
    return table.find(name) != table.end();
}

double SymbolTable::getValue(const std::string& name) const {
    auto it = table.find(name);
    if (it != table.end()) {
        return it->second.value;
    }
    return 0.0;
}

void SymbolTable::setValue(const std::string& name, double value) {
    auto it = table.find(name);
    if (it != table.end()) {
        it->second.value = value;
    }
}

std::string SymbolTable::getType(const std::string& name) const {
    auto it = table.find(name);
    if (it != table.end()) {
        return it->second.type;
    }
    return "";
}

int SymbolTable::getDeclaredLine(const std::string& name) const {
    auto it = table.find(name);
    if (it != table.end()) {
        return it->second.declaredLine;
    }
    return -1;
}

void SymbolTable::printTable() const {
    std::cout << "\n--- Symbol Table ---\n";
    for (const auto& entry : table) {
        const Symbol& symbol = entry.second;
        std::cout << "Name: " << symbol.name
                  << ", Type: " << symbol.type
                  << ", Value: " << symbol.value;
        if (symbol.declaredLine > 0) {
            std::cout << ", DeclaredLine: " << symbol.declaredLine;
        }
        std::cout << '\n';
    }
    std::cout << "--------------------\n";
}
