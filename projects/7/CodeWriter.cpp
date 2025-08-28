#include "CodeWriter.h"

#include <filesystem>

namespace fs = std::filesystem;

CodeWriter::CodeWriter(const std::string &fileName) : outputFile(fileName) {
    if (!outputFile.is_open()) {
        throw std::runtime_error("File cannot open: " + fileName);
    }
    fs::path p(fileName);
    vmFileName = p.stem().string();
}

void CodeWriter::writeArithmetic(const std::string &command) {
    outputFile << "// " << command << std::endl;

    if (command == "neg" || command == "not") {
        outputFile << "@SP" << std::endl;
        outputFile << "A=M-1" << std::endl;
        outputFile << "D=M" << std::endl;
        if (command == "neg") {
            outputFile << "M=-D" << std::endl;
        } else if (command == "not") {
            outputFile << "M=!D" << std::endl;
        }
    } else {
        outputFile << "@SP" << std::endl;
        outputFile << "AM=M-1" << std::endl;
        outputFile << "D=M" << std::endl;
        outputFile << "A=A-1" << std::endl;

        if (command == "add") {
            outputFile << "M=M+D" << std::endl;
        } else if (command == "sub") {
            outputFile << "M=M-D" << std::endl;
        } else if (command == "and") {
            outputFile << "M=M&D" << std::endl;
        } else if (command == "or") {
            outputFile << "M=M|D" << std::endl;
        } else if (command == "eq" || command == "gt" || command == "lt") {
            std::string trueLable = "JUMP_TRUE_" + std::to_string(jumpCounter);
            std::string endLabel = "JUMP_END_" + std::to_string(jumpCounter);
            jumpCounter++;

            std::string mnemonic;
            if (command == "eq") {
                mnemonic = "JEQ";
            } else if (command == "gt") {
                mnemonic = "JGT";
            } else if (command == "lt") {
                mnemonic = "JLT";
            }

            outputFile << "D=M-D" << std::endl;
            outputFile << "@" << trueLable << std::endl;
            outputFile << "D;" << mnemonic << std::endl;
            // false
            outputFile << "@SP" << std::endl;
            outputFile << "A=M-1" << std::endl;
            outputFile << "M=0" << std::endl;
            outputFile << "@" << endLabel << std::endl;
            outputFile << "0;JMP" << std::endl;
            // true
            outputFile << "(" << trueLable << ")" << std::endl;
            outputFile << "@SP" << std::endl;
            outputFile << "A=M-1" << std::endl;
            outputFile << "M=-1" << std::endl;
            // end
            outputFile << "(" << endLabel << ")" << std::endl;
        } else {
            throw std::runtime_error("Unknown command: " + command);
        }
    }
}

void CodeWriter::writePushPop(const commandType command,
                              const std::string &segment, const int index) {
    std::string mnemonic;
    if (segment == "local") {
        mnemonic = "LCL";
    } else if (segment == "argument") {
        mnemonic = "ARG";
    } else if (segment == "this") {
        mnemonic = "THIS";
    } else if (segment == "that") {
        mnemonic = "THAT";
    } else if (segment == "pointer") {
        mnemonic = "POINTER";
    } else if (segment == "temp") {
        mnemonic = "TEMP";
    } else if (segment == "constant") {
        mnemonic = "CONSTANT";
    } else if (segment == "static") {
        mnemonic = "STATIC";
    } else {
        throw std::runtime_error("Unknown segment: " + segment);
    }

    if (command == commandType::C_PUSH) {
        outputFile << "// push " << segment << " " << index << std::endl;

        if (mnemonic == "POINTER") {
            if (index == 0) {
                outputFile << "@3" << std::endl;
                outputFile << "D=M" << std::endl;
            }
            if (index == 1) {
                outputFile << "@4" << std::endl;
                outputFile << "D=M" << std::endl;
            }
        } else if (mnemonic == "TEMP") {
            outputFile << "@5" << std::endl;
            outputFile << "D=A" << std::endl;
            outputFile << "@" << index << std::endl;
            outputFile << "A=D+A" << std::endl;
            outputFile << "D=M" << std::endl;
        } else if (mnemonic == "CONSTANT") {
            outputFile << "@" << index << std::endl;
            outputFile << "D=A" << std::endl;
        } else if (mnemonic == "STATIC") {
            outputFile << "@" << vmFileName << "." << index << std::endl;
            outputFile << "D=M" << std::endl;
        } else {
            outputFile << "@" << mnemonic << std::endl;
            outputFile << "D=M" << std::endl;
            outputFile << "@" << index << std::endl;
            outputFile << "A=D+A" << std::endl;
            outputFile << "D=M" << std::endl;
        }
        outputFile << "@SP" << std::endl;
        outputFile << "A=M" << std::endl;
        outputFile << "M=D" << std::endl;
        outputFile << "@SP" << std::endl;
        outputFile << "M=M+1" << std::endl;
    }
    if (command == commandType::C_POP) {
        outputFile << "// pop " << segment << " " << index << std::endl;

        if (mnemonic == "POINTER") {
            if (index == 0) {
                outputFile << "@3" << std::endl;
                outputFile << "D=A" << std::endl;
            }
            if (index == 1) {
                outputFile << "@4" << std::endl;
                outputFile << "D=A" << std::endl;
            }
        } else if (mnemonic == "TEMP") {
            outputFile << "@5" << std::endl;
            outputFile << "D=A" << std::endl;
            outputFile << "@" << index << std::endl;
            outputFile << "D=D+A" << std::endl;
        } else if (mnemonic == "STATIC") {
            outputFile << "@" << vmFileName << "." << index << std::endl;
            outputFile << "D=A" << std::endl;
        } else {
            outputFile << "@" << mnemonic << std::endl;
            outputFile << "D=M" << std::endl;
            outputFile << "@" << index << std::endl;
            outputFile << "D=D+A" << std::endl;
        }
        outputFile << "@R13" << std::endl;
        outputFile << "M=D" << std::endl;

        outputFile << "@SP" << std::endl;
        outputFile << "AM=M-1" << std::endl;
        outputFile << "D=M" << std::endl;
        outputFile << "@R13" << std::endl;
        outputFile << "A=M" << std::endl;
        outputFile << "M=D" << std::endl;
    }
}

void CodeWriter::writeEndLoop() {
    outputFile << "(END)" << std::endl;
    outputFile << "@END" << std::endl;
    outputFile << "0;JMP" << std::endl;
}

void CodeWriter::close() { outputFile.close(); }
