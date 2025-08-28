#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <fstream>
#include <string>
#include <unordered_map>

#include "Parser.h"

class CodeWriter {
   public:
    explicit CodeWriter(const std::string &fileName);
    void setFileName(const std::string &fileName);
    void writeArithmetic(const std::string &command);
    void writePushPop(const commandType command, const std::string &segment,
                      const int index);
    void writeLabel(const std::string &label);
    void writeGoto(const std::string &label);
    void writeIf(const std::string &label);
    void writeFunction(const std::string &functionName, const int nArgs);
    void writeCall(const std::string &functionName, const int nArgs);
    void writeReturn();
    void writeEndLoop();
    void close();

   private:
    std::string vmFilename;
    std::ofstream outputFile;
    int jumpCounter = 0;
    int staticCounter = 0;
    int returnCounter = 0;
};

#endif  // CODEWRITER_H
