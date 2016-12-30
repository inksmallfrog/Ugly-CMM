#ifndef _H_CMM_ERRORHANDLER_
#define _H_CMM_ERRORHANDLER_

#include <list>
#include <iostream>

class TokenParser;

struct CMMError{
    int line;
    int column;
    std::string error_class;
    std::string filename;
    std::string descript;
    std::string maybe;
};

enum CMMErrorType{
    EMPTY_SOURCE_FILE = 0,
    END_BEFORE_EOF,
    EXPECTED_PROGRAM,

    MULTI_DECLARED,
    ARRAY_INDEX_NOT_CONST,
    UNDECLARED_VARIABLE,


    BLOCK_STATEMENT_ERROR,
    EXPECTED_EXPRESSION,
    EXPECTED_SEMICOLON,
    EXPECTED_VARIABLE,
    EXPECTED_VALUE,
    EXPECTED_AFTERBLOCKSTATEMENT,
    EXPECTED_ASSIGN,
    EQUAL_TO_ASSIGN,
    EXPECTED_TERM,
    EXPECTED_LEFTBRACKS,
    EXPECTED_RIGHTBRACKS,
    EXPECTED_LEFTBLOCK,
    EXPECTED_RIGHTBLOCK,
    EXPECTED_LEFTARRAY,
    EXPECTED_RIGHTARRAY,
};

enum CMMErrorClass{
    ERROR_TOKEN = 0,
    ERROR_GRAMMAR,
    ERROR_SEMANTICS,
    ERROR_CLASS_TOTAL
};



class ErrorHandler{
public:
    static ErrorHandler *GetInstance(){ if(instance == NULL) { instance = new ErrorHandler(); }; return instance; }
    static void DestroyErrorHandler(){ if(instance != NULL){ delete instance; instance = NULL; } }

    ~ErrorHandler();

    void AddError(CMMErrorType type, CMMErrorClass,  const TokenParser *parser);
    void AddError(CMMErrorType type, CMMErrorClass error_class, const std::string &filename, int line, int column, const std::string token_type);
    bool HasError(){ return !m_errorList.empty(); }
    const std::string GetErrors()const;

private:
    static ErrorHandler *instance;
    static std::string errorClassStr[ERROR_CLASS_TOTAL];

    ErrorHandler();

    std::list<CMMError> m_errorList;
};

#define ERROR_HANDLER ErrorHandler::GetInstance()

#endif
