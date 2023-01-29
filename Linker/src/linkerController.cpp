#include "../inc/linkerController.h"
#include <fstream>
#include <iomanip>

int linkerController::addSectionMap(linkerController::sectionMap m){
    for (list<sectionMap>::iterator i = sectionMaps.begin(); i !=sectionMaps.end(); i++)
    {
        if(i->section.compare(m.section)==0){
            return -1;
        }
    }

    sectionMaps.push_back(m);
    return 0;
    
};

void linkerController::addBatch(ifstream* in){
    linkerController::batch b(global_nmb++);

    b.sec_Tab.readFromBinary(in);
    
    b.sym_Tab.readFromBinary(in);
    b.rel_Tab.readFromBinary(in);
    batches.push_back(b);
};

int linkerController::trim(){
    list<list<sectionMap>::iterator> temp;
    for (list<sectionMap>::iterator i = sectionMaps.begin(); i !=sectionMaps.end(); i++)
    {
        bool found =false;
        for(list<batch>::iterator j = batches.begin();j!=batches.end();j++){
            for(list<sectionTable::section>::iterator k = j->sec_Tab.sections.begin(); k!=j->sec_Tab.sections.end();k++){
                if(k->name.compare(i->section)==0){
                    found=true;
                    break;
                }
            }
        
            if(found){
                break;
            }
        }

        if(!found){
            temp.push_back(i);
        }

    }

    for (list<list<sectionMap>::iterator>::iterator i = temp.begin(); i != temp.end(); i++)
    {
        sectionMaps.erase(*i);
    }
    

    return 0;    
}


int linkerController::updateSections(){
    
    //Gather section info and size

    for(list<batch>::iterator i = batches.begin();i!=batches.end();i++){
        for(list<sectionTable::section>::iterator j = i->sec_Tab.sections.begin(); j!=i->sec_Tab.sections.end();j++){
            
            bool found=false;
            for (list<sectionMap>::iterator k = sectionMaps.begin(); k !=sectionMaps.end(); k++)
            {
                if(k->section.compare(j->name)==0){
                    found =true;
                    k->size+=j->size;
                    break;
                }
            }

            if(!found){
                sectionMap s(0,j->name);
                s.size=j->size;
                s.ltr=true;
                this->addSectionMap(s);
            }
        }
    }

    //Check for address overlap

    unsigned long high_addr=0;

    for (list<sectionMap>::iterator i = sectionMaps.begin(); i !=sectionMaps.end();i++)
    {
        if(i->ltr){
            continue;
        }
        unsigned long cur_start = i->location;
        unsigned long cur_end=i->location+i->size;
        if(cur_end>high_addr){
            high_addr=cur_end;
        }

        for (list<sectionMap>::iterator j = i; j !=sectionMaps.end(); j++)
        {
            if(i==j||j->ltr){
                continue;
            }
            
            unsigned long j_start = j->location;
            unsigned long j_end=j->location+j->size;

            if((cur_start<=j_start&&cur_end>=j_start)||
                (cur_start>j_start&&cur_end<j_start)||
                (cur_start<j_end&&cur_end>j_start)){
                    cout<<"Adress overlap: "<<i->section<<" "<<j->section<<endl;
                    cout<<"First addr pair: "<<cur_start<<" "<<cur_end<<endl;
                    cout<<"Second addr pair: "<<j_start<<" "<<j_end<<endl;

                exit(-1);
            }
        }
    }


    // Set other addresses
    for (list<sectionMap>::iterator i = sectionMaps.begin(); i !=sectionMaps.end(); i++)
    {
        if(!i->ltr){
            continue;
        }

        i->location=high_addr;
        high_addr+=i->size;
    }


    return 0;
};

int linkerController::updateSymbols(){
    globals.clear();

    //update defined symbols and gather globals

    for(list<batch>::iterator i = batches.begin();i!=batches.end();i++){
        
        for (list<symTable::sym>::iterator k = i->sym_Tab.syms.begin(); k != i->sym_Tab.syms.end(); k++)
            {
               for(list<sectionMap>::iterator j=sectionMaps.begin();j!=sectionMaps.end();j++){
                    if(k->section.compare(j->section)==0&&k->section.compare(".undf")!=0&&k->section.compare(".abs")!=0){
                        k->value+=j->location;
                        break;
                    }
                }

                if(k->type=='g'&&(k->section.compare(".undf")!=0)){
                    for (list<symTable::sym>::iterator l = globals.begin(); l != globals.end(); l++)
                    {
                        if(l->name.compare(k->name)==0){
                            cout<<"Multiple definitions of: "<<k->name<<endl;
                            return -1;
                        }
                    }
                    globals.push_back(*k);
                }
            }
        
    }
    
    //update undefined symbols
    for(list<batch>::iterator i = batches.begin();i!=batches.end();i++){
        for (list<symTable::sym>::iterator k = i->sym_Tab.syms.begin(); k != i->sym_Tab.syms.end(); k++)
            {
                if(k->section.compare(".undf")!=0){
                    continue;
                }
                bool found=false;
                for (list<symTable::sym>::iterator l = globals.begin(); l != globals.end(); l++)
                {
                    if(l->name.compare(k->name)==0){
                        found=true;
                        k->type='g';
                        k->section=l->section;
                        k->value=l->value;
                    }
                }

                if(!found){
                    cout<<"undefined symbol: "<<k->name;
                    return -2;
                }
            }
    }

    // for(list<batch>::iterator i = batches.begin();i!=batches.end();i++){
    //     i->sym_Tab.printTable();
    // }

    

    return 0;

};


int linkerController::updateSectionsCode(){
    for(list<batch>::iterator i = batches.begin();i!=batches.end();i++){
        for(list<relTable::relEntry>::iterator j = i->rel_Tab.rels.begin();j!=i->rel_Tab.rels.end();j++){
            int sym_value = i->sym_Tab.getSymValueRB(j->symbol);
            int sec_value = i->sym_Tab.getSymValue(j->section);
            if(sym_value==-1){
                cout<<"unexpected error: SectionsCode"<<endl;
                exit(-1);
            }

            if(j->type=='r'){
                //sym_value=sym_value-j->offset-1+j->addend;
                sym_value=sym_value-j->offset-j->addend - sec_value;
            }

            char upper = (sym_value>>8)&0xFF;
            char lower = (sym_value)&0xFF;
            int temp=0;
            int temp2=0;
            if(j->type=='w'){//little endian order for word fill
                temp=i->sec_Tab.updateCode(j->section,upper,j->offset+1);
                temp2=i->sec_Tab.updateCode(j->section,lower,j->offset);
            }
            else{
                temp=i->sec_Tab.updateCode(j->section,upper,j->offset);
                temp2=i->sec_Tab.updateCode(j->section,lower,j->offset+1);
            }
            if(temp==-1||temp2==-1){
                cout<<"unexpected error: SectionsCode 2"<<endl;
                exit(-1);
            }
        }
        // i->sec_Tab.printTable();
    }
    return 0;
}

int linkerController::updateRelocations(){
    for(list<batch>::iterator i = batches.begin();i!=batches.end();i++){
        for(list<relTable::relEntry>::iterator j = i->rel_Tab.rels.begin();j!=i->rel_Tab.rels.end();j++){
            int temp = i->sym_Tab.getSymValue(j->section);;
            if(temp==-1){
                cout<<"unexpected error: Relocations"<<endl;
                exit(-1);
            }
            j->offset=j->offset+temp;
        }
    }
    return 0;
}


int linkerController::mergeSections(){
    for (list<sectionMap>::iterator i = sectionMaps.begin(); i != sectionMaps.end(); i++)
    {
        fin_section.addSection(i->section,i->location);
    }

    for (list<batch>::iterator i = batches.begin(); i != batches.end(); i++)
    {
        for (list<sectionTable::section>::iterator j = i->sec_Tab.sections.begin(); j != i->sec_Tab.sections.end(); j++)
        {
            for (int k  = 0; k<j->code.size(); k++)
            {
                fin_section.addCode(j->name,j->code[k]);//updates size
            }
        }
    }
    return 0;
}

int linkerController::mergeSyms(){
    for (list<batch>::iterator i = batches.begin(); i != batches.end(); i++)
    {
        for (list<symTable::sym>::iterator j = i->sym_Tab.syms.begin(); j != i->sym_Tab.syms.end(); j++)
        {
            symTable::sym temp(j->name,j->value,fin_sym.syms.size(),j->section,j->type);
            bool found=false;
            for (list<symTable::sym>::iterator k = fin_sym.syms.begin(); k != fin_sym.syms.end(); k++)
            {
                if(k->name.compare(temp.name)==0&&k->type=='g'){
                    found=true;
                    for(list<relTable::relEntry>::iterator l = i->rel_Tab.rels.begin();l!=i->rel_Tab.rels.end();l++){
                        if(l->symbol==j->rb&&l->linkerModified==false){
                            l->symbol=k->rb;
                            l->linkerModified=true;
                        }
                    }
                }
            }
            
            if(!found){
                fin_sym.syms.push_front(temp);
                for(list<relTable::relEntry>::iterator k = i->rel_Tab.rels.begin();k!=i->rel_Tab.rels.end();k++){
                    if(k->symbol==j->rb&&k->linkerModified==false){
                        k->symbol=fin_sym.syms.size()-1;
                        k->linkerModified=true;
                    }
                }
            }
        }
    }

    //add rels
    return 0;
}

//always needs to be called after mergeSyms so that the relocations are up to date
int linkerController::mergeRels(){
    for(list<batch>::iterator i = batches.begin(); i!=batches.end();i++){
        for(list<relTable::relEntry>::iterator j = i->rel_Tab.rels.begin();j!=i->rel_Tab.rels.end();j++){
            int temp =0;
            temp =fin_rel.addRel(j->offset,j->symbol,j->type,j->addend,j->section);
            if(temp<0){
                cout<<"Unexpected errror: Merge syms"<<endl;
                exit(-1);
            }
        }
    }
    return 0;
}


void linkerController::printFinalHex(string outputfile){
    ofstream out;
    out.open(outputfile);

    fin_section.sections.sort([](sectionTable::section a,sectionTable::section b){if(a.start<b.start)return true;else return false;});
    unsigned int addr =0;
    bool first=true;

    bool valid=false;
    int old_size=0;
    int old_start=0;
    for(list<sectionTable::section>::iterator i = fin_section.sections.begin();i!=fin_section.sections.end();i++){
        
        if(i->size==0){
            continue;
        }

        if(i!=fin_section.sections.begin()&&!first){
            if(old_size+old_start==i->start){
                valid=true;
            }
            else{
                for (int  j = 0; addr%8!=0&&j < 8-(addr)%8; j++)
                {
                    out<<hex<<setw(2)<<setfill('0')<<0x00<<' ';
                }
                valid=false;
                out<<endl;
                if(addr&8!=0)out<<endl;
            }
        }

        if(!valid&&i->start%8!=0){
            out<<hex<<setw(4)<<setfill('0')<<i->start-i->start%8<<" : ";
            for (int j = 0; j < i->start%8; j++)
            {
                out<<hex<<setw(2)<<setfill('0')<<0x00<<" ";
            }
        }

        addr=i->start;
        for (int j = 0; j < i->size; j++)
        {
            if(addr%8==0){
                out<<hex<<setw(4)<<setfill('0')<<addr<<" : ";
            }
            out<<hex<<setfill('0')<<setw(2)<<(i->code[j]&0xFF)<<' ';
            if(addr%8==7){
                out<<endl;
            }
            addr++;
        }

        old_size=i->size;
        old_start=i->start;
        first=false;

    }

    for (int  j = 0; addr%8!=0&&j < 8-(addr)%8; j++)
    {
        out<<hex<<setw(2)<<setfill('0')<<0x00<<' ';
    }

    out<<endl;

    out.close();

}


linkerController::linkerController(){this->global_nmb=0;};
linkerController::~linkerController(){};