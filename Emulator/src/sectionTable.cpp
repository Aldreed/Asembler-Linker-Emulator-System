#include "../inc/sectionTable.h"
#include <fstream>

sectionTable::sectionTable(bool init){
        this->initTable();
};
sectionTable::sectionTable(){};
sectionTable::~sectionTable(){};

list<sectionTable::section>* sectionTable::getTable(){
    return &sections;
};


int sectionTable::initTable(){
    if(sections.empty()){
    section* temp = new section(".undf",0);
    sections.push_front(*temp);

    temp = new section(".abs",0);
    sections.push_front(*temp);

    return 0;
    }
    return -1;
};

int sectionTable::addSection(string name, int start){

    if(name.empty()){
        cout<<"can't add section"<<endl;
        return -1;
    }


    for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
    {
        if(i->name.compare(name)==0){
            cout<<"Multiple definitions of: "<<name<<endl;
            return -1;
        }
    }

    section* temp = new section(name,start);
    sections.push_front(*temp);
    return 0;
};

int sectionTable::updateSize(string name,int nSize){

    for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
    {
        if(i->name.compare(name)==0){
            i->size=nSize;
            return 0;
        }
    }

    return -1;

};


int sectionTable::updateStart(string name,int nStart){

    for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
    {
        if(i->name.compare(name)==0){
            i->size=nStart;
            return 0;
        }
    }

    return -1;
};
int sectionTable::addCode(string name,char c){
    for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
    {
        if(i->name.compare(name)==0){
            i->code.push_back(c);
            i->size++;
            return 0;
        }
    }

    return -1;
};
int sectionTable::updateCode(string name, char c, int offset){
    for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
    {
        if(i->name.compare(name)==0){
            i->code.at(offset)=c;
            return 0;
        }
    }

    return -1;
};

void sectionTable::printTable(){
    cout<<"NAME "<< "START " << "SIZE" << endl;
    for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
    {
        cout<<i->name<<" "<<i->start<<" "<<i->size <<endl;
         for (int j = 1; j <= i->code.size(); j++)
         {
             printf("%02hhx ",i->code.at(j-1));
             if((j%10==0)){
                 cout<<endl;
             }
         }
         cout<<endl;
    }
}


void sectionTable::writeToBinary(ofstream* o){
    int sizeList = sections.size(); 
    o->write((char*)&sizeList,sizeof(int));

   for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
    {
        int temp = i->name.size();

        o->write((char*)&temp,sizeof(int));//size of name

        // o->write((char*)&i->name,(i->name.size()+1)*sizeof(char));
        // o->write((char*)(i->name).c_str(),(temp)*sizeof(char));
        *o<<i->name;
        
        o->write((char*)&i->size,sizeof(int));//size of code block
        for (int j = 0; j < i->size; j++)
        {
            o->write((char *)&i->code[j],sizeof(char));
        }
        temp = i->start;
        o->write((char*)&temp,sizeof(int));//start


    }
    
}


void sectionTable::readFromBinary(ifstream* in){
    int sizeList = 0; 
    in->read((char*)&sizeList,sizeof(int));

    for (int r = 0; r < sizeList; r++)
    {
        int temp=0;
        in->read((char*)&temp,sizeof(int));//size of name

         char* name = new char[temp+1];
         in->read(name,((temp)*sizeof(char)));
         name[temp]='\0';
         section s(name,0);
         delete [] name;

         in->read((char*)&temp,sizeof(int));//size of code block

        for (int c = 0; c < temp; c++)
        {
            char tmpByte;
            in->read((char*)&tmpByte,sizeof(char));
            s.code.push_back(tmpByte);
        }
        s.size=temp;
        in->read((char*)&temp,sizeof(int));//start
        s.start=temp;
        sections.push_back(s);

    }
    



//    for (list<section>::iterator i = sections.begin(); i != sections.end(); i++)
//     {
//         in.read((char*)&i->name,(i->name.size()+1)*sizeof(char));
//         in.read((char*)&i->size,sizeof(int));
//         for (int j = 0; j < i->size; j++)
//         {
//             in.read((char *)&i->code[j],sizeof(char));
//         }
        
//     }
}
