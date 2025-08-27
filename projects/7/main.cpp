#include <iostream>

#include "VMTranslator.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "prompt> " << argv[0] << " filename.vm" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];
    VMTranslator vmTranslator(fileName);

    vmTranslator.translate();

    return 0;
}
