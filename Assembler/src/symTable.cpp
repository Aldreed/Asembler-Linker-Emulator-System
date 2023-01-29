#include "../inc/symTable.h"
#include <vector>
#include <fstream>

int symTable::addSym(string name,int value, string section, char type){
    if(name.empty()||!(type=='g'||type=='l'||type=='u')){
        cout<<"cant add symbol"<<endl;
        return -1;
    }

    sym* temp = new sym(name,value,syms.size(),section,type);

    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(name)==0){
            cout<<"Multiple definitions of: "<<name<<endl;
            return -1;
        }
    }
    
    
    syms.push_front(*temp);
    return 0;


}

int symTable::setGlobalSym(string name){
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(name)==0){
            i->type='g';
            return 0;
        }
    }
    
        cout<<"Can't set "<<name<<" to global; it dosn't exist"<<endl;
        return -1;
}

int symTable::setValueSym(string name,int value){
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(name)==0){
            i->value=value;
            return 0;
        }
    }
    
        cout<<"Can't set "<<name<<"; it dosn't exist"<<endl;
        return -1;
}


int symTable::setSymSection(string symName,string secName){
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(symName)==0){
            i->section=secName;
            return 0;
        }
    }
    
        cout<<"Can't set "<<symName<<"; it dosn't exist"<<endl;
        return -1;
}


void symTable::printTable(string name){
    ofstream out(name,ios::out|ios::app);
    out<<"RB" << " " << "NAME" << " " << "SECTION" << " " <<"TYPE"<< " " << "VALUE" <<endl;
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        out<<i->rb << " "<< i->name << " " << i->section << " " << i->type<< " " << i->value<<endl; 
    }
    out.close();
}

list<symTable::sym>* symTable::getTable(){
    return &syms;
}


int symTable::getSymValue(string name){
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(name)==0){
            return i->value;
        }
    }
    return -1;
}


string symTable::getSymSection(string name){
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(name)==0){
            return i->section;
        }
    }
    return nullptr;
}

char symTable::getSymType(string name){
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(name)==0){
            return i->type;
        }
    }

    return 'x';
}

int symTable::getSymRB(string name){
    for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        if(i->name.compare(name)==0){
            return i->rb;
        }
    }
    return -1;
}


void symTable::writeToBinary(ostream* o){
     int sizeList = syms.size(); 
    o->write((char*)&sizeList,sizeof(int));

   for (list<sym>::iterator i = syms.begin(); i != syms.end(); i++)
    {
        int temp = i->name.size();
        o->write((char*)&temp,sizeof(int));//size of name
        // o->write((char*)&i->name,(i->name.size()+1)*sizeof(char));
        // o->write((char*)&s,(s.size())*sizeof(char));
        *o<<i->name;

        o->write((char*)&i->value,sizeof(int));
        o->write((char*)&i->rb,sizeof(int));

        temp = i->section.size();
        o->write((char*)&temp,sizeof(int));
        *o<<i->section;
        *o<<i->type;
    }
    
};

void symTable::readFromBinary(istream* in){
    int sizeList=0;
    in->read((char*)&sizeList,sizeof(int));
    for (int j = 0; j < sizeList; j++)
    {
        int tempName=0;
        in->read((char*)&tempName,sizeof(int));
         char* name = new char[tempName+1];
         in->read((char*)name,((tempName)*sizeof(char)));
         name[tempName]='\0';
        
        
        int tmpValue=0;
        in->read((char*)&tmpValue,sizeof(int));

        int tmpRb=0;
        in->read((char*)&tmpRb,sizeof(int));

        int tempSection=0;
        in->read((char*)&tempSection,sizeof(int));

        char* sectionS= new char[tempSection+1];
        in->read((char*)sectionS,tempSection*sizeof(char));
         sectionS[tempSection]='\0';

         char tmpType;
         in->read((char*)&tmpType,sizeof(char));

         sym newSym(name,tmpValue,tmpRb,sectionS,tmpType);
         delete [] name;
         delete [] sectionS;
         syms.push_back(newSym);
    }
    

}

symTable::symTable(){}
symTable::~symTable(){}

// int main(int argc, char const *argv[])
// {
//     symTable* globalSymTable;
//     globalSymTable= new symTable();
//     globalSymTable->addSym(".data",10,".data",'l');
//     globalSymTable->printTable();
//     delete globalSymTable;
//     return 0;
// }

