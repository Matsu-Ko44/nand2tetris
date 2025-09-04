#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H

#include <fstream>
#include <string>

#include "JackTokenizer.h"

class CompilationEngine {
   public:
    explicit CompilationEngine(const std::string& inputFileName,
                               const std::string& outputFileName);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    int compileExpressionList();

   private:
    std::ifstream inputFile;
    std::ofstream outputFile;
    JackTokenizer tokenizer;
    int indentLevel = 0;
    void printCurrentTokenAsXML();
    void printIndent();

    static const std::unordered_map<Keyword, std::string> keywordToString;
};

#endif  // COMPILATIONENGINE_H
