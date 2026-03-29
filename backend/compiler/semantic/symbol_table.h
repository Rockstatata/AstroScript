#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

struct Symbol {
    std::string name;
    std::string type;
    double value;
    int declaredLine;
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> table;

public:
    bool insert(const std::string& name, const std::string& type, int declaredLine = -1);
    bool exists(const std::string& name) const;
    double getValue(const std::string& name) const;
    void setValue(const std::string& name, double value);
    std::string getType(const std::string& name) const;
    int getDeclaredLine(const std::string& name) const;
    void printTable() const;
};

#endif
