#ifndef VMTRANSLATOR_H
#define VMTRANSLATOR_H

#include <string>
#include <vector>

class VMTranslator {
   public:
    explicit VMTranslator(const std::string &fileName);
    void translate();

   private:
    std::string inputFilename;
    std::string outputFilename;
    std::vector<std::string> vmFiles;
    void writeBootStrap();
};

#endif  // VMTRANSLATOR_H
