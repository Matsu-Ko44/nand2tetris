#include "JackTokenizer.h"

#include <iostream>

const std::unordered_map<std::string, Keyword> JackTokenizer::keywordMap = {
    {"class", Keyword::CLASS},
    {"constructor", Keyword::CONSTRUCTOR},
    {"function", Keyword::FUNCTION},
    {"method", Keyword::METHOD},
    {"field", Keyword::FIELD},
    {"static", Keyword::STATIC},
    {"var", Keyword::VAR},
    {"int", Keyword::INT},
    {"char", Keyword::CHAR},
    {"boolean", Keyword::BOOLEAN},
    {"void", Keyword::VOID},
    {"true", Keyword::TRUE},
    {"false", Keyword::FALSE},
    {"null", Keyword::NULL_TYPE},
    {"this", Keyword::THIS},
    {"let", Keyword::LET},
    {"do", Keyword::DO},
    {"if", Keyword::IF},
    {"else", Keyword::ELSE},
    {"while", Keyword::WHILE},
    {"return", Keyword::RETURN},
};

const std::unordered_set<char> JackTokenizer::symbols = {
    '{', '}', '(', ')', '[', ']', '.', ',', ';', '+',
    '-', '*', '/', '&', '|', '<', '>', '=', '~',
};

JackTokenizer::JackTokenizer(const std::string& fileName)
    : inputFile(fileName) {
    if (!inputFile.is_open()) {
        throw std::runtime_error("File cannot open: " + fileName);
    }
    size_t dotPos = fileName.rfind(".jack");
    if (dotPos == std::string::npos) {
        throw std::runtime_error("Extension is not .jack");
    }

    removeComments(inputFile, this->commentFreeStream);
}

bool JackTokenizer::hasMoreTokens() {
    commentFreeStream >> std::ws;
    return commentFreeStream.peek() != EOF;
}

void JackTokenizer::advance() {
    if (!hasMoreTokens()) {
        throw std::runtime_error("No more tokens to advance.");
    }

    char ch = commentFreeStream.peek();

    if (ch == '"') {
        commentFreeStream.get();
        std::string val;
        while (commentFreeStream.get(ch) && ch != '"') {
            val += ch;
        }
        currentToken = val;
        currentTokenType = TokenType::STRING_CONST;
    } else if (symbols.count(ch)) {
        commentFreeStream.get(ch);
        currentToken = ch;
        currentTokenType = TokenType::SYMBOL;
    } else if (std::isdigit(ch)) {
        std::string val;
        while (commentFreeStream.get(ch) && std::isdigit(ch)) {
            val += ch;
        }
        commentFreeStream.unget();
        currentToken = val;
        currentTokenType = TokenType::INT_CONST;
    } else if (std::isalpha(ch) || ch == '_') {
        std::string val;
        while (commentFreeStream.get(ch) && (std::isalnum(ch) || ch == '_')) {
            val += ch;
        }
        commentFreeStream.unget();

        if (keywordMap.count(val)) {
            currentToken = val;
            currentTokenType = TokenType::KEYWORD;
            currentKeyword = keywordMap.at(currentToken);
        } else {
            currentToken = val;
            currentTokenType = TokenType::IDENTIFIER;
        }
    } else {
        throw std::runtime_error("Unknown token");
    }
}

TokenType JackTokenizer::tokenType() const { return currentTokenType; }

Keyword JackTokenizer::keyword() const {
    if (currentTokenType != TokenType::KEYWORD) {
        throw std::runtime_error("Current token type is not KEYWORD.");
    }
    return currentKeyword;
}

char JackTokenizer::symbol() const {
    if (currentTokenType != TokenType::SYMBOL) {
        throw std::runtime_error("Current token type is not SYMBOL.");
    }
    return currentToken[0];
}

std::string JackTokenizer::identifier() const {
    if (currentTokenType != TokenType::IDENTIFIER) {
        throw std::runtime_error("Current token type is not IDENTIFIER.");
    }
    return currentToken;
}

int JackTokenizer::intVal() const {
    if (currentTokenType != TokenType::INT_CONST) {
        throw std::runtime_error("Current token type is not INT_CONST.");
    }
    return std::stoi(currentToken);
}

std::string JackTokenizer::stringVal() const {
    if (currentTokenType != TokenType::STRING_CONST) {
        throw std::runtime_error("Current token type is not STRING_CONST.");
    }
    return currentToken;
}

void JackTokenizer::removeComments(std::istream& input, std::ostream& output) {
    CommentState currentState = CommentState::Normal;
    char ch;

    while (input.get(ch)) {
        switch (currentState) {
            case CommentState::Normal:
                if (ch == '/') {
                    currentState = CommentState::MightEnterComment;
                } else {
                    output.put(ch);
                }
                break;

            case CommentState::MightEnterComment:
                if (ch == '/') {
                    currentState = CommentState::InSingleLineComment;
                } else if (ch == '*') {
                    currentState = CommentState::InMultiLineComment;
                } else {
                    output.put('/');
                    output.put(ch);
                    currentState = CommentState::Normal;
                }
                break;

            case CommentState::InSingleLineComment:
                if (ch == '\n') {
                    output.put(ch);
                    currentState = CommentState::Normal;
                }
                break;

            case CommentState::InMultiLineComment:
                if (ch == '*') {
                    currentState = CommentState::MightExitComment;
                }
                break;

            case CommentState::MightExitComment:
                if (ch == '/') {
                    currentState = CommentState::Normal;
                } else if (ch != '*') {
                    currentState = CommentState::InMultiLineComment;
                }
                break;
        }
    }
}
