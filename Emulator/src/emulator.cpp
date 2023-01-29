#include <iostream>
#include <fstream>
#include <termios.h>
#include <unistd.h>
#include "../inc/emulatorBase.h"
using namespace std;
// extern void retT();

int main(int argc,char** argv){
    
    termios temp;

    if(tcgetattr(STDIN_FILENO,&temp)<0){
        return -1;
    };

    emulatorBase emu;
    
    if(argc!=2){
        cout<<"calling error"<<endl;
        return -1;
    }

    ifstream in;
    in.open(argv[1],ios::binary|ios::in);
    if(in.fail())
    {
        cout<<"cannot open file: "<<argv[1]<<endl;
        return -1;
    }

    sectionTable secTab;
    secTab.readFromBinary(&in);
    //secTab.printTable();

    in.close();

    emu.initMem(secTab);


    emu.parse();
    
    //emu.debugPrintMem();

    tcsetattr(STDIN_FILENO,TCSAFLUSH,&temp);

}