#include "../inc/cmdManager.h"

#define XCHG_CODE 0x60
//ARI CODES
#define ADD_CODE 0x70
#define SUB_CODE 0x71
#define MUL_CODE 0x72
#define DIV_CODE 0x73
#define CMP_CODE 0x74
//LOG_CODES
#define NOT_CODE 0x80
#define AND_CODE 0x81
#define OR_CODE 0x82
#define XOR_CODE 0x83
#define TEST_CODE 0x84
//MOVE CODES
#define SHL_CODE 0x90
#define SHR_CODE 0x91
//JMP CODES
#define JMP_CODE 0x50
#define JEQ_CODE 0x51
#define JNE_CODE 0x52
#define JGT_CODE 0x53
//LD_STR CODES
#define LD_CODE 0xA0
#define ST_CODE 0xB0
//CALL
#define CALL_CODE 0x30
//RET
#define RET_CODE 0x40
//HALT
#define HALT_CODE 0x00
//INTR
#define INTR_CODE 0x10
//IRET
#define IRET_CODE 0x20


void global (string section,int offset,bool first,string symb, sectionTable* Sections,symTable* Syms){
    if(first){
        if(Syms->getSymType(symb)=='x'){
            if(Syms->addSym(symb,0,".undf",'u')!=0){
                exit(-1);
            }
        }
        else{
            if(Syms->setGlobalSym(symb)!=0){
                cout<<"Symbol undefined:"<<symb<<endl;
                exit(-1);
            }
        }
    }
    else{
        if(Syms->getSymType(symb)!='g'){
            cout<<"Symbol undefined or multiple definitions:"<<symb<<endl;
            exit(-1);
        }
    }
};
void exter(string section,int offset,bool first,string symb,symTable* Syms){//proveri
    if(!first){
        if(Syms->addSym(symb,0,".undf",'u')){
            exit(-1);
        };
    }
};
void section(bool first,string sym,sectionTable* Sections,symTable* Syms){
    if(first){
        if(Sections->addSection(sym,0)!=0){
            exit(-1);
        }
        if(Syms->addSym(sym,0,sym,'l')!=0){
            exit(-1);
        }
    }
};
void word(string section,int offset,bool first,dataManager input, sectionTable* Sections,symTable* Syms,relTable* Rels){
    if(!first){
        if(input.type==0){
            if(Syms->getSymType(input.symbol)=='x'){
                cout<<"undefined symbol: "<<input.symbol<<endl;
                exit(-1);
            }
            else{
                Rels->addRel(offset,Syms->getSymRB(input.symbol),'w',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            }
        }
        else if(input.type==1){//little endian order for memory fill
            int upperHalf = (input.literal&0xFF00)>>8;
            int lowerHalf = (input.literal&0xFF);
            writeToSection(section,Sections,lowerHalf);
            writeToSection(section,Sections,upperHalf);
        }
        else{
            cout<<"input error"<<endl;
            exit(-1);
        }
    }
};
void skip(string section,int offset,bool first, int literal,sectionTable* Sections){
    if(!first){
        for (int i = 0; i < literal; i++)
        {
            writeToSection(section,Sections,0);
        }
    }
};
void equ(string section,int offset,bool first,dataManager input, sectionTable* Sections, symTable* Syms){
    if(first){
        
        if(Syms->addSym(input.symbol,input.literal,".abs",'l')!=0){
            exit(-1);
        }
    }
};


void label(string section,int offset,bool first,string symbol,sectionTable* Sections,symTable* Syms){
    if(first){
        if(Syms->getSymType(symbol)=='u'){
            Syms->setGlobalSym(symbol);
            Syms->setValueSym(symbol,offset);
            Syms->setSymSection(symbol,section);
        }
        else if(Syms->getSymType(symbol)=='x'){
            Syms->addSym(symbol,offset,section,'l');
        }
        else{
            cout<<"Multiple definitions of: "<<symbol<<endl;
            exit(-1);
        }
    }
};

//TWO BYTE INSTR

void xchg(string section,int offset,bool first,dataManager input, sectionTable* Sections){
    if(!first){
        int regD = parseReg(input.regD);
        int regS = parseReg(input.regS);
        if(regD==-1||regS==-1){
            cout<<"register unknown: " << input.regD <<" or "<<input.regS<<endl;
        }
        else{
            writeToSection(section,Sections,XCHG_CODE);
            int temp = (regD<<4)|regS;
            writeToSection(section,Sections,temp);
        }

    }
};

void ari(string section,int offset,bool first, dataManager input,string ariType, sectionTable* Sections){
    if(!first){
        int regD = parseReg(input.regD);
        int regS = parseReg(input.regS);
        if(regD==-1||regS==-1){
            cout<<"register unknown: " << input.regD <<" or "<<input.regS<<endl;
            exit(-1);
        }
        int code = parseARI(ariType);
        if(code==-1){
            cout<<"unknown ari op: "<<ariType<<endl;
            exit(-1);
        }

        writeToSection(section,Sections,code);
        int temp = (regD<<4)|regS;
        writeToSection(section,Sections,temp);
    }
};
void log(string section,int offset,bool first, dataManager input,string logType, sectionTable* Sections){
    if(!first){
        int code = parseLOG(logType);
        if(code==-1){
            cout<<"unknown log op: "<<logType<<endl;
            exit(-1);
        }
        else if(code==NOT_CODE){
            int regD= parseReg(input.regD);
            if(regD==-1){
                cout<<"register unknown: " << input.regD <<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            int temp = regD<<4;
            writeToSection(section,Sections,temp);
        }
        else{
            int regD = parseReg(input.regD);
            int regS = parseReg(input.regS);
            if(regD==-1||regS==-1){
            cout<<"register unknown: " << input.regD <<" or "<<input.regS<<endl;
            exit(-1);
            }
            int temp = (regD<<4)|regS;

            writeToSection(section,Sections,code);
            writeToSection(section,Sections,temp);
        }
    }
}

void mov(string section,int offset,bool first, dataManager input,string movType, sectionTable* Sections){
    if(!first){
        int regD = parseReg(input.regD);
        int regS = parseReg(input.regS);
        int code = parseMOV(movType);

         if(regD==-1||regS==-1){
            cout<<"register unknown: " << input.regD <<" or "<<input.regS<<endl;
            exit(-1);
        }

        if(code==-1){
            cout<<"unknown mov op: "<<movType<<endl;
            exit(-1);
        }

        int temp = (regD<<4)|regS;

            writeToSection(section,Sections,code);
            writeToSection(section,Sections,temp);

    }
} ;


void halt(string section,int offset,bool first, sectionTable* Sections){
    if(!first){
        writeToSection(section,Sections,HALT_CODE);
    }
}

void intr(string section,int offset,bool first,string regD,sectionTable* Sections,symTable* Syms,relTable* Rels){
    if(!first){
        int reg = parseReg(regD);
        if(reg==-1){
            cout<<"unknown register: "<<regD<<endl;
            exit(-1);
        }

        reg=reg<<4;
        writeToSection(section,Sections,reg||0xF);
    }
};

void iret(string section,int offset,bool first, sectionTable* Sections){
    if(!first){
        writeToSection(section,Sections,IRET_CODE);
    }
};


void ret(string section,int offset,bool first, sectionTable* Sections){
    if(!first){
        writeToSection(section,Sections,RET_CODE);
    }
};



//OPERAND INSTRUCTIONS
void jmp(string section,int offset,bool first, dataManager input,string jumpType,sectionTable* Sections,symTable* Syms,relTable* Rels){
    if(!first){
        int code = parseJMP(jumpType);
        int upperHalf;
        int lowerHalf;
        int rbSym;
        int reg;
        if(code==-1){
            cout<<"unknown jmp op: "<<jumpType<<endl;
            exit(-1);
        }
        //3 min max 5 Bytes //note update modes
        switch (input.type)
        {
        case 1:
            lowerHalf = input.literal&0xFF;
            upperHalf = (input.literal>>8)&0xFF;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x00);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 2:
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x00);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            // if(Syms->getSymType(input.symbol)=='u'){
            //     Rels->addRel(offset+3,rbSym,'a',0,section);
            //     writeToSection(section,Sections,0x00);
            //     writeToSection(section,Sections,0x00);
            // }
            // else{
            //     int value = Syms->getSymValue(input.symbol);
            //     int upperHalf = (value>>4)&0xFF;
            //     int lowerHalf =value&0xFF;
            //     writeToSection(section,Sections,upperHalf);
            //     writeToSection(section,Sections,lowerHalf);
            // }
            break;
        case 3:
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF7);
            writeToSection(section,Sections,0x05);
            Rels->addRel(offset+3,rbSym,'r',2,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 4:
            lowerHalf = input.literal&0xFF;
            upperHalf = (input.literal>>8)&0xFF;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x04);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 5:
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x04);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 6:
            reg = parseReg(input.regD);
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x01);
            break;
        case 7:
            reg = parseReg(input.regD);
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x02);
            break;
        case 8:
            reg = parseReg(input.regD);
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = input.literal&0xFF;
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x03);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 9:
            reg = parseReg(input.regD);
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x03);
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        default:
            cout<<"wrong input type: "<<input.type<<endl;
            exit(-1);
            break;
        }
    }
};

void ld(string section,int offset,bool first,dataManager input,sectionTable* Sections,symTable* Syms,relTable* Rels){
    if(!first){
        int code = LD_CODE;
        int upperHalf;
        int lowerHalf;
        int rbSym;
        int regD;
        int regS;
        regD = parseReg(input.regD);
        regD = regD<<4;
        switch (input.type)
        {
        case 1:
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = (input.literal&0xFF);
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x00);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 2:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x00);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 3:
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = (input.literal&0xFF);
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x04);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 4:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x04);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 5:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|0x07);
            writeToSection(section,Sections,0x03);//important increment RS by 2 then add with payload
            Rels->addRel(offset+3,rbSym,'r',2,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 6:
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x01);
            break;
        case 7:
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x02);
            break;
        case 8:
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = (input.literal&0xFF);
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x03);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 9:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x03);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 10://pop
            regS=6;//sp;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x42);
            break;
        default:
            break;
        }
    }
};


void str(string section,int offset,bool first,dataManager input,sectionTable* Sections,symTable* Syms,relTable* Rels){
    if(!first){
        int code = ST_CODE;
        int upperHalf;
        int lowerHalf;
        int rbSym;
        int regD;
        int regS;
        regD = parseReg(input.regD);
        regD = regD<<4;
        switch (input.type)
        {
        case 1:
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = (input.literal&0xFF);
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x00);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 2:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x00);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 3:
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = (input.literal&0xFF);
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x04);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 4:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD);
            writeToSection(section,Sections,0x04);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 5:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|0x07);
            writeToSection(section,Sections,0x03);//important decrement RS by 2 then add with payload
            Rels->addRel(offset+3,rbSym,'r',2,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 6:
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x01);
            break;
        case 7:
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x02);
            break;
        case 8:
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = (input.literal&0xFF);
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x03);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 9:
            rbSym=Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            regS = parseReg(input.regS);
            if(regS==-1){
                cout<<"undiefined reg: "<<regS<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x03);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 10://push
            regS=6;//sp
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,regD|regS);
            writeToSection(section,Sections,0x12);
            break;
        default:
            break;
        }
    }
};

void call(string section,int offset,bool first,dataManager input,sectionTable* Sections,symTable* Syms,relTable* Rels){
     if(!first){
        int code = CALL_CODE;
        int upperHalf;
        int lowerHalf;
        int rbSym;
        int reg;
        //3 min max 5 Bytes //note update modes
        switch (input.type)
        {
        case 1:
            lowerHalf = input.literal&0xFF;
            upperHalf = (input.literal>>8)&0xFF;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x00);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 2:
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x00);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 3:
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF7);
            writeToSection(section,Sections,0x05);
            Rels->addRel(offset+3,rbSym,'r',2,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 4:
            lowerHalf = input.literal&0xFF;
            upperHalf = (input.literal>>8)&0xFF;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x04);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 5:
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,0xF0);
            writeToSection(section,Sections,0x04);
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        case 6:
            reg = parseReg(input.regD);
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x01);
            break;
        case 7:
            reg = parseReg(input.regD);
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x02);
            break;
        case 8:
            reg = parseReg(input.regD);
            upperHalf = (input.literal>>8)&0xFF;
            lowerHalf = input.literal&0xFF;
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x03);
            writeToSection(section,Sections,upperHalf);
            writeToSection(section,Sections,lowerHalf);
            break;
        case 9:
            reg = parseReg(input.regD);
            if(reg==-1){
                cout<<"unknown reg: "<<reg<<endl;
                exit(-1);
            }
            reg = reg|0xF0;
            writeToSection(section,Sections,code);
            writeToSection(section,Sections,reg);
            writeToSection(section,Sections,0x03);
            rbSym = Syms->getSymRB(input.symbol);
            if(rbSym==-1){
                cout<<"undefined symbol:"<<input.symbol<<endl;
                exit(-1);
            }
            Rels->addRel(offset+3,rbSym,'a',0,section);
                writeToSection(section,Sections,0x00);
                writeToSection(section,Sections,0x00);
            break;
        default:
            cout<<"wrong input type: "<<input.type<<endl;
            exit(-1);
            break;
        }
    }
};



//PARSERS
int parseReg(string reg){
    if(reg.compare("r0")==0){
        return 0;
    }
    else if(reg.compare("r1")==0){
        return 1;
    }
    else if(reg.compare("r2")==0){
        return 2;
    }
    else if(reg.compare("r3")==0){
        return 3;
    }
    else if(reg.compare("r4")==0){
        return 4;
    }
    else if(reg.compare("r5")==0){
        return 5;
    }
    else if(reg.compare("r6")==0||reg.compare("sp")==0){
        return 6;
    }
    else if(reg.compare("r7")==0||reg.compare("pc")==0){
        return 7;
    }
    else if(reg.compare("psw")==0){
        return 8;
    }
    else{
        return -1;
    }

}


int parseARI(string ariType){
    if(ariType.compare("add")==0){
        return ADD_CODE;
    }
    else if(ariType.compare("sub")==0){
        return SUB_CODE;
    }
    else if(ariType.compare("mul")==0){
        return MUL_CODE;
    }
    else if(ariType.compare("div")==0){
        return DIV_CODE;
    }
    else if(ariType.compare("cmp")==0){
        return CMP_CODE;
    }
    else{
        return -1;
    }
}


int parseLOG(string logType){
    if(logType.compare("not")==0){
        return NOT_CODE;
    }
    else if(logType.compare("and")==0){
        return ADD_CODE;
    }
    else if(logType.compare("or")==0){
        return OR_CODE;
    }
    else if(logType.compare("xor")==0){
        return XOR_CODE;
    }
    else if(logType.compare("test")==0){
        return TEST_CODE;
    }
    else{
        return -1;
    }
}

int parseMOV(string movType){
    if(movType.compare("shl")==0){
        return SHL_CODE;
    }
    else if(movType.compare("shr")==0){
        return SHR_CODE;
    }
    else{
        return -1;
    }
}


int parseJMP(string jmpType){
    if(jmpType.compare("jmp")==0){
        return JMP_CODE;
    }
    else if(jmpType.compare("jeq")==0){
        return JEQ_CODE;
    }
    else if(jmpType.compare("jne")==0){
        return JNE_CODE;
    }
    else if(jmpType.compare("jgt")==0){
        return JGT_CODE;
    }
    else{
        return -1;
    }
}




//HELPERS

void writeToSection(string section,sectionTable* Sections,int value){
            char upper  = value&0xFF;
            if(Sections->addCode(section,upper)!=0){
                cout<<"cannot write to section: "<<section<<endl;
                exit(1);
            };
}

