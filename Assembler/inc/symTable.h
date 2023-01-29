#include <stdio.h>
#include <iostream>
#include <string.h>
#include <list>
#include <iterator>
using namespace std;

class symTable
{
private:
    class sym
    {
    private:
    public:
        string name;
        int value;
        int rb;
        string section;
        char type;
        sym(string name, int value, int rb, string section, char type){
            this->name=name;
            this->value=value;
            this->rb=rb;
            this->section=section;
            this->type=type;
        };
        ~sym(){};
    };
    list <sym> syms;
public:
    symTable(/* args */);
    ~symTable();

    int addSym(string name,int value, string section, char type);
    int setGlobalSym(string name);
    int setValueSym(string name,int value);
    int getSymValue(string name);
    int getSymRB(string name);
    char getSymType(string name);
    string getSymSection(string name);
    int setSymSection(string symName,string secName);

    void printTable(string name);
    void writeToBinary(ostream* o);
    void readFromBinary(istream* in);
    list<sym>* getTable();
};
