#include <string.h>
#include <list>
#include <iostream>

using namespace std;

class relTable
{
private:
    class relEntry
    {
    private:
    public:
        string section;
        int offset;
        int symbol;
        int addend;
        char type;
        bool linkerModified=false;
        relEntry(int offset, int symbol,int addend,char type, string section){
            this->offset=offset;
            this->symbol=symbol;
            this->addend=addend;
            this->type=type;
            this->section=section;
        };
        ~relEntry(){};
    };
    list<relEntry> rels;
public:
    relTable();
    ~relTable();
    int addRel(int offset,int symbol,char type, int addend, string section);
    list<relEntry>* getTable();
    void printTable(string name);
    void readFromBinary(istream* in);
    void writeToBinary(ostream* o);

};