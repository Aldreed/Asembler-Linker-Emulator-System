#include "relTable.h"
#include "symTable.h"
#include "sectionTable.h"
#include  <string>
#include <iostream>
#include <stdio.h>
#include <list>
using namespace std;

class linkerController
{
    public:
        class sectionMap
        {
            private:
            public:
            bool ltr;
            unsigned long size;
            unsigned long location;
            string section;
            sectionMap(unsigned long loc,string sec){
                this->location=loc;
                this->section=sec;
                this->size=0;
                this->ltr=false;
            }
            ~sectionMap(){};
        };
    list<sectionMap>sectionMaps;
    list<symTable::sym> globals;

        class batch{
            public:
            int rb;
            sectionTable sec_Tab;
            symTable sym_Tab;
            relTable rel_Tab;

            batch(int r){
                rb=r;
            }
            ~batch(){};
        };
    list<batch>batches;
    
    sectionTable fin_section;
    symTable fin_sym;
    relTable fin_rel;

    int global_nmb;

    void addBatch(ifstream* in);

    int addSectionMap(sectionMap);

    int trim();

    int updateSections();
    int updateSymbols();
    int updateSectionsCode();
    int updateRelocations();

    int mergeSections();
    int mergeSyms();
    int mergeRels();

    void printFinalHex(string outputfile);

    linkerController();
    ~linkerController();
};

