#ifndef _H_CMM_TOKENPARSER_
#define _H_CMM_TOKENPARSER_

#include <iostream>
#include <fstream>
#include <map>

enum TokenType{
    IF = 0,
    ELSE,
    WHILE,
    READ,
    WRITE,
    INT,
    REAL,
    ID,

    PLUS,
    PLUS_ASSIGN,
    MINUS,
    MINUS_ASSIGN,
    MULTI,
    MULTI_ASSIGN,
    DIVIDES,
    DIVIDES_ASSIGN,

    ASSIGN,

    LESS,
    LESSEQUAL,
    BIGER,
    BIGEREQUAL,
    EQUAL,
    UNEQUAL,

    LEFT_BRACKS,
    RIGHT_BRACKS,
    SEMICOLON,
    LEFT_BLOCK,
    RIGHT_BLOCK,
    LEFT_ARRAY,
    RIGHT_ARRAY,

    NUMBER_INT,
    NUMBER_REAL,

    ILLEGLE,

    TOKEN_TYPE_TOTAL
};

enum TokenState{
    STATE_OLD,
    STATE_NEW,
    STATE_LINECOMMENT,
    STATE_BLOCKCOMMENT
};

struct Token{
    char id[32];
    double value;
    TokenType type;
    TokenState state;
    int line;
    int column;
};

typedef std::map<std::string, TokenType> STR_TT_MAP;

class TokenParser{
    public:
        TokenParser();
        TokenParser(const char *file_name);
        ~TokenParser();

        bool ReadNext();

        const Token& GetToken()const;
        const int GetLine()const;
        const int GetColumn()const;
        const std::string GetFilename()const { return m_filename; }
        bool IsEOF();

        static const std::string& TranslateType(TokenType type);

    protected:
        void ReadGap();
        void ReadOp();
        void ReadNum();
        void ReadId();

        void NextChar();
        TokenType GetIdType(const std::string &id)const;
        void ToLineEnd();
        void ToBlockCommentEnd();

    private:
        char m_next;
        Token m_token;

        std::string m_filename;
        int m_line;
        int m_column;

        std::ifstream *m_input;
        bool file_eof;
        bool eof;

    private:
        static void CreatePreservedMap();
        static STR_TT_MAP sm_preservedIdMap;
        static std::string sm_typeStr[TOKEN_TYPE_TOTAL];
};

#endif
