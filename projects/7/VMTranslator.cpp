#include "VMTranslator.h"

#include <filesystem>
#include <iostream>

#include "CodeWriter.h"
#include "Parser.h"

namespace fs = std::filesystem;

VMTranslator::VMTranslator(const std::string& fileName) {
    inputFilename = fileName;
    outputFilename = inputFilename;
    size_t dotPos = outputFilename.rfind(".vm");
    if (dotPos == std::string::npos) {
        std::cerr << "Error: The input file must have .vm extension."
                  << std::endl;
    }
    outputFilename.replace(dotPos, 3, ".asm");
}

void VMTranslator::translate() {
    Parser parser(inputFilename);
    CodeWriter codeWriter(outputFilename);

    try {
        while (parser.hasMoreLines()) {
            parser.advance();
            CommandType currentCommandType = parser.getCommandType();
            if (currentCommandType == CommandType::C_ARITHMETIC) {
                codeWriter.writeArithmetic(parser.arg1());
            }
            if (currentCommandType == CommandType::C_PUSH ||
                currentCommandType == CommandType::C_POP) {
                codeWriter.writePushPop(currentCommandType, parser.arg1(),
                                        parser.arg2());
            }
        }
        codeWriter.writeEndLoop();
        codeWriter.close();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
