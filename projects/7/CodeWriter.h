#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <fstream>
#include <string>
#include <unordered_map>

#include "Parser.h"

class CodeWriter {
   public:
    explicit CodeWriter(const std::string &fileName);
    void writeArithmetic(const std::string &command);
    void writePushPop(const CommandType command, const std::string &segment,
                      const int index);
    void writeEndLoop();
    void close();

   private:
    std::string vmFileName;
    std::ofstream outputFile;
    int jumpCounter = 0;
    int staticCounter = 0;
};

#endif  // CODEWRITER_H
