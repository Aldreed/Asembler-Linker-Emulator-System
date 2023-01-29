#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include "../inc/linkerController.h"
using namespace std;



int main(int argc,char** argv){
    bool hex=false;
    bool linkable=false;
    bool out=false;
    bool options=true;
    bool skip = false;
    string outputName = argv[0];
    outputName=outputName+".txt";
    linkerController ctr;

    list<string>readFiles;


    //IZLAZ JEDNINO KAD JE HEX ILI LINKABLE TRUE
    //NE PRAVI FAJLOVE PRE TOGA

    for (int i = 1; i < argc; i++)
    {
        string holder = argv[i];
        if(options){
            if(holder.find("place=",0)!=string::npos){
                //cout<<holder.substr(holder.find('=',0)+1,holder.find('@')-holder.find('=',0)-1)<<" placed at: "<<holder.substr(holder.find('@')+1,holder.size()-holder.find('@'))<<endl;
                unsigned long loc = stoi(holder.substr(holder.find('@')+1,holder.size()-holder.find('@',0)),nullptr,0);
                string sec = holder.substr(holder.find('=',0)+1,holder.find('@')-holder.find('=')-1);
                
                linkerController::sectionMap temp (loc,sec);

                if(ctr.addSectionMap(temp)!=0){
                    cout<<"Duplicate section location for section: "<<sec<<endl;
                    return -1;
                };
            
            }
            else if(holder.compare("-hex")==0){
                if(!linkable){
                    hex=true;
                }
                else{
                    cout<<"Calling error"<<endl;
                    return -1;
                }
            }
            else if(holder.compare("-linkable")==0){
                if(!hex){
                    linkable=true;
                }
                else{
                    cout<<"Calling error"<<endl;
                    return -1;
                }
            }
            else if(holder.compare("-o")==0){
                if(!out){
                    out=true;
                    outputName=argv[++i];
                    //cout<<"output file: "<<outputName<<endl;
                }
                else{
                    cout<<"Calling error"<<endl;
                    return -1;
                }
            }
            else{
                options = false;
                skip=false;

                for (list<string>::iterator j = readFiles.begin(); j != readFiles.end(); j++)
                {
                    if((*j).compare(argv[i])==0){
                      skip =true;
                       break;
                    }
                }

                if(!skip){
                ifstream in;
                in.open(argv[i],ios::binary|ios::in);
                if(in.fail()){
                    cout<<"cannot open file: "<<argv[i]<<endl;
                    return -1;
                }
                else{
                    ctr.addBatch(&in);
                }
                in.close();

                readFiles.push_back(argv[i]);
                }

                //cout<<"done"<<endl;
            }
        }
        else{
            options = false;
            skip=false;

            for (list<string>::iterator j = readFiles.begin(); j != readFiles.end(); j++)
            {
                if((*j).compare(argv[i])==0){
                    skip =true;
                    break;
                }
            }
            
            if(!skip){
            ifstream in;
            in.open(argv[i],ios::binary|ios::in);
            if(in.fail()){
                cout<<"cannot open file: "<<argv[i]<<endl;
                return -1;
                }
            else{
                ctr.addBatch(&in);
            }
            in.close();
            readFiles.push_back(argv[i]);
            }

        }
    }

    if(hex==false&&linkable==false){
        cout<<"Calling error"<<endl;
        return -1;
    }
    
    ctr.trim();

    ctr.updateSections();
    ctr.updateSymbols();
    
    ctr.updateSectionsCode();
    ctr.updateRelocations();
    

    ctr.mergeSections();
    ctr.mergeSyms();
    ctr.mergeRels();

    
    


    if(hex){
        string temp = argv[0];
        temp=temp+".txt";
        ctr.printFinalHex(temp);

        if(out){
        //outputName=outputName+".bin";
        ofstream binary_output(outputName,ios::binary|ios::out|ios::trunc);
        
        ctr.fin_section.writeToBinary(&binary_output);//for emu init
        
        binary_output.close();
        }
    }
    else if(linkable){
        
        string temp2 = argv[0];
        temp2=temp2+".txt"; 
        ofstream out(temp2,ios::trunc);
        out.close();

        ctr.fin_sym.printTable(temp2);
        ctr.fin_section.printTable(temp2);
        ctr.fin_rel.printTable(temp2);

        if(out){
        //outputName=outputName+".bin";
        ofstream binary_output(outputName,ios::binary|ios::out|ios::trunc);
        
        ctr.fin_section.writeToBinary(&binary_output);
        ctr.fin_sym.writeToBinary(&binary_output);
        ctr.fin_rel.writeToBinary(&binary_output);

        binary_output.close();
        }
    }

    
}