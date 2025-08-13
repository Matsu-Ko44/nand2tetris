
#include <bitset>
#include <iostream>

#include "Code.h"
#include "Parser.h"
#include "SymbolTable.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "prompt> " << argv[0] << " filename.asm" << std::endl;
        return 1;
    }

    std::string inputFilename = argv[1];

    std::string outputFilename = inputFilename;
    size_t dotPos = outputFilename.rfind(".asm");
    if (dotPos == std::string::npos) {
        std::cerr << "Error: The input file must have the .asm extension."
                  << std::endl;
        return 1;
    }
    outputFilename.replace(dotPos, 4, ".hack");

    SymbolTable symbolTable;
    try {
        Parser labelParser(inputFilename);
        int romAddress = 0;
        while (labelParser.hasMoreLines()) {
            labelParser.advance();
            if (labelParser.instructionType() ==
                InstructionType::L_INSTRUCTION) {
                symbolTable.addEntry(labelParser.symbol(), romAddress);
            } else {
                romAddress++;
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    try {
        Parser mainParser(inputFilename);
        std::ofstream outputFile(outputFilename);
        if (!outputFile.is_open()) {
            std::cerr << "Error: The output file " << outputFilename
                      << "cannot be open." << std::endl;
            return 1;
        }

        int nextRamAddress = 16;

        while (mainParser.hasMoreLines()) {
            mainParser.advance();
            InstructionType type = mainParser.instructionType();

            if (type == InstructionType::A_INSTRUCTION) {
                std::string symbol = mainParser.symbol();
                int address = 0;

                if (isdigit(symbol[0])) {
                    address = std::stoi(symbol);
                } else {
                    if (symbolTable.contains(symbol)) {
                        address = symbolTable.getAddress(symbol);
                    } else {
                        address = nextRamAddress;
                        symbolTable.addEntry(symbol, nextRamAddress);
                        nextRamAddress++;
                    }
                }
                outputFile << std::bitset<16>(address).to_string() << std::endl;
            } else if (type == InstructionType::C_INSTRUCTION) {
                std::string compBits = Code::comp(mainParser.comp());
                std::string destBits = Code::dest(mainParser.dest());
                std::string jumpBits = Code::jump(mainParser.jump());

                outputFile << "111" << compBits << destBits << jumpBits
                           << std::endl;
            }
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
