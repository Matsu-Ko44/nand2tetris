#include "Parser.h"

#include <algorithm>
#include <stdexcept>

Parser::Parser(const std::string &filename) : inputFile(filename) {
    if (!inputFile.is_open()) {
        throw std::runtime_error("File cannot open: " + filename);
    }
}

std::string Parser::cleanLine(const std::string &line) {
    std::string cleaned = line;
    size_t commentPos = cleaned.find("//");
    if (commentPos != std::string::npos) {
        cleaned = cleaned.substr(0, commentPos);
    }

    cleaned.erase(0, cleaned.find_first_not_of(" \t\n\r\f\v"));

    cleaned.erase(cleaned.find_last_not_of(" \t\n\r\f\v") + 1);

    return cleaned;
}

bool Parser::hasMoreLines() {
    while (inputFile.peek() != EOF) {
        std::streampos originalPos = inputFile.tellg();
        std::string line;
        std::getline(inputFile, line);

        if (!cleanLine(line).empty()) {
            inputFile.seekg(originalPos);
            return true;
        }
    }
    return false;
}

void Parser::advance() {
    std::string line;
    while (std::getline(inputFile, line)) {
        std::string cleaned = cleanLine(line);
        if (!cleaned.empty()) {
            currentCommand = cleaned;

            if (currentCommand[0] == '@') {
                currentInstructionType = InstructionType::A_INSTRUCTION;
            } else if (currentCommand[0] == '(' &&
                       currentCommand.back() == ')') {
                currentInstructionType = InstructionType::L_INSTRUCTION;
            } else {
                currentInstructionType = InstructionType::C_INSTRUCTION;
            }
            return;
        }
    }
}

InstructionType Parser::instructionType() const {
    return currentInstructionType;
}

std::string Parser::symbol() const {
    if (currentInstructionType == InstructionType::A_INSTRUCTION) {
        return currentCommand.substr(1);
    }
    if (currentInstructionType == InstructionType::L_INSTRUCTION) {
        return currentCommand.substr(1, currentCommand.length() - 2);
    }
    return "";
}

std::string Parser::dest() const {
    if (currentInstructionType != InstructionType::C_INSTRUCTION) {
        return "";
    }

    size_t equalPos = currentCommand.find('=');
    if (equalPos != std::string::npos) {
        return currentCommand.substr(0, equalPos);
    }
    return "";
}

std::string Parser::comp() const {
    if (currentInstructionType != InstructionType::C_INSTRUCTION) {
        return "";
    }

    size_t equalPos = currentCommand.find('=');
    size_t semicolonPos = currentCommand.find(';');
    if (equalPos != std::string::npos) {
        std::string compPart = currentCommand.substr(equalPos + 1);
        if (semicolonPos != std::string::npos) {
            compPart = compPart.substr(0, compPart.find(';'));
        }
        return compPart;
    }

    if (semicolonPos != std::string::npos) {
        return currentCommand.substr(0, semicolonPos);
    }

    return "";
}

std::string Parser::jump() const {
    if (currentInstructionType != InstructionType::C_INSTRUCTION) {
        return "";
    }

    size_t semicolonPos = currentCommand.find(';');
    if (semicolonPos != std::string::npos) {
        return currentCommand.substr(semicolonPos + 1);
    }
    return "";
}
