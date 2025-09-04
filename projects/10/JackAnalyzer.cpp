#include <filesystem>
#include <iostream>
#include <vector>

#include "CompilationEngine.h"
#include "JackTokenizer.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "prompt> " << argv[0] << " {filename.jack | directory}"
                  << std::endl;
        return 1;
    }

    std::string inputPathStr = argv[1];
    std::string outputFileName = inputPathStr;
    std::vector<std::string> inputFileNames;
    fs::path inputPath(inputPathStr);
    fs::path baseDir;

    if (fs::is_directory(inputPath)) {
        baseDir = inputPath;
        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.is_regular_file() &&
                entry.path().extension() == ".jack") {
                inputFileNames.push_back(entry.path().string());
            }
        }
    } else if (fs::is_regular_file(inputPath) &&
               inputPath.extension() == ".jack") {
        baseDir = inputPath.parent_path();
        inputFileNames.push_back(inputPath.string());
    } else {
        std::cerr << "Error: Input path is not a valid .jack file or directory."
                  << std::endl;
        return 1;
    }

    if (inputFileNames.empty()) {
        std::cout << "No .jack files found to process." << std::endl;
        return 0;
    }

    fs::path outputDir = baseDir / "XMLOutput";
    try {
        if (!fs::exists(outputDir)) {
            fs::create_directory(outputDir);
        }

    } catch (const std::exception& e) {
        std::cerr << "Failed to create directory " << e.what() << std::endl;
        return 1;
    }

    for (const std::string& inputFileName : inputFileNames) {
        fs::path currentInputPath(inputFileName);
        std::string outputBaseName = currentInputPath.stem().string() + ".xml";
        fs::path outputFilePath = outputDir / outputBaseName;

        CompilationEngine compilationEngine(inputFileName,
                                            outputFilePath.string());

        compilationEngine.compileClass();

        // JackTokenizer tokenizer(inputFileName);

        // std::ofstream outputFile(outputFileName);

        //     outputFile << "<tokens>" << std::endl;
        //     while (tokenizer.hasMoreTokens()) {
        //         tokenizer.advance();

        //         TokenType tokenType = tokenizer.tokenType();
        //         std::string currentTokenStr;
        //         switch (tokenType) {
        //             case TokenType::KEYWORD: {
        //                 Keyword kw = tokenizer.keyword();
        //                 currentTokenStr = keywordToString.at(kw);
        //                 outputFile << "<keyword> " << currentTokenStr
        //                            << " </keyword>" << std::endl;
        //                 break;
        //             }

        //             case TokenType::SYMBOL: {
        //                 char sym = tokenizer.symbol();
        //                 if (sym == '<') {
        //                     currentTokenStr = "&lt;";
        //                 } else if (sym == '>') {
        //                     currentTokenStr = "&gt;";
        //                 } else if (sym == '&') {
        //                     currentTokenStr = "&amp;";
        //                 } else {
        //                     currentTokenStr = std::string(1, sym);
        //                 }
        //                 outputFile << "<symbol> " << currentTokenStr << "
        //                 </symbol>"
        //                            << std::endl;
        //                 break;
        //             }

        //             case TokenType::IDENTIFIER: {
        //                 currentTokenStr = tokenizer.identifier();
        //                 outputFile << "<identifier> " << currentTokenStr
        //                            << " </identifier>" << std::endl;
        //                 break;
        //             }

        //             case TokenType::INT_CONST: {
        //                 currentTokenStr = std::to_string(tokenizer.intVal());
        //                 outputFile << "<integerConstant> " << currentTokenStr
        //                            << " </integerConstant>" << std::endl;
        //                 break;
        //             }

        //             case TokenType::STRING_CONST: {
        //                 currentTokenStr = tokenizer.stringVal();
        //                 outputFile << "<stringConstant> " << currentTokenStr
        //                            << " </stringConstant>" << std::endl;
        //                 break;
        //             }

        //             default: {
        //                 throw std::runtime_error(
        //                     "Syntax error: Unknown token type");
        //             }
        //         }
        //     }
        //     outputFile << "</tokens>" << std::endl;
    }

    return 0;
}
