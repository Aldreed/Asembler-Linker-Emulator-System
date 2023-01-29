#include "../inc/relTable.h"
#include <fstream>

relTable::relTable(){};

relTable::~relTable(){};
int relTable::addRel(int offset,int symbol,char type, int addend, string section){
    
    for (list<relEntry>::iterator i = rels.begin(); i != rels.end(); i++)
    {
        if(i->section.compare(section)==0&&i->offset==offset){
            return -1;
        }
    }

    relEntry* temp = new relEntry(offset,symbol,addend,type, section);
    rels.push_front(*temp);
    return 0;

};
list<relTable::relEntry>* relTable::getTable(){
    return &rels;
};

void relTable::printTable(string name){
    ofstream out(name,ios::out|ios::app);
    out<<"SECTION "<<"OFFSET "<<"SYMBOL "<<"ADDEND "<<"TYPE"<<endl;
    for (list<relEntry>::iterator i = rels.begin(); i != rels.end(); i++)
    {
     out<<i->section<<" "<<i->offset <<" "<<i->symbol<<" "<<i->addend<<" "<<i->type<<endl;  
    }
    out.close();
}

void relTable::writeToBinary(ostream* o){
    int temp=rels.size();
    o->write((char*)&temp,sizeof(int));

    for(list<relEntry>::iterator i = rels.begin(); i!=rels.end(); i++){
        temp = i->section.size();
        o->write((char*)&temp,sizeof(int));

        *o<<i->section;

        temp=i->offset;
        o->write((char*)&temp,sizeof(int));

        temp=i->symbol;
        o->write((char*)&temp,sizeof(int));

        temp=i->addend;
        o->write((char*)&temp,sizeof(int));

        o->write((char*)&i->type,sizeof(char));
        
    }

}


void relTable::readFromBinary(istream* in){
    int temp = 0;
    in->read((char*)&temp,sizeof(int));

    for (int j = 0; j < temp; j++)
    {
        int secSize=0;
        in->read((char*)&secSize,sizeof(int));
        char* sec = new char[secSize+1];
        in->read((char*)sec,sizeof(char)*secSize);
        sec[secSize]='\0';

        int offset=0;
        in->read((char*)&offset,sizeof(int));

        int symbol = 0;
        in->read((char*)&symbol,sizeof(int));
        
        int addend=0;
        in->read((char*)&addend,sizeof(int));

        char type='d';
        in->read((char*)&type,sizeof(char));

        relEntry n(offset,symbol,addend,type,sec);
        delete [] sec;
        rels.push_back(n);
 
    }
    
}