#ifndef _H_CMM_IDTABLE_
#define _H_CMM_IDTABLE_

#include <map>

struct Token;

enum IdType{
    ID_INT,
    ID_REAL,
    ID_FUNCTION,
    ID_UNKNOWN
};

struct IdInfo{
    std::string name;
    IdType type;
    int memoPos;
    int lastLinkedPos;
    int defLine;
    int defColumn;
    int arrayPos;
    bool isConst;
    bool isRegistered;
    std::string defFilename;
};

typedef std::map<std::string, IdInfo*> STR_ID_MAP;

class IdTable{
public:
    IdTable();
    ~IdTable();
    bool AddId(IdInfo *idInfo);
    const IdInfo* GetIdInfo(const std::string &name)const;

private:
    STR_ID_MAP m_table;
};

#endif
