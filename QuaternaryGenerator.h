#ifndef QUATERNARYGENERATOR_H
#define QUATERNARYGENERATOR_H

#include <vector>
#include <stack>
#include "IdTable.h"
#include <iostream>

#define QUATERNARY_ARG1_DEFAULT "arg1"
#define QUATERNARY_ARG2_DEFAULT "arg2"
#define QUATERNARY_RESULT_DEFAULT "result"
#define QUATERNARY_NULL "-"

enum OptionType{
    OPTION_PLUS = 0,
    OPTION_MINUS = 1,
    OPTION_MULTI = 2,
    OPTION_DIVIDES = 3,
    OPTION_EQUAL = 4,
    OPTION_UNEQUAL = 5,
    OPTION_LESS = 6,
    OPTION_LESSEQUAL = 7,
    OPTION_BIGER = 8,
    OPTION_BIGEREQUAL = 9,
    OPTION_READ = 10,
    OPTION_WRITE = 11,
    OPTION_MOVE = 12,
    OPTION_OFFSET = 13,
    OPTION_JUMP = 14,
    OPTION_JUMPNIF = 15,
    OPTION_END = 16,
    OPTION_TOTAL = 17
};

enum QuaternaryArgs{
    ARG_OP,
    ARG_ARG1,
    ARG_ARG2,
    ARG_RESULT
};

struct Quaternary{
    OptionType op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

struct JumpLink{
    int last_line;
};

class QuaternaryGenerator
{
public:
    QuaternaryGenerator();

    int AddQuaternaryDefault(OptionType type);
    int AddQuaternary(OptionType type,
                      const std::string &arg1 = QUATERNARY_NULL,
                      const std::string &arg2 = QUATERNARY_NULL,
                      const std::string &result = QUATERNARY_NULL);
    int AddQuaternary(Quaternary &q);

    void EnterStack();
    void QuitStack();
    const IdInfo* GenerateTemp(IdType type);
    void RegisterId(const IdInfo *info, IdType type);
    const IdInfo* GetIdInfo(const std::string &id);

    void SetJumpNIfEnd(int line);

    void EnterJumpLink();
    void AddJumpLink();
    void EndJumpLink();

    const std::string GetQuaternaries()const;

    int GetNextLine()const{ return m_result.size() + 1; }

protected:
    void ReplaceStr(int line, QuaternaryArgs arg, std::string target);

private:
    std::vector<Quaternary> m_result;
    std::vector<JumpLink> m_jumpLinks;
    std::vector<IdTable *> m_idTables;

    int m_memory_pos;

    static std::string sm_optiontypes[OPTION_TOTAL];
};

#endif // QUATERNARYGENERATOR_H
