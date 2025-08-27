#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>

enum class InstructionType { A_INSTRUCTION, C_INSTRUCTION, L_INSTRUCTION };

class Parser {
   public:
    explicit Parser(const std::string &fileName);
    bool hasMoreLines();
    void advance();
    InstructionType instructionType() const;
    std::string symbol() const;
    std::string dest() const;
    std::string comp() const;
    std::string jump() const;

   private:
    std::ifstream inputFile;
    std::string currentCommand;
    InstructionType currentInstructionType;
    std::string cleanLine(const std::string &line);
};

#endif  // PARSER_H
