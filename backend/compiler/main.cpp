#include <cstdio>
#include <iostream>

#include "semantic/symbol_table.h"
#include "ir/tac.h"

extern int yyparse();
extern FILE* yyin;

extern SymbolTable symbolTable;
extern TACGenerator tacGenerator;

int main(int argc, char** argv)
{
    std::cout << "AstroScript Compiler Running\n";

    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            std::cerr << "Error: Cannot open file '" << argv[1] << "'\n";
            return 1;
        }
        yyin = file;
    }

    int parseStatus = yyparse();
    if (parseStatus == 0) {
        tacGenerator.optimize();
        tacGenerator.printCode("Optimized Three Address Code");
        tacGenerator.execute();
        symbolTable.printTable();
    }

    return parseStatus;
}
