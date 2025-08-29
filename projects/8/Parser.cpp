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
            } else if (currentCommand[0] == "label") {
                currentCommandType = CommandType::C_LABEL;
            } else if (currentCommand[0] == "goto") {
                currentCommandType = CommandType::C_GOTO;
            } else if (currentCommand[0] == "if-goto") {
                currentCommandType = CommandType::C_IF;
            } else if (currentCommand[0] == "function") {
                currentCommandType = CommandType::C_FUNCTION;
            } else if (currentCommand[0] == "call") {
                currentCommandType = CommandType::C_CALL;
            } else if (currentCommand[0] == "return") {
                currentCommandType = CommandType::C_RETURN;
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
    } else if (currentCommandType == CommandType::C_PUSH ||
               currentCommandType == CommandType::C_POP) {
        if (currentCommand.size() >= 2) {
            return currentCommand[1];
        }
    } else if (currentCommandType == CommandType::C_LABEL) {
        if (currentCommand.size() >= 2) {
            return currentCommand[1];
        }
    } else if (currentCommandType == CommandType::C_GOTO) {
        if (currentCommand.size() >= 2) {
            return currentCommand[1];
        }
    } else if (currentCommandType == CommandType::C_IF) {
        if (currentCommand.size() >= 2) {
            return currentCommand[1];
        }
    } else if (currentCommandType == CommandType::C_FUNCTION) {
        if (currentCommand.size() >= 2) {
            return currentCommand[1];
        }
    } else if (currentCommandType == CommandType::C_CALL) {
        if (currentCommand.size() >= 2) {
            return currentCommand[1];
        }
    }
    throw std::logic_error("arg1() called on command with no argument 1.");
}

int Parser::arg2() const {
    if (currentCommandType == CommandType::C_PUSH ||
        currentCommandType == CommandType::C_POP ||
        currentCommandType == CommandType::C_FUNCTION ||
        currentCommandType == CommandType::C_CALL) {
        if (currentCommand.size() >= 3) {
            return stoi(currentCommand[2]);
        }
    }
    throw std::logic_error("arg2() called on command with no argument 2.");
}
