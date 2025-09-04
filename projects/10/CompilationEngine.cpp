#include "CompilationEngine.h"

#include <unordered_map>

const std::unordered_map<Keyword, std::string>
    CompilationEngine::keywordToString = {
        {Keyword::CLASS, "class"},
        {Keyword::CONSTRUCTOR, "constructor"},
        {Keyword::FUNCTION, "function"},
        {Keyword::METHOD, "method"},
        {Keyword::FIELD, "field"},
        {Keyword::STATIC, "static"},
        {Keyword::VAR, "var"},
        {Keyword::INT, "int"},
        {Keyword::CHAR, "char"},
        {Keyword::BOOLEAN, "boolean"},
        {Keyword::VOID, "void"},
        {Keyword::TRUE, "true"},
        {Keyword::FALSE, "false"},
        {Keyword::NULL_TYPE, "null"},
        {Keyword::THIS, "this"},
        {Keyword::LET, "let"},
        {Keyword::DO, "do"},
        {Keyword::IF, "if"},
        {Keyword::ELSE, "else"},
        {Keyword::WHILE, "while"},
        {Keyword::RETURN, "return"},
};

CompilationEngine::CompilationEngine(const std::string &inputFileName,
                                     const std::string &outputFileName)
    : inputFile(inputFileName),
      outputFile(outputFileName),
      tokenizer(inputFileName) {
    if (!inputFile.is_open()) {
        throw std::runtime_error("File cannot open: " + inputFileName);
    }
    if (!outputFile.is_open()) {
        throw std::runtime_error("File cannot open: " + outputFileName);
    }
}

void CompilationEngine::compileClass() {
    printIndent();
    outputFile << "<class>" << std::endl;
    indentLevel++;

    tokenizer.advance();

    // 'class'
    printCurrentTokenAsXML();
    // className
    printCurrentTokenAsXML();
    // '{'
    printCurrentTokenAsXML();
    // classDec*
    while (tokenizer.tokenType() == TokenType::KEYWORD &&
           (tokenizer.keyword() == Keyword::STATIC ||
            tokenizer.keyword() == Keyword::FIELD)) {
        compileClassVarDec();
    }
    // subroutineDec*
    while (tokenizer.tokenType() == TokenType::KEYWORD &&
           (tokenizer.keyword() == Keyword::CONSTRUCTOR ||
            tokenizer.keyword() == Keyword::FUNCTION ||
            tokenizer.keyword() == Keyword::METHOD)) {
        compileSubroutine();
    }
    // '}'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</class>" << std::endl;
}

void CompilationEngine::compileClassVarDec() {
    printIndent();
    outputFile << "<classVarDec>" << std::endl;
    indentLevel++;

    // 'static'|'field'
    printCurrentTokenAsXML();
    // type
    printCurrentTokenAsXML();
    // varName
    printCurrentTokenAsXML();
    // (',' varName)*
    while (tokenizer.tokenType() == TokenType::SYMBOL &&
           tokenizer.symbol() == ',') {
        // ','
        printCurrentTokenAsXML();
        // varName
        printCurrentTokenAsXML();
    }
    // ';'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</classVarDec>" << std::endl;
};

void CompilationEngine::compileSubroutine() {
    printIndent();
    outputFile << "<subroutineDec>" << std::endl;
    indentLevel++;

    // 'constructor'|'function'|'method'
    printCurrentTokenAsXML();
    // 'void'|type
    printCurrentTokenAsXML();
    // subroutineName
    printCurrentTokenAsXML();
    // '('
    printCurrentTokenAsXML();
    // parameterList
    compileParameterList();
    // ')'
    printCurrentTokenAsXML();
    // subroutineBody
    compileSubroutineBody();

    indentLevel--;
    printIndent();
    outputFile << "</subroutineDec>" << std::endl;
};

void CompilationEngine::compileParameterList() {
    printIndent();
    outputFile << "<parameterList>" << std::endl;
    indentLevel++;

    if ((tokenizer.tokenType() == TokenType::KEYWORD &&
         (tokenizer.keyword() == Keyword::INT ||
          tokenizer.keyword() == Keyword::CHAR ||
          tokenizer.keyword() == Keyword::BOOLEAN)) ||
        tokenizer.tokenType() == TokenType::IDENTIFIER) {
        // type
        printCurrentTokenAsXML();
        // varName
        printCurrentTokenAsXML();
        // (',' type varName)*
        while (tokenizer.tokenType() == TokenType::SYMBOL &&
               tokenizer.symbol() == ',') {
            // ','
            printCurrentTokenAsXML();
            // type
            printCurrentTokenAsXML();
            // varName
            printCurrentTokenAsXML();
        }
    }

    indentLevel--;
    printIndent();
    outputFile << "</parameterList>" << std::endl;
};

void CompilationEngine::compileSubroutineBody() {
    printIndent();
    outputFile << "<subroutineBody>" << std::endl;
    indentLevel++;

    // '{'
    printCurrentTokenAsXML();
    // varDec*
    while (tokenizer.tokenType() == TokenType::KEYWORD &&
           tokenizer.keyword() == Keyword::VAR) {
        compileVarDec();
    }
    // statements
    compileStatements();
    // '}'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</subroutineBody>" << std::endl;
};

void CompilationEngine::compileVarDec() {
    printIndent();
    outputFile << "<varDec>" << std::endl;
    indentLevel++;

    // 'var'
    printCurrentTokenAsXML();
    // type
    printCurrentTokenAsXML();
    // varName
    printCurrentTokenAsXML();
    // (',' varName)*
    while (tokenizer.tokenType() == TokenType::SYMBOL &&
           tokenizer.symbol() == ',') {
        // ','
        printCurrentTokenAsXML();
        // varName
        printCurrentTokenAsXML();
    }
    // ';'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</varDec>" << std::endl;
};

void CompilationEngine::compileStatements() {
    printIndent();
    outputFile << "<statements>" << std::endl;
    indentLevel++;

    // statement*
    while (tokenizer.tokenType() == TokenType::KEYWORD &&
           (tokenizer.keyword() == Keyword::LET ||
            tokenizer.keyword() == Keyword::IF ||
            tokenizer.keyword() == Keyword::WHILE ||
            tokenizer.keyword() == Keyword::DO ||
            tokenizer.keyword() == Keyword::RETURN)) {
        if (tokenizer.keyword() == Keyword::LET) {
            // letStatement
            compileLet();
        } else if (tokenizer.keyword() == Keyword::IF) {
            // ifStatement
            compileIf();
        } else if (tokenizer.keyword() == Keyword::WHILE) {
            // whileStatement
            compileWhile();
        } else if (tokenizer.keyword() == Keyword::DO) {
            // doStatement
            compileDo();
        } else if (tokenizer.keyword() == Keyword::RETURN) {
            // returnStatement
            compileReturn();
        } else {
            throw std::runtime_error("Syntax error: Unknown keyword");
        }
    }

    indentLevel--;
    printIndent();
    outputFile << "</statements>" << std::endl;
};

void CompilationEngine::compileLet() {
    printIndent();
    outputFile << "<letStatement>" << std::endl;
    indentLevel++;

    // 'let'
    printCurrentTokenAsXML();
    // varName
    printCurrentTokenAsXML();
    // ('[' expression ']')?
    if (tokenizer.tokenType() == TokenType::SYMBOL &&
        tokenizer.symbol() == '[') {
        // '['
        printCurrentTokenAsXML();
        // expression
        compileExpression();
        // ']'
        printCurrentTokenAsXML();
    }
    // '='
    printCurrentTokenAsXML();
    // expression
    compileExpression();
    // ';'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</letStatement>" << std::endl;
};

void CompilationEngine::compileIf() {
    printIndent();
    outputFile << "<ifStatement>" << std::endl;
    indentLevel++;

    // 'if'
    printCurrentTokenAsXML();
    // '('
    printCurrentTokenAsXML();
    // expression
    compileExpression();
    // ')'
    printCurrentTokenAsXML();
    // '{'
    printCurrentTokenAsXML();
    // statements
    compileStatements();
    // '}'
    printCurrentTokenAsXML();
    // ('else' '{' statements '}')?
    if (tokenizer.tokenType() == TokenType::KEYWORD &&
        tokenizer.keyword() == Keyword::ELSE) {
        // 'else'
        printCurrentTokenAsXML();
        // '{'
        printCurrentTokenAsXML();
        // statements
        compileStatements();
        // '}'
        printCurrentTokenAsXML();
    }

    indentLevel--;
    printIndent();
    outputFile << "</ifStatement>" << std::endl;
};

void CompilationEngine::compileWhile() {
    printIndent();
    outputFile << "<whileStatement>" << std::endl;
    indentLevel++;

    // 'while'
    printCurrentTokenAsXML();
    // '('
    printCurrentTokenAsXML();
    // expression
    compileExpression();
    // ')'
    printCurrentTokenAsXML();
    // '{'
    printCurrentTokenAsXML();
    // statements
    compileStatements();
    // '}'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</whileStatement>" << std::endl;
};

void CompilationEngine::compileDo() {
    printIndent();
    outputFile << "<doStatement>" << std::endl;
    indentLevel++;

    // 'do'
    printCurrentTokenAsXML();
    // subroutineCall
    // subroutineName
    printCurrentTokenAsXML();
    if (tokenizer.tokenType() == TokenType::SYMBOL &&
        tokenizer.symbol() == '.') {
        // '.'
        printCurrentTokenAsXML();
        // subroutineName
        printCurrentTokenAsXML();
    }
    // '('
    printCurrentTokenAsXML();
    // expressionList
    compileExpressionList();
    // ')'
    printCurrentTokenAsXML();
    // ';'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</doStatement>" << std::endl;
};

void CompilationEngine::compileReturn() {
    printIndent();
    outputFile << "<returnStatement>" << std::endl;
    indentLevel++;

    // 'return'
    printCurrentTokenAsXML();
    if (tokenizer.tokenType() != TokenType::SYMBOL ||
        (tokenizer.tokenType() == TokenType::SYMBOL &&
         (tokenizer.symbol() == '(' || tokenizer.symbol() == '-' ||
          tokenizer.symbol() == '~'))) {
        // expression
        compileExpression();
    }
    // ';'
    printCurrentTokenAsXML();

    indentLevel--;
    printIndent();
    outputFile << "</returnStatement>" << std::endl;
};

void CompilationEngine::compileExpression() {
    printIndent();
    outputFile << "<expression>" << std::endl;
    indentLevel++;

    // term
    compileTerm();
    // (op term)*
    while (tokenizer.tokenType() == TokenType::SYMBOL &&
               tokenizer.symbol() == '+' ||
           tokenizer.symbol() == '-' || tokenizer.symbol() == '*' ||
           tokenizer.symbol() == '/' || tokenizer.symbol() == '&' ||
           tokenizer.symbol() == '|' || tokenizer.symbol() == '<' ||
           tokenizer.symbol() == '>' || tokenizer.symbol() == '=') {
        // op
        printCurrentTokenAsXML();
        // term
        compileTerm();
    }

    indentLevel--;
    printIndent();
    outputFile << "</expression>" << std::endl;
};

void CompilationEngine::compileTerm() {
    printIndent();
    outputFile << "<term>" << std::endl;
    indentLevel++;

    if (tokenizer.tokenType() == TokenType::KEYWORD ||
        tokenizer.tokenType() == TokenType::IDENTIFIER ||
        tokenizer.tokenType() == TokenType::INT_CONST ||
        tokenizer.tokenType() == TokenType::STRING_CONST) {
        // integerConstant|stringConstant|keywordConstant|varName
        printCurrentTokenAsXML();
        if (tokenizer.tokenType() == TokenType::SYMBOL &&
            tokenizer.symbol() == '[') {
            // '['
            printCurrentTokenAsXML();
            // expression
            compileExpression();
            // ']'
            printCurrentTokenAsXML();
        } else if (tokenizer.tokenType() == TokenType::SYMBOL &&
                   tokenizer.symbol() == '.') {
            // '.'
            printCurrentTokenAsXML();
            // subroutineName
            printCurrentTokenAsXML();
            // '('
            printCurrentTokenAsXML();
            // expressionList
            compileExpressionList();
            // ')'
            printCurrentTokenAsXML();
        } else if (tokenizer.tokenType() == TokenType::SYMBOL &&
                   tokenizer.symbol() == '(') {
            // '('
            printCurrentTokenAsXML();
            // expressionList
            compileExpressionList();
            // ')'
            printCurrentTokenAsXML();
        }
    } else if (tokenizer.tokenType() == TokenType::SYMBOL &&
               tokenizer.symbol() == '(') {
        // '('
        printCurrentTokenAsXML();
        // expression
        compileExpression();
        // ')'
        printCurrentTokenAsXML();
    } else if (tokenizer.tokenType() == TokenType::SYMBOL &&
               (tokenizer.symbol() == '-' || tokenizer.symbol() == '~')) {
        // unaryOp
        printCurrentTokenAsXML();
        // term
        compileTerm();
    }

    indentLevel--;
    printIndent();
    outputFile << "</term>" << std::endl;
};

int CompilationEngine::compileExpressionList() {
    int expressionCount = 0;

    printIndent();
    outputFile << "<expressionList>" << std::endl;
    indentLevel++;

    if (tokenizer.tokenType() != TokenType::SYMBOL ||
        (tokenizer.tokenType() == TokenType::SYMBOL &&
         (tokenizer.symbol() == '(' || tokenizer.symbol() == '-' ||
          tokenizer.symbol() == '~'))) {
        // expression
        compileExpression();
        expressionCount++;
        // (',' expression)*
        while (tokenizer.tokenType() == TokenType::SYMBOL &&
               tokenizer.symbol() == ',') {
            // ','
            printCurrentTokenAsXML();
            // expression
            compileExpression();
            expressionCount++;
        }
    }

    indentLevel--;
    printIndent();
    outputFile << "</expressionList>" << std::endl;
    return expressionCount;
};

void CompilationEngine::printCurrentTokenAsXML() {
    std::string currentTokenStr;
    switch (tokenizer.tokenType()) {
        case TokenType::KEYWORD: {
            Keyword kw = tokenizer.keyword();
            currentTokenStr = keywordToString.at(kw);
            printIndent();
            outputFile << "<keyword> " << currentTokenStr << " </keyword>"
                       << std::endl;
            break;
        }

        case TokenType::SYMBOL: {
            char sym = tokenizer.symbol();
            if (sym == '<') {
                currentTokenStr = "&lt;";
            } else if (sym == '>') {
                currentTokenStr = "&gt;";
            } else if (sym == '&') {
                currentTokenStr = "&amp;";
            } else {
                currentTokenStr = std::string(1, sym);
            }
            printIndent();
            outputFile << "<symbol> " << currentTokenStr << " </symbol>"
                       << std::endl;
            break;
        }

        case TokenType::IDENTIFIER: {
            currentTokenStr = tokenizer.identifier();
            printIndent();
            outputFile << "<identifier> " << currentTokenStr << " </identifier>"
                       << std::endl;
            break;
        }

        case TokenType::INT_CONST: {
            currentTokenStr = std::to_string(tokenizer.intVal());
            printIndent();
            outputFile << "<integerConstant> " << currentTokenStr
                       << " </integerConstant>" << std::endl;
            break;
        }

        case TokenType::STRING_CONST: {
            currentTokenStr = tokenizer.stringVal();
            printIndent();
            outputFile << "<stringConstant> " << currentTokenStr
                       << " </stringConstant>" << std::endl;
            break;
        }

        default:
            throw std::runtime_error("Syntax error: Unknown token type");
    }

    if (tokenizer.hasMoreTokens()) {
        tokenizer.advance();
    }
}

void CompilationEngine::printIndent() {
    for (int i = 0; i < indentLevel; i++) {
        outputFile << '\t';
    }
}
