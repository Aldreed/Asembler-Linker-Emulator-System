#include <stdio.h>
#include <string.h>
#include <iostream>
#include "../inc/sectionTable.h"
#include "../inc/relTable.h"
#include "../inc/symTable.h"
struct dataManager
{
    string regD;
    string regS;
    string symbol;
    int literal;
    int type;
};



using namespace std;


void global (string section,int offset,bool first,string symb,sectionTable* Sections,symTable* Syms);
void exter(string section,int offset,bool first,string symb,symTable* Syms);
void section(bool first,string sym,sectionTable* Sections,symTable* Syms);
void word(string section,int offset,bool first,dataManager input, sectionTable* Sections, symTable* Syms, relTable* Rels);
void skip(string section, int offset,bool first, int literal,sectionTable* Sections);
void equ(string section,int offset,bool first,dataManager input, sectionTable* Sections, symTable* Syms);

void halt(string section,int offset,bool first, sectionTable* Sections);
void intr(string section,int offset,bool first,string regD,sectionTable* Sections,symTable* Syms,relTable* Rels);
void iret(string section,int offset,bool first, sectionTable* Sections);
void call(string section,int offset,bool first,dataManager input,sectionTable* Sections,symTable* Syms,relTable* Rels);
void ret(string section,int offset,bool first, sectionTable* Sections);
void jmp(string section,int offset,bool first, dataManager input,string jumpType,sectionTable* Sections,symTable* Syms,relTable* Rels);
// // int push()
// // int pop()

void xchg(string section,int offset,bool first,dataManager input, sectionTable* Sections);
void ari(string section,int offset,bool first, dataManager input,string ariType, sectionTable* Sections);
void log(string section,int offset,bool first, dataManager input,string logType, sectionTable* Sections);
void mov(string section,int offset,bool first, dataManager input,string movType, sectionTable* Sections) ;

void ld(string section,int offset,bool first,dataManager input,sectionTable* Sections,symTable* Syms,relTable* Rels);
void str(string section,int offset,bool first,dataManager input,sectionTable* Sections,symTable* Syms,relTable* Rels);


void label(string section,int offset,bool first,string symbol,sectionTable* Sections,symTable* Syms);

int parseReg(string reg);
int parseARI(string ariType);
int parseLOG(string logType);
int parseMOV(string movType);
int parseJMP(string jmpType);

void writeToSection(string section, sectionTable* Sections, int value);
