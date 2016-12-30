#include <string.h>
#include "TokenParser.h"

#define CHECK_VALIDATE(p) {(p); switch(m_token.state){case STATE_NEW: return true; case STATE_LINECOMMENT: case STATE_BLOCKCOMMENT: ReadNext();break;default:break;}}

STR_TT_MAP TokenParser::sm_preservedIdMap;
std::string TokenParser::sm_typeStr[TOKEN_TYPE_TOTAL] = {
    "if",
    "else",
    "while",
    "read",
    "write",
    "int",
    "real",
    "id",
    "+",
    "+=",
    "-",
    "-=",
    "*",
    "*=",
    "/",
    "/=",
    "=",
    "<",
    "<=",
    ">",
    ">=",
    "==",
    "<>",
    "(",
    ")",
    ";",
    "{",
    "}",
    "[",
    "]",
    "number_int",
    "number_real",
    "illegle"
};

const std::string& TokenParser::TranslateType(TokenType type){
    return sm_typeStr[type];
}

void TokenParser::CreatePreservedMap(){
    sm_preservedIdMap["if"] = IF;
    sm_preservedIdMap["else"] = ELSE;
    sm_preservedIdMap["while"] = WHILE;
    sm_preservedIdMap["read"] = READ;
    sm_preservedIdMap["write"] = WRITE;
    sm_preservedIdMap["int"] = INT;
    sm_preservedIdMap["real"] = REAL;
}

TokenParser::TokenParser(const char *file_name) :
    m_input(NULL),
    m_next('\0'),
    m_filename(file_name),
    m_line(1),
    m_column(1),
    file_eof(false),
    eof(false)
{
    if(sm_preservedIdMap.size() == 0){
        CreatePreservedMap();
    }
    m_input = new std::ifstream(file_name);
    if(!m_input->is_open()){
        std::cout<< "No file named " << file_name << "!" << std::endl;
    }
    m_token.id[0] = '\0';
    m_token.value = 0.0;
    m_token.state = STATE_OLD;
    m_token.line = 1;
    m_token.column = 0;
    NextChar();
}

TokenParser::~TokenParser(){
    m_input->close();
    delete m_input;
    m_input = NULL;
}

const Token& TokenParser::GetToken()const{
    return m_token;
}
const int TokenParser::GetLine()const{
    return m_token.line;
}
const int TokenParser::GetColumn()const{
    return m_token.column;
}


bool TokenParser::ReadNext(){
    if(!m_input->is_open()) return false;
    if(file_eof) {
        eof = true;
        return false;
    }
    switch(m_token.state){
    case STATE_OLD:
    case STATE_NEW:
        m_token.state = STATE_OLD;
        break;
    case STATE_LINECOMMENT:
        ToLineEnd();
        m_token.state = STATE_OLD;
        break;
    case STATE_BLOCKCOMMENT:
        ToBlockCommentEnd();
        m_token.state = STATE_OLD;
        break;
    }

    ReadGap();
    CHECK_VALIDATE(ReadOp());
    CHECK_VALIDATE(ReadNum());
    CHECK_VALIDATE(ReadId());
    if(eof) return false;
    else return true;
}

void TokenParser::ReadGap(){
    while(!file_eof && (m_next == ' ' || m_next == '\t' || m_next == '\r' || m_next == '\n')){
        if(m_next == '\n'){
            ++m_line;
            m_column = 0;
        }
        NextChar();
    }
}

void TokenParser::ReadOp(){
    int c = m_column;
    switch(m_next){
        case '+':
            NextChar();
            if(m_next == '='){
                m_token.type = PLUS_ASSIGN;
                NextChar();
            }
            else{
                m_token.type = PLUS;
            }
            m_token.column = c;
            m_token.line = m_line;
            m_token.state = STATE_NEW;
            break;
        case '-':
            NextChar();
            if(m_next == '='){
                m_token.type = MINUS_ASSIGN;
                NextChar();
            }
            else{
                m_token.type = MINUS;
            }
            m_token.column = c;
            m_token.line = m_line;
            m_token.state = STATE_NEW;
            break;
        case '*':
            NextChar();
            if(m_next == '='){
                m_token.type = MULTI_ASSIGN;
                NextChar();
            }
            else{
                m_token.type = MULTI;
            }
            m_token.column = c;
            m_token.line = m_line;
            m_token.state = STATE_NEW;
            break;
        case '/':
            NextChar();
            if(m_next == '='){
                m_token.column = c;
                m_token.line = m_line;
                m_token.type = DIVIDES_ASSIGN;
                NextChar();
                m_token.state = STATE_NEW;
            }
            else if(m_next == '/'){
                m_token.state = STATE_LINECOMMENT;
                NextChar();
            }
            else if(m_next == '*'){
                m_token.state = STATE_BLOCKCOMMENT;
                NextChar();
            }
            else{
                m_token.column = c;
                m_token.line = m_line;
                m_token.type = DIVIDES;
                m_token.state = STATE_NEW;
            }
            break;
        case '=':
            NextChar();
            if(m_next == '='){
                m_token.type = EQUAL;
                NextChar();
            }
            else{
                m_token.type = ASSIGN;
            }
            m_token.column = c;
            m_token.line = m_line;
            m_token.state = STATE_NEW;
            break;
        case '<':
            NextChar();
            if(m_next == '='){
                m_token.type = LESSEQUAL;
                NextChar();
            }
            else if(m_next == '>'){
                m_token.type = UNEQUAL;
                NextChar();
            }
            else{
                m_token.type = LESS;
            }
            m_token.column = c;
            m_token.line = m_line;
            m_token.state = STATE_NEW;
            break;
        case '>':
            NextChar();
            if(m_next == '='){
                m_token.type = BIGEREQUAL;
                NextChar();
            }
            else{
                m_token.type = BIGER;
            }
            m_token.column = c;
            m_token.line = m_line;
            m_token.state = STATE_NEW;
            break;
        case '(':
            NextChar();
            m_token.column = c;
            m_token.line = m_line;
            m_token.type = LEFT_BRACKS;
            m_token.state = STATE_NEW;
            break;
        case ')':
            NextChar();
            m_token.column = c;
            m_token.line = m_line;
            m_token.type = RIGHT_BRACKS;
            m_token.state = STATE_NEW;
            break;
        case '{':
            NextChar();
            m_token.column = c;
            m_token.line = m_line;
            m_token.type = LEFT_BLOCK;
            m_token.state = STATE_NEW;
            break;
        case '}':
            NextChar();
            m_token.column = c;
            m_token.line = m_line;
            m_token.type = RIGHT_BLOCK;
            m_token.state = STATE_NEW;
            break;
        case ';':
            NextChar();
            m_token.column = c;
            m_token.line = m_line;
            m_token.type = SEMICOLON;
            m_token.state = STATE_NEW;
            break;
        case '[':
            NextChar();
            m_token.column = c;
            m_token.line = m_line;
            m_token.type = LEFT_ARRAY;
            m_token.state = STATE_NEW;
            break;
        case ']':
            NextChar();
            m_token.column = c;
            m_token.line = m_line;
            m_token.type = RIGHT_ARRAY;
            m_token.state = STATE_NEW;
            break;
        default:
            return;
    }
}

void TokenParser::ReadNum(){
    if(m_next < '0' || m_next > '9') return;
    int c = m_column;
    int number = 0;
    int f = 0;
    std::string number_str = "";
    //int
    while(m_next >= '0' && m_next <= '9'){
        number *= 10;
        number += m_next - '0';
        number_str += m_next;
        NextChar();
    }
    //real
    if(m_next == '.'){
        number_str += ".";
        NextChar();
        while(m_next >= '0' && m_next <= '9'){
            ++f;
            number *= 10;
            number += m_next - '0';
            number_str += m_next;
            NextChar();
        }
    }
    m_token.column = c;
    m_token.line = m_line;
    if(f == 0){
        strncpy(m_token.id, number_str.c_str(), 31);
        m_token.type = NUMBER_INT;
        m_token.value = number;
        m_token.state = STATE_NEW;
    }
    else{
        strncpy(m_token.id, number_str.c_str(), 31);
        m_token.type = NUMBER_REAL;
        m_token.value = (number * 1.0) / f;
        m_token.state = STATE_NEW;
    }
}

void TokenParser::ReadId(){
    if(!isalpha(m_next))    return;

    int c = m_column;
    char id[32] = {'\0'};
    int i = 0;
    for(;(isalnum(m_next) || m_next == '_') && i < 32; ++i){
        id[i] = m_next;
        NextChar();
    }
    if(i == 32){
        //Error: ID takes too much char
        m_token.column = c;
        m_token.line = m_line;
        m_token.type = ILLEGLE;
        m_token.state = STATE_NEW;
    }
    if(id[i - 1] == '_'){
        //Error ID ends with '_'
        m_token.column = c;
        m_token.line = m_line;
        m_token.type = ILLEGLE;
        strcpy(m_token.id, id);
        m_token.state = STATE_NEW;
    }
    else{
        m_token.column = c;
        m_token.line = m_line;
        m_token.type = GetIdType(id);
        strcpy(m_token.id, id);
        m_token.state = STATE_NEW;
    }
}

void TokenParser::NextChar(){
    if(!m_input->get(m_next)) file_eof = true;
    ++m_column;
}

TokenType TokenParser::GetIdType(const std::string &id)const{
    STR_TT_MAP::iterator it = sm_preservedIdMap.find(id);
    return it != sm_preservedIdMap.end()? it->second : ID;
}

void TokenParser::ToLineEnd(){
    while(m_next != '\n'){
        NextChar();
    }
    ++m_line;
    m_column = 0;
    NextChar();
}

void TokenParser::ToBlockCommentEnd(){
    while(m_next != '*'){
        NextChar();
        if(m_next == '\n'){
            ++m_line;
            m_column = 0;
            NextChar();
        }
    }
    NextChar();
    if(m_next != '/') ToBlockCommentEnd();
    else NextChar();
}

bool TokenParser::IsEOF(){
    if(!eof){
        ReadGap();
        eof = file_eof;
    }
    return eof;
}
