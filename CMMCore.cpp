#include "CMMCore.h"
#include "util.h"
#include <sstream>
#include <stdlib.h>

#define CHECKSEMICOLON(p)     if(!EatTokenType(SEMICOLON))                   \
{m_errorHandler->AddError(EXPECTED_SEMICOLON, ERROR_GRAMMAR, m_tokenParser); \
    while(!m_tokenParser->IsEOF()){                                          \
        if(EatTokenType(SEMICOLON) || tokenInList((p))){                      \
            return true;                                                     \
        }                                                                    \
        else{                                                                \
            EatTokenType(m_tokenParser->GetToken().type);                    \
        }                                                                    \
    }                                                                        \
}

//Start & Postfix
const TokenType program_start[] = {
    LEFT_BLOCK,
    IF,
    WHILE,
    READ,
    WRITE,
    INT,
    REAL,
    ID
};
const TokenType program_postfix[] = {
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

const TokenType *sentence_postfix = program_postfix;
const TokenType *assignSen_postfix = program_postfix;

const TokenType Expression_start[] = {
    MINUS,
    NUMBER_INT,
    NUMBER_REAL,
    ID,
    LEFT_BRACKS
};

const TokenType Expression_postfix[] = {
    SEMICOLON,
    RIGHT_BRACKS
};

const TokenType *BSF_start = program_start;


const TokenType *BSF_postfix = program_postfix;

const TokenType IFF_start[] = {
    LEFT_BRACKS
};

const TokenType *IFF_postfix = program_postfix;
const TokenType *WF_postfix = IFF_postfix;
const TokenType *WF_start = IFF_start;

const TokenType Term_postfix[] = {
    PLUS,
    MINUS
};

const TokenType Term_start[] = {
    NUMBER_INT,
    NUMBER_REAL,
    ID,
    LEFT_BRACKS
};

const TokenType TermFollow_start[] = {
    MULTI,
    DIVIDES,
    EQUAL,
    UNEQUAL,
    LESS,
    LESSEQUAL,
    BIGER,
    BIGEREQUAL
};

const TokenType Variable_postfix[] = {
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

const TokenType Variable_start[] = {
    ID
};

const TokenType *AS_postfix = Variable_postfix;

const TokenType AS_start[] = {
    LEFT_BLOCK
};

//================


CMMCore::CMMCore():
    m_tokenParser(NULL),
    m_errorHandler(NULL),
    m_qg(NULL),
    m_mode(IDLE)
{
}

CMMCore::~CMMCore(){
    DELETE(m_tokenParser)
    DELETE(m_qg)
    ErrorHandler::DestroyErrorHandler();
}

void CMMCore::CompileFile(std::__cxx11::string &filePath){
    m_tokenParser = new TokenParser(filePath.c_str());
    m_qg = new QuaternaryGenerator();
    m_errorHandler = ErrorHandler::GetInstance();
    Start();
    m_mode = COMPILED;
}

bool CMMCore::HasError(){
    if(NULL != m_errorHandler){
        return m_errorHandler->HasError();
    }
    return false;
}

const std::string CMMCore::GetErrors(){
    if(NULL != m_errorHandler){
        return m_errorHandler->GetErrors();
    }
    return "";
}

const std::string CMMCore::GetQuaternaries(){
    if(NULL != m_qg){
        return m_qg->GetQuaternaries();
    }
    return "";
}

void CMMCore::RunResult(){
    if(COMPILED == m_mode && !HasError()){
        time_t t = time(NULL);
        std::stringstream sst;
        sst << "temp_quartenary" << t << ".tmp";
        std::ofstream o(sst.str(), std::ios::out);
        o << GetQuaternaries();
        RunFile(sst.str());
        remove(sst.str().c_str());
    }
}

void CMMCore::RunFile(const std::__cxx11::string &filePath){
    system(("java QR " + filePath).c_str());
}


void CMMCore::Start(){
    if(m_tokenParser->ReadNext()){
        m_qg->EnterStack();
        Program();
        m_qg->QuitStack();
        m_qg->AddQuaternaryDefault(OPTION_END);
        if(m_tokenParser->IsEOF()) return;
        else m_errorHandler->AddError(END_BEFORE_EOF, ERROR_GRAMMAR, m_tokenParser);
    }
    else{
        m_errorHandler->AddError(EMPTY_SOURCE_FILE, ERROR_GRAMMAR, m_tokenParser);
    }
}

bool CMMCore::Program(){
    if(Block() || Sentence()){
        return true;
    }
    while(!m_tokenParser->IsEOF()){
        m_errorHandler->AddError(EXPECTED_PROGRAM, ERROR_GRAMMAR, m_tokenParser);
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

bool CMMCore::Sentence(){
    if(EatTokenType(IF)){
        m_qg->EnterJumpLink();
        IfFollow();
        m_qg->EndJumpLink();
        return true;
    }
    else if(EatTokenType(WHILE)){
        WhileFollow();
        return true;
    }
    else if(EatTokenType(READ)){
        const IdInfo *res = Expression();
        if(res){
            if(!res->isConst && !res->isRegistered){
                //Error undeclared variable
                m_qg->RegisterId(res, ID_REAL);
                DELETE(res);
                return true;
            }
            if(!EatTokenType(SEMICOLON)){
                m_errorHandler->AddError(EXPECTED_SEMICOLON, ERROR_GRAMMAR, m_tokenParser);
                while(!m_tokenParser->IsEOF()){
                    if(EatTokenType(SEMICOLON) || tokenInList(sentence_postfix)){
                        return true;
                    }
                    else{
                        EatTokenType(m_tokenParser->GetToken().type);
                    }
                }
            }
            if(res->type == ID_INT){
                m_qg->AddQuaternary(OPTION_READ, res->name, "int");
            }
            else if(res->type == ID_REAL){
                m_qg->AddQuaternary(OPTION_READ, res->name, "real");
            }
            return true;
        }
    }
    else if(EatTokenType(WRITE)){
        const IdInfo *res = Variable();
        if(res){
            if(!res->isConst && !res->isRegistered){
                //Error undeclared variable
                m_errorHandler->AddError(UNDECLARED_VARIABLE, ERROR_SEMANTICS, m_tokenParser);
                m_qg->RegisterId(res, ID_REAL);
                DELETE(res);
                return true;
            }
            CHECKSEMICOLON(sentence_postfix);
            if(res->type == ID_INT){
                m_qg->AddQuaternary(OPTION_WRITE, res->name, "int");
            }
            else if(res->type == ID_REAL){
                m_qg->AddQuaternary(OPTION_WRITE, res->name, "real");
            }
            return true;
        }
    }
    else if(EatTokenType(INT) || EatTokenType(REAL)){
        IdType type = (m_lastToken.type == INT?  ID_INT : ID_REAL);
        const IdInfo *variable = Variable();
        if(variable){
            if(variable->isRegistered){
                m_errorHandler->AddError(MULTI_DECLARED, ERROR_SEMANTICS, m_tokenParser);
                return true;
            }
            m_qg->RegisterId(variable, type);
            if(EatTokenType(ASSIGN)){
                const IdInfo *res = Expression();
                if(!res){
                    DELETE(variable);
                    return false;
                }
                m_qg->AddQuaternary(OPTION_MOVE, res->name, QUATERNARY_NULL, variable->name);
            }
            CHECKSEMICOLON(sentence_postfix);
            DELETE(variable);
            return true;
        }
        /*if(Function()){

        }*/
    }
    else {
        AssignSentence();
        CHECKSEMICOLON(sentence_postfix);
        return true;
    }
    return false;
}

bool CMMCore::Block(bool newTable){
    if(EatTokenType(LEFT_BLOCK)){
        if(newTable){
            m_qg->EnterStack();
        }
        while(!m_tokenParser->IsEOF() && !EatTokenType(RIGHT_BLOCK)){
            Program();
        }
        if(m_tokenParser->IsEOF() && m_tokenParser->GetToken().type != RIGHT_BLOCK){
            m_errorHandler->AddError(EXPECTED_RIGHTBLOCK, ERROR_GRAMMAR, m_tokenParser);
        }
        else if(newTable){
            m_qg->QuitStack();
        }
        return true;
    }
    return false;
}

int CMMCore::BlockStatementFollow(){
    if(EatTokenType(SEMICOLON)){
        return true;
    }
    Program();
    return true;
}

bool CMMCore::IfFollow(){
    m_qg->EnterStack();
    if(!EatTokenType(LEFT_BRACKS)){
        m_errorHandler->AddError(EXPECTED_LEFTBRACKS, ERROR_GRAMMAR, m_tokenParser);
        while(!m_tokenParser->IsEOF()){
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
    const IdInfo *res = Expression();
    if(res){
        int line = m_qg->AddQuaternary(OPTION_JUMPNIF, res->name, "-1");
        if(!EatTokenType(RIGHT_BRACKS)){
            m_errorHandler->AddError(EXPECTED_RIGHTBRACKS, ERROR_GRAMMAR, m_tokenParser);
            while(!m_tokenParser->IsEOF()){
                if(EatTokenType(RIGHT_BRACKS) || tokenInList(BSF_start)){
                    break;
                }
                else{
                    EatTokenType(m_tokenParser->GetToken().type);
                }
            }
        }
        if(BlockStatementFollow()){
            m_qg->AddJumpLink();
            m_qg->SetJumpNIfEnd(line);
            Else();
            m_qg->QuitStack();
            return true;
        }
        else{
            m_qg->QuitStack();
            return false;
        }
    }
    else{
        return false;
    }
}

bool CMMCore::Else(){
    if(EatTokenType(ELSE)){
        if(EatTokenType(IF)){
            return IfFollow();
        }
        else if(0 != BlockStatementFollow()){
            return true;
        }
    }
    return false;
}

bool CMMCore::WhileFollow(){
    m_qg->EnterStack();
    if(!EatTokenType(LEFT_BRACKS)){
        m_errorHandler->AddError(EXPECTED_LEFTBRACKS, ERROR_GRAMMAR, m_tokenParser);
        while(!m_tokenParser->IsEOF()){
            if(tokenInList(WF_postfix)){
                return true;
            }
            else if(tokenInList(WF_start)){
                return WhileFollow();
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
        return true;
    }

    int start_line = m_qg->GetNextLine();
    const IdInfo *res = Expression();
    if(res){
        int line = m_qg->AddQuaternary(OPTION_JUMPNIF, res->name, "-1");
        if(!EatTokenType(RIGHT_BRACKS)){
            m_errorHandler->AddError(EXPECTED_RIGHTBRACKS, ERROR_GRAMMAR, m_tokenParser);
            while(!m_tokenParser->IsEOF()){
                if(EatTokenType(RIGHT_BRACKS) || tokenInList(BSF_start)){
                    break;
                }
                else{
                    EatTokenType(m_tokenParser->GetToken().type);
                }
            }
        }
        if(BlockStatementFollow()){
            m_qg->AddQuaternary(OPTION_JUMP, std::to_string(start_line));
            m_qg->SetJumpNIfEnd(line);
            m_qg->QuitStack();
            return true;
        }
        else{
            m_qg->QuitStack();
            return false;
        }
    }
    return false;
}

bool CMMCore::AssignSentence(){
    const IdInfo *variable = Variable();
    if(variable){
        if(!EatTokenType(ASSIGN)){
            m_errorHandler->AddError(EXPECTED_ASSIGN, ERROR_GRAMMAR, m_tokenParser);
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
        const IdInfo *value = Expression();
        if(value){
            m_qg->AddQuaternary(OPTION_MOVE, value->name, QUATERNARY_NULL, variable->name);
            return true;
        }
        else{
            return false;
        }
    }
    return false;
}

const IdInfo* CMMCore::Expression(){
    const IdInfo *arg1 = Term();
    if(arg1){
        const IdInfo *res = ExpressionFollow(arg1);
        if(res) return res;
        else return arg1;
    }
    else if(EatTokenType(MINUS)){
        const IdInfo *arg1 = Term();
        if(arg1){
            const IdInfo *temp = m_qg->GenerateTemp(arg1->type);
            m_qg->AddQuaternary(OPTION_MULTI, arg1->name, "-1", temp->name);
            const IdInfo *res = ExpressionFollow(temp);
            if(res) return res;
            else return temp;
        }
        else{
            return NULL;
        }
    }
    else{
        while(!m_tokenParser->IsEOF()){
            m_errorHandler->AddError(EXPECTED_EXPRESSION, ERROR_GRAMMAR, m_tokenParser);
            if(tokenInList(Expression_postfix)){
                return NULL;
            }
            if(tokenInList(Expression_start)){
                return Expression();
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
        return NULL;
    }
}

const IdInfo* CMMCore::ExpressionFollow(const IdInfo *arg1){
    std::string temp;
    IdType temp_type;
    OptionType op_type;
    if(EatTokenType(PLUS)){
        op_type = OPTION_PLUS;
    }
    else if(EatTokenType(MINUS)){
        op_type = OPTION_MINUS;
    }
    else{
        return NULL;
    }
    const IdInfo *arg2 = Expression();
    if(!arg2){
        return NULL;
    }
    else if(arg1->isConst && arg2->isConst){
        IdInfo *info = new IdInfo;
        double res = 0;
        double a1 = atof(arg1->name.c_str());
        double a2 = atof(arg2->name.c_str());
        switch (op_type) {
        case OPTION_PLUS:
            res = a1 + a2;
            break;
        case OPTION_MINUS:
            res = a1 - a2;
            break;
        }
        info->type = (arg1->type == ID_INT && arg2->type == ID_INT) ? ID_INT : ID_REAL;
        info->name = std::to_string(res);
        info->isConst = true;

        delete arg1;
        delete arg2;
        return info;
    }
    else{
        temp_type = (arg1->type == ID_INT && arg2->type == ID_INT)? ID_INT : ID_REAL;

        const IdInfo *info = m_qg->GenerateTemp(temp_type);
        m_qg->AddQuaternary(op_type, arg1->name, arg2->name, info->name);
        if(arg1->isConst){
            delete arg1;
        }
        if(arg2->isConst){
            delete arg2;
        }
        return info;
    }
}

const IdInfo* CMMCore::Term(){
    const IdInfo *arg1 = Factor();
    if(arg1){
        const IdInfo *res = TermFollow(arg1);
        if(res){
            return res;
        }
        else{
            return arg1;
        }
    }
    else if(EatTokenType(LEFT_BRACKS)){
        const IdInfo *arg1 = Expression();
        if(arg1){
            if(EatTokenType(RIGHT_BRACKS)){
                const IdInfo *res = TermFollow(arg1);
                if(res){
                    return res;
                }
                else{
                    return arg1;
                }
            }
            else{
                m_errorHandler->AddError(EXPECTED_RIGHTBRACKS, ERROR_GRAMMAR, m_tokenParser);
                while(!m_tokenParser->IsEOF()){
                    if(EatTokenType(RIGHT_BRACKS)){
                        const IdInfo *res = TermFollow(arg1);
                        if(res){
                            return res;
                        }
                        else{
                            return arg1;
                        }
                    }
                    if(tokenInList(Term_postfix)){
                        return arg1;
                    }
                    if(tokenInList(TermFollow_start)){
                        const IdInfo *res = TermFollow(arg1);
                        if(res){
                            return res;
                        }
                        else{
                            return arg1;
                        }
                    }
                    else{
                        EatTokenType(m_tokenParser->GetToken().type);
                    }
                }
                return arg1;
            }
        }
    }
    else{
        while(!m_tokenParser->IsEOF()){
            m_errorHandler->AddError(EXPECTED_TERM, ERROR_GRAMMAR, m_tokenParser);
            if(tokenInList(Term_postfix)){
                return NULL;
            }
            if(tokenInList(Term_start)){
                return Term();
            }
            else{
                EatTokenType(m_tokenParser->GetToken().type);
            }
        }
    }
    return NULL;
}

const IdInfo* CMMCore::TermFollow(const IdInfo *arg1){
    OptionType op_type;
    std::string temp;
    IdType temp_type = ID_REAL;
    if(EatTokenType(MULTI)){
        op_type = OPTION_MULTI;
    }
    else if(EatTokenType(DIVIDES)){
        op_type = OPTION_DIVIDES;
    }
    else if(EatTokenType(EQUAL)){
        op_type = OPTION_EQUAL;
        temp_type = ID_INT;
    }
    else if(EatTokenType(UNEQUAL)){
        op_type = OPTION_UNEQUAL;
        temp_type = ID_INT;
    }
    else if(EatTokenType(LESS)){
        op_type = OPTION_LESS;
        temp_type = ID_INT;
    }
    else if(EatTokenType(LESSEQUAL)){
        op_type = OPTION_LESSEQUAL;
        temp_type = ID_INT;
    }
    else if(EatTokenType(BIGER)){
        op_type = OPTION_BIGER;
        temp_type = ID_INT;
    }
    else if(EatTokenType(BIGEREQUAL)){
        op_type = OPTION_BIGEREQUAL;
        temp_type = ID_INT;
    }
    else{
        return NULL;
    }

    const IdInfo* arg2 = Term();
    if(!arg2){
        return NULL;
    }
    else if(arg1->isConst && arg2->isConst){
        IdInfo *info = new IdInfo;
        double res = 0;
        double a1 = atof(arg1->name.c_str());
        double a2 = atof(arg2->name.c_str());
        switch (op_type) {
        case OPTION_MULTI:
            res = a1 * a2;
            break;
        case OPTION_DIVIDES:
            res = a1 / a2;
            break;
        case OPTION_EQUAL:
            res = (double)(a1 == a2);
            break;
        case OPTION_UNEQUAL:
            res = (double)(a1 != a2);
            break;
        case OPTION_LESS:
            res = (double)(a1 < a2);
            break;
        case OPTION_LESSEQUAL:
            res = (double)(a1 <= a2);
            break;
        case OPTION_BIGER:
            res = (double)(a1 > a2);
            break;
        case OPTION_BIGEREQUAL:
            res = (double)(a1 >= a2);
            break;
        }
        if(temp_type == ID_INT || (arg1->type == ID_INT && arg2->type == ID_INT)){
            info->type = ID_INT;
            info->name = std::to_string((int)res);
        }
        else{
            info->type = ID_REAL;
            info->name = std::to_string(res);
        }
        info->isConst = true;

        delete arg1;
        delete arg2;
        return info;
    }
    else{
        if(op_type == MULTI || op_type == DIVIDES){
            temp_type = (arg1->type == ID_INT && arg2->type == ID_INT)? ID_INT : ID_REAL;
        }
        const IdInfo *info = m_qg->GenerateTemp(temp_type);
        m_qg->AddQuaternary(op_type, arg1->name, arg2->name, info->name);
        if(arg1->isConst){
            delete arg1;
        }
        if(arg2->isConst){
            delete arg2;
        }
        return info;
    }
}

const IdInfo* CMMCore::Factor(){
    if(EatTokenType(NUMBER_INT)){
        IdInfo *info = new IdInfo;
        info->name = m_lastToken.id;
        info->type = ID_INT;
        info->isConst = true;
        return info;
    }
    else if(EatTokenType(NUMBER_REAL)){
        IdInfo *info = new IdInfo;
        info->name = m_lastToken.id;
        info->type = ID_REAL;
        info->isConst = true;
        return info;
    }
    else{
        const IdInfo *info = Variable();
        if(!info->isRegistered){
            m_errorHandler->AddError(UNDECLARED_VARIABLE, ERROR_SEMANTICS, m_tokenParser);
            m_qg->RegisterId(info, ID_REAL);
            std::string id = info->name;
            delete info;
            info = m_qg->GetIdInfo(id);
            return info;
        }
        return info;
    }
}

const IdInfo* CMMCore::Variable(){
    const std::string id = EatId();

    const IdInfo *info = m_qg->GetIdInfo(id);
    if(info) {
        const IdInfo *array_info = ArrayStruct();
        if(array_info){
            const IdInfo *temp = m_qg->GenerateTemp(info->type);
            m_qg->AddQuaternary(OPTION_OFFSET, info->name, array_info->name, temp->name);
            return temp;
        }
        return info;
    }

    IdInfo *new_info = new IdInfo;
    new_info->name = id;
    new_info->isRegistered = false;
    if("" != id){
        new_info->defLine = m_tokenParser->GetLine();
        new_info->defColumn = m_lastToken.column;
        new_info->defFilename = m_tokenParser->GetFilename();
        const IdInfo *array_info = ArrayStruct();
        int arrayPos = -1;
        if(array_info){
            if(!(array_info->isConst && array_info->type == ID_INT)){
                m_errorHandler->AddError(ARRAY_INDEX_NOT_CONST, ERROR_SEMANTICS, m_tokenParser);
            }
            else{
                arrayPos = atoi(array_info->name.c_str());
            }
        }
        new_info->arrayPos = arrayPos;
        new_info->isConst = false;
        return new_info;
    }
    while(!m_tokenParser->IsEOF()){
        if(tokenInList(Variable_postfix)){
            return NULL;
        }
        if(tokenInList(Variable_start)){
            return Variable();
        }
        else{
            EatTokenType(m_tokenParser->GetToken().type);
        }
    }
    return NULL;
}

const IdInfo* CMMCore::ArrayStruct(){
    if(EatTokenType(LEFT_ARRAY)){
        const IdInfo *arrayPos = Expression();
        if("" != arrayPos->name){
            if(!EatTokenType(RIGHT_ARRAY)){
                m_errorHandler->AddError(EXPECTED_RIGHTARRAY, ERROR_GRAMMAR, m_tokenParser);
                while(!m_tokenParser->IsEOF()){
                    if(EatTokenType(RIGHT_ARRAY) || tokenInList(AS_postfix)){
                        return arrayPos;
                    }
                    if(tokenInList(AS_start)){
                        return ArrayStruct();
                    }
                    else{
                        EatTokenType(m_tokenParser->GetToken().type);
                    }
                }
            }
            return arrayPos;
        }
        else return NULL;
    }
    else return NULL;
}

/*bool CMMCore::FunctionCallFollow(){
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

bool CMMCore::FunctionParamsList(){
    if(Expression()){
        if()
    }
}*/


bool CMMCore::EatTokenType(TokenType type){
    if(m_tokenParser->GetToken().type == type){
        m_lastToken = m_tokenParser->GetToken();
        m_tokenParser->ReadNext();
        return true;
    }
    else return false;
}

const std::string CMMCore::EatId(){
    if(EatTokenType(ID)) return m_lastToken.id;
    else return "";
}


bool CMMCore::tokenInList(const TokenType typeList[]){
    for(int i = 0; i < (sizeof(typeList) / sizeof(typeList[0])); ++i){
        if(m_tokenParser->GetToken().type == typeList[i]){
            return true;
        }
    }
    return false;
}

