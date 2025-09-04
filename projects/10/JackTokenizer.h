#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

enum class TokenType {
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST,
};

enum class Keyword {
    CLASS,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    NULL_TYPE,
    THIS
};

enum class CommentState {
    Normal,
    InSingleLineComment,
    InMultiLineComment,
    MightEnterComment,
    MightExitComment,
};

class JackTokenizer {
   public:
    explicit JackTokenizer(const std::string& fileName);
    bool hasMoreTokens();
    void advance();
    TokenType tokenType() const;
    Keyword keyword() const;
    char symbol() const;
    std::string identifier() const;
    int intVal() const;
    std::string stringVal() const;

   private:
    std::ifstream inputFile;
    std::stringstream commentFreeStream;
    std::string currentToken;
    TokenType currentTokenType;
    Keyword currentKeyword;
    void removeComments(std::istream& input, std::ostream& output);

    static const std::unordered_map<std::string, Keyword> keywordMap;
    static const std::unordered_set<char> symbols;
};

#endif  // JACKTOKENIZER_H
