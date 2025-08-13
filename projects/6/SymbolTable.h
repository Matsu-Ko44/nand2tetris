#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>

class SymbolTable {
   public:
    explicit SymbolTable();
    void addEntry(const std::string& symbol, int address);
    bool contains(const std::string& symbol) const;
    int getAddress(const std::string& symbol) const;

   private:
    std::unordered_map<std::string, int> table;
};

#endif  // SYMBOLTABLE_H
