#include "ErrorHandler.h"
#include "TokenParser.h"
#include <sstream>

ErrorHandler* ErrorHandler::instance = new ErrorHandler();
std::string ErrorHandler::errorClassStr[ERROR_CLASS_TOTAL] = {
    "Token",
    "Grammar",
    "Semantics"
};

ErrorHandler::ErrorHandler(){

}

ErrorHandler::~ErrorHandler(){

}

void ErrorHandler::AddError(CMMErrorType type, CMMErrorClass error_class, const TokenParser *parser){
    AddError(type, error_class, parser->GetFilename(), parser->GetLine(), parser->GetColumn(), TokenParser::TranslateType(parser->GetToken().type));
}

void ErrorHandler::AddError(CMMErrorType type, CMMErrorClass error_class, const std::string &filename, int line, int column, const std::string token_type){
    CMMError error;
    error.error_class = errorClassStr[error_class];
    error.filename = filename;
    error.line = line;
    error.column = column;
    switch(type){
        case EMPTY_SOURCE_FILE:
            error.line = 0;
            error.column = 0;
            error.descript = "Target file: " + error.filename + " is empty!";
            error.maybe = "Wrong filename?";
            break;

        case END_BEFORE_EOF:
            error.descript = "Illegle characters after program!";
            error.maybe = "Forget '{' '}' somewhere";
            break;

        case EXPECTED_PROGRAM:
            error.descript = "Expected '{' 'if' 'while' 'read' 'write' 'int' 'real' id, Got " +
                    token_type;
            error.maybe = "Code Error";
            break;

        case MULTI_DECLARED:
            error.descript = "Find multi-declared id! ";
            error.maybe = "multi-declared";
            break;

        case ARRAY_INDEX_NOT_CONST:
            error.descript = "Array defined size not a const int number! ";
            error.maybe = "Code Error";
            break;

        case UNDECLARED_VARIABLE:
            error.descript = "Undeclared variable! ";
            error.maybe = "Forgot declare it?";
            break;

        case BLOCK_STATEMENT_ERROR:
            error.descript = "Wrong block statement code!";
            error.maybe = "Code Wrong!";
            break;

        case EXPECTED_EXPRESSION:
            error.descript = "Expected '-' '(' number id, Got " +
                    token_type;
            error.maybe = "Code Wrong!";
            break;

        case EXPECTED_SEMICOLON:
            error.descript = "Expected ';', Got " +
                    token_type;
            error.maybe = "Forget ';'?";
            break;

        case EXPECTED_VARIABLE:
            error.descript = "Expected Id, Got " +
                    token_type;
            error.maybe = "Code Wrong!";
            break;

        case EXPECTED_VALUE:
            error.descript = "Expected Id | Number | '(' | '-' , Got " +
                    token_type;
            error.maybe = "Code Wrong!";
            break;

        case EXPECTED_AFTERBLOCKSTATEMENT:
            error.descript = "Expected ';' or '{' or sentence, Got " +
                    token_type;
            error.maybe = "Code Wrong!";
            break;

        case EXPECTED_ASSIGN:
            error.descript = "Expected '=' Got " +
                    token_type;
            error.maybe = "Forget '='?";
            break;

        case EQUAL_TO_ASSIGN:
            error.descript = "Expected '=' Got " +
                    token_type;
            error.maybe = "Do you mean '='?";
            break;

        case EXPECTED_TERM:
            error.descript = "Expected number id '(' , Got " +
                    token_type;
            error.maybe = "Code Wrong!";
            break;

        case EXPECTED_LEFTBRACKS:
            error.descript = "Expected '(' , Got " +
                    token_type;
            error.maybe = "Forget '('?";
            break;

        case EXPECTED_RIGHTBRACKS:
            error.descript = "Expected ')' , Got " +
                    token_type;
            error.maybe = "Forget ')'?";
            break;

        case EXPECTED_LEFTBLOCK:
            error.descript = "Expected '{' , Got " +
                    token_type;
            error.maybe = "Forget '{'?";
            break;

        case EXPECTED_RIGHTBLOCK:
            error.descript = "Expected '}' , Got " +
                    token_type;
            error.maybe = "Forget '}'?";
            break;

        case EXPECTED_LEFTARRAY:
            error.descript = "Expected '[' , Got " +
                    token_type;
            error.maybe = "Forget '['?";
            break;

        case EXPECTED_RIGHTARRAY:
            error.descript = "Expected ']' , Got " +
                    token_type;
            error.maybe = "Forget ']'?";
            break;
    }
    m_errorList.push_back(error);
}

const std::string ErrorHandler::GetErrors()const{
    std::ostringstream ost;
    for(std::list<CMMError>::const_iterator it = m_errorList.begin(); it != m_errorList.end(); ++it){
        ost << "Error: " << std::endl
            << "\tClass: " << ((CMMError)*it).error_class << std::endl
            << "\tFile: " << ((CMMError)*it).filename << std::endl
            << "\tLine: " << ((CMMError)*it).line << " Column: " << ((CMMError)*it).column << std::endl
            << "\tDescript: " << ((CMMError)*it).descript << std::endl
            << "\tMaybe: " << ((CMMError)*it).maybe << std::endl;
    }
    return ost.str();
}

