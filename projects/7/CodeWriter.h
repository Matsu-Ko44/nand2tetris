#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <fstream>
#include <string>
#include <unordered_map>

#include "Parser.h"

class CodeWriter {
   public:
    explicit CodeWriter(const std::string &filename);
    void writeArithmetic(const std::string &command);
    void writePushPop(const commandType command, const std::string &segment,
                      const std::string &index);
    void writeEndLoop();
    void close();

   private:
    std::string vmFilename;
    std::ofstream outputFile;
    int jumpCounter = 0;
    int staticCounter = 0;
};

#endif  // CODEWRITER_H
