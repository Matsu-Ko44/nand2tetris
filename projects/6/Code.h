#ifndef CODE_H
#define CODE_H

#include <string>

class Code {
   public:
    static std::string dest(const std::string& mnemonic);
    static std::string comp(const std::string& mnemonic);
    static std::string jump(const std::string& mnemonic);
};

#endif  // CODE_H
