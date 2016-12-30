#ifndef CMMCORE_H
#define CMMCORE_H

#include "TokenParser.h"
#include "ErrorHandler.h"
#include "QuaternaryGenerator.h"

enum CMMCoreMode{
    IDLE,
    COMPILED
};

class CMMCore
{
public:
    CMMCore();
    virtual ~CMMCore();

    void CompileFile(std::string &filePath);

    bool HasError();
    const std::string GetErrors();
    const std::string GetQuaternaries();

    void RunResult();
    void RunFile(const std::string &filePath);

protected:
    void Start();

    bool Program();

    bool Sentence();
    bool Block(bool newTable = true);

    int BlockStatementFollow();

    bool IfFollow();
    bool Else();
    bool WhileFollow();

    bool AssignSentence();

    const IdInfo* Expression();
    const IdInfo* ExpressionFollow(const IdInfo* arg1);
    const IdInfo* Term();
    const IdInfo* TermFollow(const IdInfo* arg1);
    const IdInfo* Factor();

    const IdInfo* Variable();
    const IdInfo* ArrayStruct();

    bool EatTokenType(TokenType type);
    const std::string EatId();

    bool tokenInList(const TokenType typeList[]);
    void CheckSemicolon(const TokenType *restoreTypeList);
private:
    ErrorHandler *m_errorHandler;
    QuaternaryGenerator *m_qg;
    TokenParser *m_tokenParser;
    Token m_lastToken;
    CMMCoreMode m_mode;
};

#endif // CMMCORE_H
