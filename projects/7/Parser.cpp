#include "Parser.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

Parser::Parser(const std::string &fileName) : inputFile(fileName) {
    if (!inputFile.is_open()) {
        throw std::runtime_error("File cannot open: " + fileName);
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
            std::stringstream ss(cleaned);
            std::string cmd;

            currentCommand.clear();
            while (ss >> cmd) {
                currentCommand.push_back(cmd);
            }

            if (currentCommand[0] == "add" || currentCommand[0] == "sub" ||
                currentCommand[0] == "neg" || currentCommand[0] == "eq" ||
                currentCommand[0] == "gt" || currentCommand[0] == "lt" ||
                currentCommand[0] == "and" || currentCommand[0] == "or" ||
                currentCommand[0] == "not") {
                currentCommandType = CommandType::C_ARITHMETIC;
            } else if (currentCommand[0] == "push") {
                currentCommandType = CommandType::C_PUSH;
            } else if (currentCommand[0] == "pop") {
                currentCommandType = CommandType::C_POP;
            } else {
                throw std::runtime_error("Unknown command: " + cmd);
            }
            return;
        }
    }
}

CommandType Parser::getCommandType() const { return currentCommandType; }

std::string Parser::arg1() const {
    if (currentCommandType == CommandType::C_ARITHMETIC) {
        return currentCommand[0];
    }
    if (currentCommandType == CommandType::C_PUSH ||
        currentCommandType == CommandType::C_POP) {
        if (currentCommand.size() >= 2) {
            return currentCommand[1];
        }
    }
    throw std::logic_error("arg1() called on command with no argument 1.");
}

int Parser::arg2() const {
    if (currentCommandType == CommandType::C_PUSH ||
        currentCommandType == CommandType::C_POP) {
        if (currentCommand.size() >= 3) {
            return stoi(currentCommand[2]);
        }
    }
    throw std::logic_error("arg2() called on command with no argument 2.");
}
