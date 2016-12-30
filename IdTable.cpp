#include "IdTable.h"
#include "TokenParser.h"

IdTable::IdTable(){}

IdTable::~IdTable(){
    STR_ID_MAP::iterator it = m_table.begin();
    for(;it != m_table.end(); ++it){
        delete it->second;
        it->second = NULL;
    }
    m_table.clear();
}

bool IdTable::AddId(IdInfo *idInfo){
    if(m_table.end() == m_table.find(idInfo->name)){
        m_table[idInfo->name] = idInfo;
        return true;
    }
    else{
        return false;
    }
}

const IdInfo* IdTable::GetIdInfo(const std::string &name) const{
   STR_ID_MAP::const_iterator it = m_table.find(name);
    if(m_table.end() == it) return NULL;
    else return it->second;
}
