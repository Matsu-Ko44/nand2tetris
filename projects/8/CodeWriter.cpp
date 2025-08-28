#include "CodeWriter.h"

#include <filesystem>

namespace fs = std::filesystem;

CodeWriter::CodeWriter(const std::string &fileName) : outputFile(fileName) {
    if (!outputFile.is_open()) {
        throw std::runtime_error("File cannot open: " + fileName);
    }
    outputFile << "// boot strap" << std::endl;
    outputFile << "@256" << std::endl;
    outputFile << "D=A" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "M=D" << std::endl;
    writeCall("Sys.init", 0);
}

void CodeWriter::setFileName(const std::string &fileName) {
    fs::path p(fileName);
    vmFilename = p.stem().string();
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
            outputFile << "@" << vmFilename << "." << index << std::endl;
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
            outputFile << "@" << vmFilename << "." << index << std::endl;
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

void CodeWriter::writeLabel(const std::string &label) {
    outputFile << "// label " << label << std::endl;

    outputFile << "(" << label << ")" << std::endl;
}

void CodeWriter::writeGoto(const std::string &label) {
    outputFile << "// goto " << label << std::endl;

    outputFile << "@" << label << std::endl;
    outputFile << "0;JMP" << std::endl;
}

void CodeWriter::writeIf(const std::string &label) {
    outputFile << "// if-goto " << label << std::endl;

    outputFile << "@SP" << std::endl;
    outputFile << "AM=M-1" << std::endl;
    outputFile << "D=M" << std::endl;

    outputFile << "@" << label << std::endl;
    outputFile << "!D" << std::endl;
    outputFile << "D;JNE" << std::endl;
}

void CodeWriter::writeFunction(const std::string &functionName,
                               const int nArgs) {
    outputFile << "// function " << functionName << " " << std::to_string(nArgs)
               << std::endl;

    outputFile << "(" << functionName << ")" << std::endl;
    for (int i = 0; i < nArgs; i++) {
        writePushPop(commandType::C_PUSH, "constant", 0);
    }
}

void CodeWriter::writeCall(const std::string &functionName, const int nArgs) {
    outputFile << "// call " << functionName << " " << std::to_string(nArgs)
               << std::endl;

    // push returnAddress
    outputFile << "@" << functionName << "$ret."
               << std::to_string(returnCounter) << std::endl;
    outputFile << "D=A" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "A=M" << std::endl;
    outputFile << "M=D" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "M=M+1" << std::endl;
    // push LCL
    outputFile << "@LCL" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "A=M" << std::endl;
    outputFile << "M=D" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "M=M+1" << std::endl;
    // push ARG
    outputFile << "@ARG" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "A=M" << std::endl;
    outputFile << "M=D" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "M=M+1" << std::endl;
    // push THIS
    outputFile << "@THIS" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "A=M" << std::endl;
    outputFile << "M=D" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "M=M+1" << std::endl;
    // push THAT
    outputFile << "@THAT" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "A=M" << std::endl;
    outputFile << "M=D" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "M=M+1" << std::endl;
    // ARG = SP - 5 - nArgs
    outputFile << "@SP" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@5" << std::endl;
    outputFile << "D=D-A" << std::endl;
    outputFile << "@" << std::to_string(nArgs) << std::endl;
    outputFile << "D=D-A" << std::endl;
    outputFile << "@ARG" << std::endl;
    outputFile << "M=D" << std::endl;
    // LCL = SP
    outputFile << "@SP" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@LCL" << std::endl;
    outputFile << "M=D" << std::endl;
    // goto f
    outputFile << "@" << functionName << std::endl;
    outputFile << "0;JMP" << std::endl;
    // (returnAddress)
    outputFile << "(" << functionName << "$ret."
               << std::to_string(returnCounter) << ")" << std::endl;
    returnCounter++;
}

void CodeWriter::writeReturn() {
    outputFile << "// return" << std::endl;

    // frame = LCL
    outputFile << "@LCL" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@R13" << std::endl;
    outputFile << "M=D" << std::endl;
    // retAddr = *(frame - 5)
    outputFile << "@5" << std::endl;
    outputFile << "A=D-A" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@R14" << std::endl;
    outputFile << "M=D" << std::endl;
    // *ARG = pop()
    outputFile << "@SP" << std::endl;
    outputFile << "AM=M-1" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@ARG" << std::endl;
    outputFile << "A=M" << std::endl;
    outputFile << "M=D" << std::endl;
    // SP = ARG + 1
    outputFile << "@ARG" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@SP" << std::endl;
    outputFile << "M=D+1" << std::endl;
    // THAT = *(frame - 1)
    outputFile << "@R13" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@1" << std::endl;
    outputFile << "A=D-A" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@THAT" << std::endl;
    outputFile << "M=D" << std::endl;
    // THIS = *(frame - 2)
    outputFile << "@R13" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@2" << std::endl;
    outputFile << "A=D-A" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@THIS" << std::endl;
    outputFile << "M=D" << std::endl;
    // ARG = *(frame - 3)
    outputFile << "@R13" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@3" << std::endl;
    outputFile << "A=D-A" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@ARG" << std::endl;
    outputFile << "M=D" << std::endl;
    // LCL = *(frame - 4)
    outputFile << "@R13" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@4" << std::endl;
    outputFile << "A=D-A" << std::endl;
    outputFile << "D=M" << std::endl;
    outputFile << "@LCL" << std::endl;
    outputFile << "M=D" << std::endl;
    // goto retAddr
    outputFile << "@R14" << std::endl;
    outputFile << "A=M" << std::endl;
    outputFile << "0;JMP" << std::endl;
}

void CodeWriter::writeEndLoop() {
    outputFile << "(END)" << std::endl;
    outputFile << "@END" << std::endl;
    outputFile << "0;JMP" << std::endl;
}

void CodeWriter::close() { outputFile.close(); }
