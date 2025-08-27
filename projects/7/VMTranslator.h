#ifndef VMTRANSLATOR_H
#define VMTRANSLATOR_H

#include <string>

class VMTranslator {
   public:
    explicit VMTranslator(const std::string &fileName);
    void translate();

   private:
    std::string inputFilename;
    std::string outputFilename;
};

#endif  // VMTRANSLATOR_H
