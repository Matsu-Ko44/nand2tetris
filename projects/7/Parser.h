#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>
#include <vector>

enum class CommandType {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

class Parser {
   public:
    explicit Parser(const std::string &fileName);
    bool hasMoreLines();
    void advance();
    CommandType getCommandType() const;
    std::string arg1() const;
    int arg2() const;

   private:
    std::ifstream inputFile;
    std::vector<std::string> currentCommand;
    CommandType currentCommandType;
    std::string cleanLine(const std::string &line);
};

#endif  // PARSER_H
