#include "QuaternaryGenerator.h"
#include "util.h"
#include <stdlib.h>
#include <sstream>
std::string QuaternaryGenerator::sm_optiontypes[OPTION_TOTAL] = {
    "+",
    "-",
    "*",
    "/",
    "==",
    "<>",
    "<",
    "<=",
    ">",
    ">=",
    "READ",
    "WRITE",
    "MOVE",
    "OFFSET",
    "JUMP",
    "JUMPNIF",
    "END"
};

QuaternaryGenerator::QuaternaryGenerator():
    m_memory_pos(0)
{
}

void QuaternaryGenerator::ReplaceStr(int line, QuaternaryArgs arg, std::string target){
    Quaternary q = m_result[line - 1];
    switch (arg) {
    case ARG_ARG1:
        q.arg1 = target;
        break;
    case ARG_ARG2:
        q.arg2 = target;
        break;
    case ARG_RESULT:
        q.result = target;
        break;
    default:
        break;
    }
    m_result[line - 1] = q;
}

int QuaternaryGenerator::AddQuaternaryDefault(OptionType type){
    Quaternary q;
    q.op = type;
    switch(type){
    case OPTION_JUMPNIF:
        q.arg1 = QUATERNARY_ARG1_DEFAULT;
        q.arg2 = QUATERNARY_ARG2_DEFAULT;
        q.result = QUATERNARY_NULL;
        break;
    case OPTION_END:
        q.arg1 = QUATERNARY_NULL;
        q.arg2 = QUATERNARY_NULL;
        q.result = QUATERNARY_NULL;
        break;
    }
    return AddQuaternary(q);
}

int QuaternaryGenerator::AddQuaternary(OptionType type, const std::string &arg1, const std::string &arg2, const std::string &result){
    Quaternary q;
    q.op = type;
    q.arg1 = arg1;
    q.arg2 = arg2;
    q.result = result;

    return AddQuaternary(q);
}

int QuaternaryGenerator::AddQuaternary(Quaternary &q){
    const IdInfo *arg1Info = GetIdInfo(q.arg1);
    const IdInfo *arg2Info = GetIdInfo(q.arg2);
    const IdInfo *resInfo = GetIdInfo(q.result);
    if(arg1Info){
        q.arg1 = "M(" + std::to_string(arg1Info->memoPos) + ")";
    }
    if(arg2Info){
        q.arg2 = "M(" + std::to_string(arg2Info->memoPos) + ")";
    }
    if(resInfo){
        q.result = "M(" + std::to_string(resInfo->memoPos) + ")";
    }
    m_result.push_back(q);
    return m_result.size();
}

void QuaternaryGenerator::EnterStack(){
    IdTable *table = new IdTable;
    m_idTables.push_back(table);
}

void QuaternaryGenerator::QuitStack(){
    IdTable *top = m_idTables.at(m_idTables.size() - 1);
    m_idTables.pop_back();
    DELETE(top);
}

const IdInfo* QuaternaryGenerator::GenerateTemp(IdType type){
    IdInfo *info = new IdInfo;

    info->name = "M(" + std::to_string(m_memory_pos) + ")";
    info->isConst = false;
    info->arrayPos = -1;
    RegisterId(info, type);
    return info;
}

void QuaternaryGenerator::RegisterId(const IdInfo *info, IdType type){
    IdTable *currentTable = m_idTables.at(m_idTables.size() - 1);
    IdInfo *_info = new IdInfo;
    if(1 < info->arrayPos){
        _info->memoPos = m_memory_pos;
        m_memory_pos += info->arrayPos;
    }
    else{
        _info->memoPos = m_memory_pos++;
    }
    _info->isRegistered = true;
    _info->type = type;
    _info->defColumn = info->defColumn;
    _info->defFilename = info->defFilename;
    _info->defLine = info->defLine;
    _info->isConst = info->isConst;
    _info->name = info->name;
    currentTable->AddId(_info);
}

const IdInfo* QuaternaryGenerator::GetIdInfo(const std::__cxx11::string &id){
    std::vector<IdTable *>::reverse_iterator it = m_idTables.rbegin();
    const IdInfo *info = NULL;
    for(;it < m_idTables.rend(); ++it){
        info = (*it)->GetIdInfo(id);
        if(info) return info;
    }
    return NULL;
}

void QuaternaryGenerator::SetJumpNIfEnd(int line){
    int next_line = line;
    int current_line;
    int end_line = m_result.size() + 1;
    while(next_line != -1)
    {
        current_line = next_line;
        Quaternary &q = m_result[current_line - 1];
        next_line = atoi(q.arg2.c_str());
        q.arg2 = std::to_string(end_line);
    }
}

void QuaternaryGenerator::EnterJumpLink(){
    JumpLink link;
    link.last_line = -1;
    m_jumpLinks.push_back(link);
}

void QuaternaryGenerator::AddJumpLink(){
    JumpLink top = m_jumpLinks.at(m_jumpLinks.size() - 1);
    m_jumpLinks.at(m_jumpLinks.size() - 1).last_line = AddQuaternary(OPTION_JUMP, std::to_string(top.last_line));
}

void QuaternaryGenerator::EndJumpLink(){
    int last_line = m_jumpLinks.at(m_jumpLinks.size() - 1).last_line;
    int end_line = m_result.size() + 1;
    int current_line = -1;
    while(last_line != -1){
        current_line = last_line;
        last_line = atoi(m_result[current_line - 1].arg1.c_str());
        m_result[current_line - 1].arg1 = std::to_string(end_line);
    }
    m_jumpLinks.pop_back();
}

const std::string QuaternaryGenerator::GetQuaternaries()const{
    std::vector<Quaternary>::const_iterator it = m_result.begin();
    std::ostringstream ost;
    for(int i = 1;it != m_result.end(); ++it){
        ost << "(" << sm_optiontypes[(*it).op]
            << "," << (*it).arg1
            << "," << (*it).arg2
            << "," << (*it).result
            << ")\n";
        ++i;
    }
    return ost.str();
}
