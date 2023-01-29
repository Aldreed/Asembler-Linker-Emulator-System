#include <stdio.h>
#include <string.h>
#include <iostream>
#include <list>
#include <vector>
#include <iterator>
using namespace std;

class sectionTable
{
private:
    class section
    {
    private:
    public:
        string name;
        int start;
        int size;
        vector<char> code;
        section(string name,int start){
            this->name=name;
            this->start=start;
            this->size=0;
        };
        ~section(){};
    };
    list<section> sections;
public:
    sectionTable(bool init);
    sectionTable();
    ~sectionTable();

    int initTable();
    int addSection(string name, int start);
    int updateSize(string name,int nSize);
    int updateStart(string name,int nStart);
    int addCode(string name,char c);
    int updateCode(string name, char c, int offset);
    void printTable(string name);
    void writeToBinary(ostream* o);
    void readFromBinary(istream* in);
    list<section>* getTable();

};

