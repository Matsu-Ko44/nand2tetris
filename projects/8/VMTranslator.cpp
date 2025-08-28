#include "VMTranslator.h"

#include <filesystem>

#include "CodeWriter.h"
#include "Parser.h"

namespace fs = std::filesystem;

VMTranslator::VMTranslator(const std::string& fileName) {
    size_t dotPos = fileName.rfind(".vm");
    if (dotPos == std::string::npos) {
        for (const auto& entry : fs::directory_iterator(fileName)) {
            if (entry.is_regular_file() && entry.path().extension() == ".vm") {
                vmFiles.push_back(entry.path().string());
            }
        }
        outputFilename =
            fileName + "/" + fs::path(fileName).stem().string() + ".asm";
    } else {
        vmFiles.push_back(fileName);
        outputFilename = fileName;
        outputFilename.replace(dotPos, 3, ".asm");
    }
}

void VMTranslator::translate() {
    CodeWriter codeWriter(outputFilename);
    for (const auto& inputFilename : vmFiles) {
        Parser parser(inputFilename);
        codeWriter.setFileName(inputFilename);

        while (parser.hasMoreLines()) {
            parser.advance();
            commandType currentCommandType = parser.getCommandType();
            if (currentCommandType == commandType::C_ARITHMETIC) {
                codeWriter.writeArithmetic(parser.arg1());
            } else if (currentCommandType == commandType::C_PUSH ||
                       currentCommandType == commandType::C_POP) {
                codeWriter.writePushPop(currentCommandType, parser.arg1(),
                                        parser.arg2());
            } else if (currentCommandType == commandType::C_LABEL) {
                codeWriter.writeLabel(parser.arg1());
            } else if (currentCommandType == commandType::C_GOTO) {
                codeWriter.writeGoto(parser.arg1());
            } else if (currentCommandType == commandType::C_IF) {
                codeWriter.writeIf(parser.arg1());
            } else if (currentCommandType == commandType::C_FUNCTION) {
                codeWriter.writeFunction(parser.arg1(), parser.arg2());
            } else if (currentCommandType == commandType::C_CALL) {
                codeWriter.writeCall(parser.arg1(), parser.arg2());
            } else if (currentCommandType == commandType::C_RETURN) {
                codeWriter.writeReturn();
            }
        }
        // codeWriter.writeEndLoop();
    }
    codeWriter.close();
}
