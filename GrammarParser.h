#ifndef _H_CMM_GRAMMARPARSER_
#define _H_CMM_GRAMMARPARSER_


#include <vector>
#include <iostream>
#include "TokenParser.h"

struct Quaternary;
struct IdInfo;
class ErrorHandler;


class GrammarParser{
public:
    GrammarParser(char *fileName);
    ~GrammarParser();

    void Start();
    const std::string GetResult();

protected:
    bool Program();

    bool Sentence();
    bool Block();

    //Return start line
    //0 for error
    bool BlockStatementFollow();

    bool IfFollow();
    bool Else();
    bool WhileFollow();

    bool AssignSentence();

    //Return result variable id
    //0 for error
    bool Expression();
    bool ExpressionFollow();
    bool Term();
    bool TermFollow();
    bool Factor();

    bool Variable();
    int ArrayStruct();
    //bool FunctionCallFollow();
    //bool FunctionParamsList();

    //bool FunctionDef();
    //bool FunctionParamsDefList();


    bool EatTokenType(TokenType type);
    const std::string EatId();

    bool tokenInList(const TokenType typeList[]);

private:
    ErrorHandler *m_errorHandler;
    TokenParser *m_tokenParser;
    Token m_lastToken;
    std::vector<std::string> m_parsedTree;
};

#endif
