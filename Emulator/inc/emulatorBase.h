#include "vector"
#include "../inc/sectionTable.h"
#include "stdint.h"
using namespace std;

class emulatorBase
{

public:
    struct Register{
        int rb;
        uint16_t value;
    };
    vector<Register>regs;
    char mem[0xFFFF+1];

    //tajmer
    //terminal


    int initMem(sectionTable t);
    void debugPrintMem();

    int parse();

    void setPC(int16_t i);
    void incPC();
    char readPC();

    void parseARI(int cur_b);
    void parseLOG(int cur_b);
    void parseMOV(int cur_b);
    void parseJMP(int cur_b);


    void setZ(int z);
    void setC(int c);
    void setN(int n);
    void setO(int o);

    bool getZ();
    bool getC();
    bool getN();
    bool getO();

    bool getEQ();
    bool getNEQ();
    bool getGT();

    void setINT();
    void setT1();
    void setTr();

    void clINT();
    void clT1();
    void clTr();

    bool getINT();
    bool getT1();
    bool getTr();

    void intrError();


    uint16_t readWordMem(uint16_t addr);

    emulatorBase(/* args */);
    ~emulatorBase();
};

