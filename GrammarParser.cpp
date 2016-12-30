#include "TokenParser.h"
#include "GrammarParser.h"
#include "ErrorHandler.h"
#include "QuaternaryGenerator.h"
#include "util.h"
#include <assert.h>

static const TokenType program_start[] = {
    LEFT_BLOCK,
    IF,
    WHILE,
    READ,
    WRITE,
    INT,
    REAL,
    ID
};
static const TokenType program_postfix[] = {
    RIGHT_BLOCK,
    ELSE,
    LEFT_BLOCK,
    IF,
    WHILE,
    READ,
    WRITE,
    INT,
    REAL,
    ID
};

static const TokenType *sentence_postfix = program_postfix;
static const TokenType *assignSen_postfix = program_postfix;

static const TokenType Expression_start[] = {
    MINUS,
    NUMBER_INT,
    NUMBER_REAL,
    ID,
    LEFT_BRACKS
};

static const TokenType Expression_postfix[] = {
    SEMICOLON,
    RIGHT_BRACKS
};

static const TokenType *BSF_start = program_start;


static const TokenType *BSF_postfix = program_postfix;

static const TokenType IFF_start[] = {
    LEFT_BRACKS
};

static const TokenType *IFF_postfix = program_postfix;

static const TokenType Term_postfix[] = {
    PLUS,
    MINUS
};

static const TokenType Term_start[] = {
    MULTI,
    DIVIDES,
    EQUAL,
    UNEQUAL,
    LESS,
    LESSEQUAL,
    BIGER,
    BIGEREQUAL
};

static const TokenType Variable_postfix[] = {
    ASSIGN,
    PLUS,
    MINUS,
    MULTI,
    DIVIDES,
    EQUAL,
    UNEQUAL,
    LESS,
    LESSEQUAL,
    BIGER,
    BIGEREQUAL
};

static const TokenType Variable_start[] = {
    ID
};

static const TokenType *AS_postfix = Variable_postfix;

static const TokenType AS_start[] = {
    LEFT_BLOCK
};

GrammarParser::GrammarParser(char *fileName){
    m_tokenParser = new TokenParser(fileName);
    m_errorHandler = ErrorHandler::GetInstance();
}

GrammarParser::~GrammarParser(){
    DELETE(m_tokenParser)
    ErrorHandler::DestroyErrorHandler();
}

void GrammarParser::Start(){
    if(m_tokenParser->ReadNext()){
        Program();
        m_parsedTree.push_back("End");
        if(!m_tokenParser->IsEOF()){
            //Expected EOF
        }
    }
    else{
        //m_errorHandler->AddError(EMPTY_SOURCE_FILE, ERROR_GRAMMAR, m_tokenParser);
    }
}

const std::string GrammarParser::GetResult(){
    std::vector<std::string>::const_iterator it = m_parsedTree.begin();
    std::string res;
    int layer = 0;
    for(;it < m_parsedTree.end(); ++it){
        if(*it != "End" && (*it).substr(0, 5) != "Error"){
            ++layer;
            for(int i = 0; i < layer; ++i){
                res.append("  ");
            }
            res += *it;
            res.append("\n");
        }
        else if(*it == "End"){
            --layer;
        }
        else{
            res.append("!");
            res += *it;
            res.append("\n");
        }


    }
    return res;
}

bool GrammarParser::Program(){
    m_parsedTree.push_back("Program");
    if(Block() || Sentence()){
        m_parsedTree.push_back("End");
        return true;
    }
    m_parsedTree.push_back("Error Program");
    while(!m_tokenParser->IsEOF()){
        //Expected Program Start
        if(tokenInList(program_postfix)){
            return true;
        }
        else if(tokenInList(program_start)){
            return Program();
        }
        else{
            EatTokenType(m_tokenParser->GetToken().type);
        }
    }
    return false;
}

bool GrammarParser::Sentence(){
    m_parsedTree.push_back("Sentence");
    if(EatTokenType(IF)){
        IfFollow();
        m_parsedTree.push_back("End");
        return true;
    }
    else if(EatTokenType(WHILE)){
        WhileFollow();
        m_parsedTree.push_back("End");
        return true;
    }
    else if(EatTokenType(READ)){
        Expression();
        m_parsedTree.push_back("End");
        if(!EatTokenType(SEMICOLON)){
            m_parsedTree.push_back("Error Sentence");
            while(!m_tokenParser->IsEOF()){
                if(EatTokenType(SEMICOLON) || tokenInList(sentence_postfix)){
                    return true;
                }
                else{
                    EatTokenType(m_tokenParser->GetToken().type);
                }
            }
        }
        return true;
    }
    else if(EatTokenType(WRITE)){
        Variable();
        m_parsedTree.push_back("End");
        if(!EatTokenType(SEMICOLON)){
            m_parsedTree.push_back("Error Sentence");
            while(!m_tokenParser->IsEOF()){
                if(EatTokenType(SEMICOLON) || tokenInList(sentence_postfix)){
                    return true;
                }
                else{
                    EatTokenType(m_tokenParser->GetToken().type);
                }
            }
        }
        return true;
    }
    else if(EatTokenType(INT) || EatTokenType(REAL)){
        Variable();
        m_parsedTree.push_back("End");
        if(EatTokenType(ASSIGN)){
            Expression();
            if(!EatTokenType(SEMICOLON)){
                m_parsedTree.push_back("Error Sentence");
                while(!m_tokenParser->IsEOF()){
                    if(EatTokenType(SEMICOLON) || tokenInList(sentence_postfix)){
                        return true;
                    }
                    else{
                        EatTokenType(m_tokenParser->GetToken().type);
                    }
                }
            }
            return true;
        }
        if(!EatTokenType(SEMICOLON)){
            m_parsedTree.push_back("Error Sentence");
            while(!m_tokenParser->IsEOF()){
                if(EatTokenType(SEMICOLON) || tokenInList(sentence_postfix)){
                    return true;
                }
                else{
                    EatTokenType(m_tokenParser->GetToken().type);
                }
            }
        }
        else{
            return true;
        }
    }
    else{
        AssignSentence();
        m_parsedTree.push_back("End");
        while(!m_tokenParser->IsEOF()){
            if(EatTokenType(SEMICOLON) || tokenInList(sentence_postfix)){
                return true;
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
        return true;
    }
    return false;
}

bool GrammarParser::Block(){
    if(EatTokenType(LEFT_BLOCK)){
        m_parsedTree.push_back("Block");
        while(!m_tokenParser->IsEOF() && !EatTokenType(RIGHT_BLOCK)){
            Program();
        }
        return true;
    }
    return false;
}

bool GrammarParser::BlockStatementFollow(){
    m_parsedTree.push_back("BSF");
    if(EatTokenType(SEMICOLON)){
        return true;
    }
    else if(!Program()){
        m_parsedTree.push_back("Error BSF");
        while(!m_tokenParser->IsEOF()){
            //Expected Program Start
            if(tokenInList(BSF_postfix)){
                return true;
            }
            else if(tokenInList(BSF_start)){
                return BlockStatementFollow();
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
    }
    return false;
}

bool GrammarParser::IfFollow(){
    if(!EatTokenType(LEFT_BRACKS)){
        m_parsedTree.push_back("Error IFF");
        while(!m_tokenParser->IsEOF()){
            //Expected Program Start
            if(tokenInList(IFF_postfix)){
                return true;
            }
            else if(tokenInList(IFF_start)){
                return IfFollow();
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
        return true;
    }
    m_parsedTree.push_back("IFF");
    Expression();
    m_parsedTree.push_back("End");
    if(!EatTokenType(RIGHT_BRACKS)){
        m_parsedTree.push_back("Error IFF");
        while(!m_tokenParser->IsEOF()){
            if(EatTokenType(RIGHT_BRACKS) || tokenInList(BSF_start)){
                break;
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
    }
    BlockStatementFollow();
    m_parsedTree.push_back("End");
    Else();
    m_parsedTree.push_back("End");
    return true;
}

bool GrammarParser::Else(){
    if(EatTokenType(ELSE)){
        m_parsedTree.push_back("Else");
        if(EatTokenType(IF)){
            IfFollow();
            m_parsedTree.push_back("End");
            return true;
        }
        BlockStatementFollow();
        m_parsedTree.push_back("End");
        return true;
    }
    return false;
}

bool GrammarParser::WhileFollow(){
    if(!EatTokenType(LEFT_BRACKS)){
        if(!EatTokenType(LEFT_BRACKS)){
            m_parsedTree.push_back("Error WF");
            while(!m_tokenParser->IsEOF()){
                //Expected Program Start
                if(tokenInList(IFF_postfix)){
                    return true;
                }
                else if(tokenInList(IFF_start)){
                    return WhileFollow();
                }
                else{
                    EatTokenType(m_tokenParser->GetToken().type);
                }
            }
            return true;
        }
    }
    m_parsedTree.push_back("WF");
    Expression();
    m_parsedTree.push_back("End");
    if(!EatTokenType(RIGHT_BRACKS)){
        m_parsedTree.push_back("Error WF");
        while(!m_tokenParser->IsEOF()){
            if(EatTokenType(RIGHT_BRACKS) || tokenInList(BSF_start)){
                break;
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
    }
    BlockStatementFollow();
    m_parsedTree.push_back("End");
    return true;
}

bool GrammarParser::AssignSentence(){
    m_parsedTree.push_back("AssignSentence");
    Variable();
    m_parsedTree.push_back("End");
    if(!EatTokenType(ASSIGN)){
        m_parsedTree.push_back("Error AssignSentence");
        while(!m_tokenParser->IsEOF()){
            if(EatTokenType(ASSIGN) || tokenInList(Expression_start)){
                break;
            }
            if(tokenInList(assignSen_postfix)){
                return true;
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
    }
    Expression();
    return false;
}

bool GrammarParser::Expression(){
    m_parsedTree.push_back("Expression");
    if(Term()){
        m_parsedTree.push_back("End");
        ExpressionFollow();
        m_parsedTree.push_back("End");
        return true;
    }
    else if(EatTokenType(MINUS)){
        Term();
        m_parsedTree.push_back("End");
        ExpressionFollow();
        m_parsedTree.push_back("End");

        return true;
    }
    else{
        m_parsedTree.push_back("ERROR Expression");
        while(!m_tokenParser->IsEOF()){
            if(tokenInList(Expression_postfix)){
                return true;
            }
            if(tokenInList(Expression_start)){
                return Expression();
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
    }
    return false;
}

bool GrammarParser::ExpressionFollow(){
    m_parsedTree.push_back("ExpressionFollow");
    if(EatTokenType(PLUS)){
        return true;
    }
    else if(EatTokenType(MINUS)){
        return true;
    }
    else{
        return true;
    }
}

bool GrammarParser::Term(){
    m_parsedTree.push_back("Term");
    if(Factor()){
        m_parsedTree.push_back("End");
        TermFollow();
        return true;
    }
    else if(EatTokenType(LEFT_BRACKS)){
        Expression();
        m_parsedTree.push_back("End");
    }
    m_parsedTree.push_back("Error Term");
    while(!m_tokenParser->IsEOF()){
        if(tokenInList(Term_postfix)){
            return true;
        }
        if(tokenInList(Term_start)){
            return Term();
        }
        else{
            EatTokenType(m_tokenParser->GetToken().type);
        }
    }
    return true;
}

bool GrammarParser::TermFollow(){
    m_parsedTree.push_back("TermFollow");
    if(EatTokenType(MULTI)){

    }
    else if(EatTokenType(DIVIDES)){

    }
    else if(EatTokenType(EQUAL)){

    }
    else if(EatTokenType(UNEQUAL)){

    }
    else if(EatTokenType(LESS)){

    }
    else if(EatTokenType(LESSEQUAL)){

    }
    else if(EatTokenType(BIGER)){

    }
    else if(EatTokenType(BIGEREQUAL)){

    }
    else{
        return false;
    }

    Term();
    m_parsedTree.push_back("End");
    return true;
}

bool GrammarParser::Factor(){
    m_parsedTree.push_back("Factor");
    if(EatTokenType(NUMBER_INT)){
        return true;
    }
    else if(EatTokenType(NUMBER_REAL)){
        return true;
    }
    Variable();
    m_parsedTree.push_back("End");
    return true;
}

bool GrammarParser::Variable(){
    m_parsedTree.push_back("Variable");
    const std::string id = EatId();
    if(id != "") return true;
    m_parsedTree.push_back("Error Variable");
    while(!m_tokenParser->IsEOF()){
        if(tokenInList(Variable_postfix)){
            return true;
        }
        if(tokenInList(Variable_start)){
            return Variable();
        }
        else{
            EatTokenType(m_tokenParser->GetToken().type);
        }
    }
    return true;
}

int GrammarParser::ArrayStruct(){
    if(EatTokenType(LEFT_ARRAY)){
        m_parsedTree.push_back("AS");
        Expression();
        if(!EatTokenType(RIGHT_ARRAY)){
            m_parsedTree.push_back("Error AS");
            while(!m_tokenParser->IsEOF()){
                if(EatTokenType(RIGHT_ARRAY) || tokenInList(AS_postfix)){
                    return true;
                }
                if(tokenInList(AS_start)){
                    return ArrayStruct();
                }
                else{
                    EatTokenType(m_tokenParser->GetToken().type);
                }
            }
        }
    }
    else return -1;
}

/*bool GrammarParser::FunctionCallFollow(){
    if(EatTokenType(LEFT_BRACKS)){
        if(FunctionParamsList()){
            if(EatTokenType(RIGHT_BRACKS)){

            }
            else{
                //Error missing )
            }
        }
        else if(EatTokenType(RIGHT_BRACKS)){

        }
        else{
            //Error missing )
        }
        return true;
    }
    return false;
}

bool GrammarParser::FunctionParamsList(){
    if(Expression()){
        if()
    }
}*/


bool GrammarParser::EatTokenType(TokenType type){
    if(m_tokenParser->GetToken().type == type){
        m_lastToken = m_tokenParser->GetToken();
        m_tokenParser->ReadNext();
        return true;
    }
    else return false;
}

const std::string GrammarParser::EatId(){
    if(EatTokenType(ID)) return m_lastToken.id;
    else return "";
}

bool GrammarParser::tokenInList(const TokenType typeList[]){
    for(int i = 0; i < (sizeof(typeList) / sizeof(typeList[0])); ++i){
        if(m_tokenParser->GetToken().type == typeList[i]){
            return true;
        }
    }
    return false;
}
