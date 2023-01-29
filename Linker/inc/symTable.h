#include <stdio.h>
#include <iostream>
#include <string.h>
#include <list>
#include <iterator>
using namespace std;

class symTable
{
private:
public:
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
    symTable(/* args */);
    ~symTable();

    int addSym(string name,int value, string section, char type);
    int setGlobalSym(string name);
    int setValueSym(string name,int value);
    int getSymValue(string name);
    int getSymRB(string name);
    int getSymValueRB(int rb);
    char getSymType(string name);
    string getSymSection(string name);
    int setSymSection(string symName,string secName);

    void printTable(string name);
    void writeToBinary(ofstream* o);
    void readFromBinary(ifstream* in);
    list<sym>* getTable();
};
