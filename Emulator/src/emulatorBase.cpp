#include "../inc/emulatorBase.h"
#include <iomanip>
#include <numeric>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

#define R0 0
#define R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5

#define SP 6
#define PC 7
#define PSW 8

#define XCHG_CODE 0x60
//ARI CODES
#define ARI_CODE 0x70
#define ADD_CODE 0x70
#define SUB_CODE 0x71
#define MUL_CODE 0x72
#define DIV_CODE 0x73
#define CMP_CODE 0x74
//LOG_CODES
#define LOG_CODE 0x80
#define NOT_CODE 0x80
#define AND_CODE 0x81
#define OR_CODE 0x82
#define XOR_CODE 0x83
#define TEST_CODE 0x84
//MOVE CODES
#define MOV_CODE 0x90
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


//TEST GROUP 1

int emulatorBase::initMem(sectionTable t){


    for (list<sectionTable::section>::iterator i = t.sections.begin(); i != t.sections.end(); i++)
    {
        for (int j = 0; j < i->size; j++)
        {
            if((i->start+j)>0xFFFF){
                cout<<"cant init mem"<<endl;
                exit(-1);
            }
            mem[i->start+j]=(i->code[j]);
        }
        
    }
    
    return 0;
}

void emulatorBase::debugPrintMem(){
    for (int i = 0; i < 0x0100; i++)
    {
        if(i==0){
            cout<<hex<<setfill('0')<<setw(4)<<0x0000<<" : "<<setw(2);
        }

        
        printf("%02hhx ",mem[i]);

        if(i%8==7){
            cout<<endl<<hex<<setfill('0')<<setw(4)<<(i+1)<<" : "<<setw(2);
        }
    }
    
}

termios t;

void retT(){
        tcsetattr(STDIN_FILENO,TCSAFLUSH,&t);
};

int emulatorBase::parse(){
    bool halted=false;
    bool checked=false;
    auto start = chrono::system_clock::now();

    uint8_t iniPCHigh=mem[0x0001];
    uint8_t iniPCLow=mem[0x0000];

    regs[PC].value=(iniPCHigh<<8)|iniPCLow;

    

    if(tcgetattr(STDIN_FILENO,&t)<0){
        return -1;
    };
    
    atexit(retT);
    
    termios nov=t;

    nov.c_lflag&=~ECHO;
    nov.c_lflag&=~ECHONL;
    nov.c_lflag&=~ICANON;
    nov.c_lflag&=~IEXTEN;

    //vreme ispisa
    nov.c_cc[VMIN]=0;
    nov.c_cc[VTIME]=0;

    //velicina citanja sa teminala
    nov.c_cflag&=~CSIZE;
    nov.c_cflag&=~PARENB;

    //velicina 8 bitova
    nov.c_cflag|=~CS8;
    
    if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&nov)>0){
        return -1;
    }


    while (!halted)
    {

        // cout<<hex<<setw(4)<<setfill('0')<<regs[PC].value<<" ";
        int8_t regD;
        int8_t regS;
        int16_t movement;
        int8_t regB;
        char addrType;
        uint16_t operand=0;
        int16_t temp2=0;
        uint8_t dataLow;
        uint8_t dataHigh;

        bool termReq=false;
        char inputC;


        if(read(STDIN_FILENO,&inputC,1)==1){
            mem[0xFF02]=inputC;
            termReq=true;
        }

        if(getINT()&&getT1()&&termReq){
            regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PC].value>>8);
                regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PC].value&0xFF);
            
                //push PSW
                regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PSW].value>>8);
                regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PSW].value&0xFF);

                //mask intr
                clINT();

                //jump
                regs[PC].value=readWordMem(0x0006);
                termReq=false;
        }

        //load diff
        //timer 
            //cout<<regs[PSW].value<<endl;
            //if(regs[PSW].value!=0xFFF0&&regs[PSW].value!=0x7FF0)return(-1);
        if(getTr()&&getINT()){
            
            chrono::duration<double> delta = chrono::system_clock::now()- start ;
            uint16_t mode = readWordMem(0xFF10);
            double diff=0;
            switch (mode)
            {
            case 0x0:
                diff=0.5;
                break;
            case 0x1:
                diff=1;
                break;
            case 0x2:
                diff=1.5;
                break;
            case 0x3:
                diff=2.0;
                break;
            case 0x4:
                diff=5.0;
                break;
            case 0x5:
                diff=10.0;
                break;
            case 0x6:
                diff=30.0;
                break;
            case 0x7:
                diff=60.0;
                break;
            
            default:
                diff=0.5;
                break;
            }


            if(!checked){
            start = std::chrono::system_clock::now();
            delta=start-start;
            checked=true;
            }

            //cout<<regs[PSW].value<<endl;
            //if(regs[PSW].value!=0xFFF0)exit(-1);

            if(delta.count()>=diff){
                checked=false;
                //mask intr
                //clTr();

                //push PC
                regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PC].value>>8);
                regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PC].value&0xFF);
            
                //push PSW
                regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PSW].value>>8);
                regs[SP].value--;
                mem[regs[SP].value]=(char)(regs[PSW].value&0xFF);

                //mask intr
                clINT();

                //jump
                regs[PC].value=readWordMem(0x0004);

                
                //cout<<"ddddddd"<<endl;
            }
        }

        //cout<<hex<<setw(4)<<setfill('0')<<regs[PC].value<<" ";

        int8_t cur_b = (int)readPC();
         //cout<<hex<<setw(2)<<setfill('0')<<(int)cur_b<<" ";
        uint8_t temp = cur_b&0xF0;
        //cout<<hex<<setw(2)<<setfill('0')<<(int)temp<<endl;
        

        switch (temp)
        {
        case XCHG_CODE:
            //already atomic 
            //push interupts/save intr

            regB=readPC();
            
            regD = (regB&0xF0)>>4;
            regS = (regB&0x0F);

            if(regD<0||regD>8||regS<0||regS>8){
                cout<<"xchg unknown register: "<< hex<<regB<<dec<<endl;
                exit(-1);
            }

            temp2 = regs[regD].value;
            regs[regD].value=regs[regS].value;
            regs[regS].value=temp2;
             
            //return interupts
            break;
        case ARI_CODE:
            parseARI(cur_b);
            break;
        case LOG_CODE:
            parseLOG(cur_b);
            break;
        case MOV_CODE:
            parseMOV(cur_b);
            break;
        case JMP_CODE:
            parseJMP(cur_b);
            break;
        case LD_CODE:
            regB=readPC();

            regD = (regB&0xF0)>>4;
            regS = (regB&0x0F);

            if(regD<0||regD>8||regS<0||regS>8){
                cout<<"ld unknown register: "<< hex<<regB<<dec<<endl;
                exit(-1);
            }
            
            addrType = readPC();

            dataHigh=-1;
            dataLow=-1;


            if((addrType&0x0F)!=1&&(addrType&0x0F)!=2){
                dataHigh=readPC();
                dataLow=readPC();
            }

            switch (addrType)
            {
            case (char)0x00://imm
                operand=((dataHigh<<8)|(dataLow));
                break;
            case (char)0x04://mem/indirect
               // operand = readWordMem((int16_t)((((int16_t)dataHigh)<<8)|dataLow));
                operand=((dataHigh<<8)|(dataLow));
                break;
            case (char)0x03://reg ind 16bit mov
                operand=regs[regS].value+(int16_t)(((int16_t)dataHigh<<8)|(dataLow));
                //operand=readWordMem(movement);
                break;
            case (char)0x01://reg dir
                operand=(uint16_t)regs[regS].value;
                break;
            case (char)0x02://reg ind
                operand=readWordMem(regs[regS].value);
                break;
            case (char)0x42://stack pop postdec +4 stack
                dataLow=(int8_t)mem[regs[regS].value];
                dataHigh=(int8_t)mem[regs[regS].value+1];
                regs[regS].value+=2;//4bytes read;
                operand=((int16_t)dataHigh<<8)|dataLow;
                break;
            default:
                //cout<<"unknown addrType"<<hex<<setw(2)<<addrType<<dec<<endl;
                intrError();
                break;
            }

            if(addrType==(char)0x00||addrType==(char)0x01){
                regs[regD].value=operand;
            }else{
            regs[regD].value=readWordMem(operand);
            }

            break;
        case ST_CODE:
            //cout<<hex<<regs[PC].value<<dec<<endl;
            regB=readPC();

            regD = (regB&0xF0)>>4;
            regS = (regB&0x0F);

            if(regD<0||regD>8||regS<0||regS>8){
                cout<<"st unknown register: "<< hex<<regB<<dec<<endl;
                exit(-1);
            }
            
            addrType = readPC();

            dataHigh=-1;
            dataLow=-1;


            if((addrType&0x0F)!=1&&(addrType&0x0F)!=2){
                dataHigh=readPC();
                dataLow=readPC();
            }

            
            switch (addrType)
            {
            case (char)0x04://mem/indirect
                //operand = readWordMem((int)((((int16_t)dataHigh)<<8)|dataLow));
                operand=(uint16_t)(((int16_t)dataHigh<<8)|(dataLow));
                break;
            case (char)0x03://reg ind 16bit mov
                operand=(int16_t)regs[regS].value+(int16_t)(((int16_t)dataHigh<<8)|(dataLow));
                //operand=readWordMem(movement);
                break;
            case (char)0x01://reg dir
                operand=regs[regS].value;
                break;
            case (char)0x02://reg ind
                operand=readWordMem(regs[regS].value);
                break;
            case (char)0x12://stack push -4 stack
                // mem[regs[regS].value-1]=(char)((regs[regD].value&0xFF00)>>8);
                // mem[regs[regS].value-2]=(char)(regs[regD].value&0xFF);
                operand=regs[regS].value-2;
                regs[regS].value-=2;//4bytes read;
                break;
            default:
                //cout<<"unknown addrType"<<hex<<setw(2)<<addrType<<dec<<endl;
                intrError();
                break;
            }
            


            //regs[regD].value=readWordMem(operand);
            //little endian
            if(operand==0xFF00){
                cout<<(char)regs[regD].value<<flush;
            }

            mem[operand]=(char)(regs[regD].value&0xFF);
            mem[operand+1]=(char)((regs[regD].value&0xFF00)>>8);
            

            break;
        case CALL_CODE:
            //push pc
            regs[SP].value--;
            mem[regs[SP].value]=(char)(regs[PC].value>>8);
            regs[SP].value--;
            mem[regs[SP].value]=(char)(regs[PC].value&0xFF);


            parseJMP(JMP_CODE);
            break;
        case RET_CODE:
            //pop PC
            dataLow=(int8_t)mem[regs[SP].value];
            regs[SP].value++;
            dataHigh=(int8_t)mem[regs[SP].value];
            regs[SP].value++;

            regs[PC].value=((int16_t)dataHigh<<8)|dataLow;
            break;
        case HALT_CODE:
            halted=true;
            break;
        case INTR_CODE:
            //push PC
            regs[SP].value--;
            mem[regs[SP].value]=(char)(regs[PC].value>>8);
            regs[SP].value--;
            mem[regs[SP].value]=(char)(regs[PC].value&0xFF);
            
            //push PSW
            regs[SP].value--;
            mem[regs[SP].value]=(char)(regs[PSW].value>>8);
            regs[SP].value--;
            mem[regs[SP].value]=(char)(regs[PSW].value&0xFF);

            //maskIntr
            clINT();

            regB=readPC();

            regD = (regB&0xF0)>>4;//signed or unsigned ?

            if(regD<0||regD>8){
                cout<<"intr unknown register: "<< hex<<regB<<dec<<endl;
                exit(-1);
            }            

            regs[PC].value=mem[((uint16_t)regD%8)*2];

            break;
        case IRET_CODE:
            //pop psw
            dataLow=(int8_t)mem[regs[SP].value];
            regs[SP].value++;
            dataHigh=(int8_t)mem[regs[SP].value];
            regs[SP].value++;

            regs[PSW].value=((int16_t)dataHigh<<8)|dataLow;
            
             //pop PC
            dataLow=(int8_t)mem[regs[SP].value];
            regs[SP].value++;
            dataHigh=(int8_t)mem[regs[SP].value];
            regs[SP].value++;

            regs[PC].value=((int16_t)dataHigh<<8)|dataLow;
            //cout<<hex<<regs[PC].value<<dec<<endl;


            break;
        default:
            intrError();
            break;
        }

    }
    return 0;
}

void emulatorBase::parseARI(int cur_b){
    char regB=readPC();
    
    int8_t regD = (regB&0xF0)>>4;
    int8_t regS = (regB&0x0F);
    int16_t a=0;
    int16_t b=0;
    int16_t temp;

    if(regD<0||regD>8||regS<0||regS>8){
        cout<<"ari unknown register: "<< hex<<regB<<dec<<endl;
        exit(-1);
    }

    switch (cur_b)
    {
    case ADD_CODE:
        regs[regD].value=regs[regD].value+regs[regS].value;
        break;
    case SUB_CODE:
        regs[regD].value=regs[regD].value-regs[regS].value;
        break;
    case MUL_CODE:
        regs[regD].value=regs[regD].value*regs[regS].value;
        break;
    case DIV_CODE:
        regs[regD].value=regs[regD].value/regs[regS].value;
        break;
    case CMP_CODE:
        temp = (int16_t)regs[regD].value-(int16_t)regs[regS].value;
        if(temp==0){
            setZ(1);
        }
        else{
            setZ(0);
        }

        if(temp<0){
            setN(1);
            setC(1);
        }
        else{
            setC(0);
            setN(0);
        }

        a=regs[regD].value;
        b=regs[regS].value;
        

        if((a>0&&b<0&&temp<0)||
        (a<0&&b>0&&temp>0)){
            setO(1);
        }
        else{
            setO(0);
        }
        

        break;

    default:
        //cout<<"unknown function code:" <<hex<< cur_b<<dec<<endl;
        //exit(-1);
        intrError();
        break;
    }
};

void emulatorBase::parseLOG(int cur_b){
    char regB=readPC();
    
    int8_t regD = (regB&0xF0)>>4;
    int8_t regS = (regB&0x0F);
    int16_t temp;

    if(regD<0||regD>8||regS<0||regS>8){
        cout<<"log unknown register: "<< hex<<regB<<dec<<endl;
        exit(-1);
    }


    switch (cur_b)
    {
    case NOT_CODE:
        regs[regD].value=~regs[regD].value;
        break;
    case AND_CODE:
        regs[regD].value=regs[regD].value&regs[regS].value;
        break;
    case OR_CODE:
        regs[regD].value=regs[regD].value|regs[regS].value;
        break;
    case XOR_CODE:
        regs[regD].value=regs[regD].value^regs[regS].value;
        break;
    case TEST_CODE:
        temp =regs[regD].value&regs[regS].value;
        //Check Z N
        if(temp==0){
            setZ(1);
            setN(0);
        }
        else if(temp<0){
            setZ(0);
            setN(1);
        }
        else{
            setN(0);
            setZ(0);
        }
        break;

    default:
        // cout<<"unknown function code:" <<hex<< cur_b<<dec<<endl;
        // exit(-1);
        intrError();
        break;
    }
};

void emulatorBase::parseMOV(int cur_b){
       char regB=readPC();
    
    int8_t regD = (regB&0xF0)>>4;
    int8_t regS = (regB&0x0F);

    if(regD<0||regD>8||regS<0||regS>8){
        cout<<"mov unknown register: "<< hex<<regB<<dec<<endl;
        exit(-1);
    }

    switch (cur_b)
    {
    case SHL_CODE:
        //Check C
        if(regs[regD].value&0x8000){
            setC(1);
        }
        else{
            setC(0);
        }

        regs[regD].value=regs[regD].value<<regs[regS].value;

        //Check Z
        if(regs[regD].value==0){
            setZ(1);
        }
        else{
            setZ(0);
        }
        //Check N
        if(regs[regD].value<0){
            setN(1);
        }
        else{
            setN(0);
        }

        break;
    case SHR_CODE:
        //Check C
        if(regs[regD].value&0x0001){
            setC(1);
        }
        else{
            setC(0);
        }

        regs[regD].value=regs[regD].value>>regs[regS].value;
        
        //Check Z
        if(regs[regD].value==0){
            setZ(1);
        }
        else{
            setZ(0);
        }
        //Check N
        if(regs[regD].value<0){
            setN(1);
        }
        else{
            setN(0);
        }

        break;

    default:
        // cout<<"unknown function code:" <<hex<< cur_b<<dec<<endl;
        // exit(-1);
        intrError();
        break;
    }
};

void emulatorBase::parseJMP(int cur_b){
    int8_t regB=readPC();
    
    int8_t regS = (regB&0x0F);

    if(regS<0||regS>8){
        cout<<"ari unknown register: "<< hex<<regB<<dec<<endl;
        exit(-1);
    }

    char addrType=readPC();
    
    uint8_t dataHigh=-1;
    uint8_t dataLow=-1;
    int16_t operand=0;
    int16_t movement;

    if((addrType&0x0F)!=1&&(addrType&0x0F)!=2){
        dataHigh=readPC(); //cout<<"dhigh "<< (int)(dataHigh)<<" ";
        dataLow=readPC(); //cout<<"dlow: "<< (int)(dataLow)<<" ";
        //cout<<"here "<<((dataHigh)<<8|dataLow)<<" "<<regs[SP].value<<" ";
    }

    //cout<<hex<<setw(2)<<(int)addrType<<endl;

    switch (addrType)
            {
            case (char)0x00://imm
                operand=((dataHigh<<8)|(dataLow));
                break;
            case (char)0x04://mem/indirect
                operand = readWordMem((int16_t)(((int16_t)(dataHigh)<<8)|dataLow));
                break;
            case (char)0x05://reg dir 16bit mov
                operand=regs[regS].value+(int16_t)(((int16_t)dataHigh<<8)|(dataLow));
                break;
            case (char)0x03://reg ind 16bit mov
                operand=regs[regS].value+(int16_t)(((int16_t)dataHigh<<8)|(dataLow));
                operand=readWordMem(movement);
                break;
            case (char)0x01://reg dir
                operand=regs[regS].value;
                break;
            case (char)0x02://reg ind
                operand=readWordMem(regs[regS].value);
                break;
            default:
                //cout<<"unknown addrType"<<hex<<setw(2)<<addrType<<dec<<endl;
                intrError();
                break;
            }


    switch (cur_b)
    {
    case JMP_CODE:
        regs[PC].value=operand;
        break;
    case JEQ_CODE:
        if(getEQ())regs[PC].value=operand;
        break;
    case JNE_CODE:
        if(getNEQ())regs[PC].value=operand;
        break;
    case JGT_CODE:
        if(getGT())regs[PC].value=operand;
        break;
    
    default:
        // cout<<"unknown function code:" <<hex<< cur_b<<dec<<endl;
        // exit(-1);
        intrError();
        break;
    }
};


emulatorBase::emulatorBase(){
    for(int i=0;i<9;i++){
        Register temp;
        temp.rb=i;
        temp.value=0;
        regs.push_back(temp);
    }

    regs[SP].value=0xFEFF;
    regs[PSW].value=0x0FF0;

    for (int i = 0; i < 0x10000; i++)
    {
        mem[i]=0;
    }

}

emulatorBase::~emulatorBase(){};

// END OF TEST GROUP 1


//TEST GROUP 2

void emulatorBase::setPC(int16_t i){
    regs[PC].value=i;
}

void emulatorBase::incPC(){
    regs[PC].value++;
}

char emulatorBase::readPC(){
    
    char cur_b = mem[regs[PC].value];
    incPC();
    return cur_b;
}

void emulatorBase::setZ(int z){
    
    if(z==1){
    unsigned short mask = 0x0001;

    regs[PSW].value=regs[PSW].value|mask;
    }
    else{
        unsigned short mask = 0xFFFE;

        regs[PSW].value=regs[PSW].value&mask;
    }
};
void emulatorBase::setC(int c){
    if(c==1){
    unsigned short mask = 0x0004;

    regs[PSW].value=regs[PSW].value|mask;
    }
    else{
        unsigned short mask = 0xFFFB;

        regs[PSW].value=regs[PSW].value&mask;
    }


};
void emulatorBase::setN(int n){
    if(n==1){
    unsigned short mask = 0x0008;

    regs[PSW].value=regs[PSW].value|mask;
    }
    else{
        unsigned short mask = 0xFFF7;

        regs[PSW].value=regs[PSW].value&mask;
    }

};
void emulatorBase::setO(int o){
    if(o==1){
    unsigned short mask = 0x0002;

    regs[PSW].value=regs[PSW].value|mask;
    }
    else{
        unsigned short mask = 0xFFFD;

        regs[PSW].value=regs[PSW].value&mask;
    }

};

bool emulatorBase::getZ(){
    return regs[PSW].value&0x0001;
};
bool emulatorBase::getC(){
    return regs[PSW].value&0x0004;
};
bool emulatorBase::getN(){
    return regs[PSW].value&0x0008;
};
bool emulatorBase::getO(){
    return regs[PSW].value&0x0002;
};

bool emulatorBase::getEQ(){
    bool Z = getZ();
    bool O = getO();
    bool C = getC();
    bool N = getN();

    return Z;
};
bool emulatorBase::getNEQ(){
    bool Z = getZ();
    bool O = getO();
    bool C = getC();
    bool N = getN();

    return !Z;
};
bool emulatorBase::getGT(){
    bool Z = getZ();
    bool O = getO();
    bool C = getC();
    bool N = getN();

    return !(N^O)&!Z;
};

uint16_t emulatorBase::readWordMem(uint16_t adr){
    int firstByte=mem[adr];
    int secondByte=mem[adr+1];
    int operand=(int)((secondByte<<8)|(firstByte));
    return operand;
}


// END OF TEST GROUP 2

//TEST GROUP 3

void emulatorBase::setINT(){
    regs[PSW].value&=0x7FFF; 
}


void emulatorBase::setT1(){
    regs[PSW].value&=0xBFFF; 
}


void emulatorBase::setTr(){
    regs[PSW].value&=0xDFFF; 
}

void emulatorBase::clINT(){
    regs[PSW].value|=0x8000; 
}


void emulatorBase::clT1(){
    regs[PSW].value|=0x4000; 
}


void emulatorBase::clTr(){
    regs[PSW].value|=0x2000; 
}


bool emulatorBase::getINT(){   
    return ((regs[PSW].value&0x8000)==0);
}

bool emulatorBase::getT1(){   
    return ((regs[PSW].value&0x4000)==0);
}

bool emulatorBase::getTr(){   
    return ((regs[PSW].value&0x2000)==0);
}

// END OF TEST GROUP 3

//TEST GROUP 4

void emulatorBase::intrError(){
    regs[SP].value--;
    mem[regs[SP].value]=(char)(regs[PC].value>>8);
    regs[SP].value--;
    mem[regs[SP].value]=(char)(regs[PC].value&0xFF);
            
    //push PSW
    regs[SP].value--;
    mem[regs[SP].value]=(char)(regs[PSW].value>>8);
    regs[SP].value--;
    mem[regs[SP].value]=(char)(regs[PSW].value&0xFF);

    //mask intr
    clINT();

    //jump
    regs[PC].value=readWordMem(0x0002);
}

//END OF TEST GROUP 4