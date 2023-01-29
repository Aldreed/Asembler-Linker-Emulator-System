
%{
  #include <cstdio>
  #include <iostream>
  #include <cstring>
  #include <fstream>
  #include "./inc/cmdManager.h"
  using namespace std;


  extern "C" int yylex();
  extern int yyparse();
  extern FILE *yyin;
 
  void yyerror(const char *s);
  
  ofstream output;
  int offset;
  string cur_section;
  symTable* g_sym;
  sectionTable* g_sec;
  relTable* g_rel;
  bool first;
  int lns;
%}

%union {
  long ival;
  float fval;
  char *sval;
  char *opval;
  char *regval;
}

%token GLOBAL
%token EXTERN
%token SECTION
%token WORD
%token SKIP
%token EQ
%token ENDL
%token END
%token HALT
%token INTR
%token IRET
%token CALL
%token RET
%token LDR
%token STR
%token PUSH
%token POP
%token XCHG


%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING
%token <opval> JUMP
%token <opval> ARIOP
%token <opval> LOGOP
%token <opval> NOT
%token <opval> MOVOP
%token <regval> REG
%%

/*
snazzle:
  snazzle INT {
      output << "bison found an int: " << $2 << endl;
    }
  | snazzle FLOAT {
      output << "bison found a float: " << $2 << endl;
    }
  | snazzle STRING {
      output << "bison found a string: " << $2 << endl; free($2);
    }
  | INT {
      output << "bison found an int: " << $1 << endl;
    }
  | FLOAT {
      output << "bison found a float: " << $1 << endl;
    }
  | STRING {
     output << "bison found a string: " << $1 << endl; free($1);
    }
  ;
*/

  //Da li mora da pocne sa nekom sekcijom
asembler:
  body END{
    //cout << "DONE" <<endl;
  }
  | END {
    //cout << "DONE" <<endl;
  }
;
body:
  body HEADER_SECTION ENDSTM SECTION_BODY
  | body ENDSTM
  | body GLOBAL_DIR ENDSTM
  | body EXTERN_DIR ENDSTM
  | body EQ STRING ',' INT ENDSTM{
    //cout << "EQ Symbol: " << $3 << $5 << endl; 
    dataManager temp;
    temp.literal = $5;
    temp.symbol =$3;
    equ(cur_section,offset,first,temp,g_sec,g_sym);
    free($3);
  }
  | GLOBAL_DIR ENDSTM
  | EXTERN_DIR ENDSTM
  | EQ STRING ',' INT ENDSTM{
    //cout << "EQ Symbol: " << $2 << $4 << endl; 
    dataManager temp;
    temp.literal = $4;
    temp.symbol =$2;
    equ(cur_section,offset,first,temp,g_sec,g_sym);
    free($2);
  }
  | ENDSTM
  | HEADER_SECTION ENDSTM SECTION_BODY
;
SECTION_BODY:
  SECTION_BODY GLOBAL_DIR ENDSTM
  | SECTION_BODY EXTERN_DIR ENDSTM
  | SECTION_BODY WORD_DIR ENDSTM
  | SECTION_BODY SKIP INT ENDSTM{
     //cout << "Skip spaces: " << $3 <<endl;
    skip(cur_section,offset,first,$3,g_sec);offset+= $3*1;
  }
  | SECTION_BODY EQ STRING ',' INT ENDSTM{
    //cout << "EQ Symbol: " << $3 << $5 << endl; 
    dataManager temp;
    temp.literal = $5;
    temp.symbol =$3;
    equ(cur_section,offset,first,temp,g_sec,g_sym);
    free($3);
  }
  | SECTION_BODY ARIT_OP ENDSTM
  | SECTION_BODY LOGI_OP ENDSTM
  | SECTION_BODY MOVE_OP ENDSTM
  | SECTION_BODY JUMP_OP ENDSTM
  | SECTION_BODY HALT ENDSTM { halt(cur_section,offset,first,g_sec); offset+=1; }
  | SECTION_BODY INTR REG ENDSTM { intr(cur_section,offset,first,$3,g_sec,g_sym,g_rel);offset+=2; }
  | SECTION_BODY IRET ENDSTM { iret(cur_section,offset,first,g_sec); offset+=1; }
  | SECTION_BODY CALL_OP ENDSTM
  | SECTION_BODY RET ENDSTM { ret(cur_section,offset,first,g_sec); offset+=1; }
  | SECTION_BODY PUSH REG ENDSTM { dataManager temp; temp.regD=$3; temp.type=10; str(cur_section,offset,first,temp,g_sec,g_sym,g_rel); offset+=3; }
  | SECTION_BODY POP REG ENDSTM  { dataManager temp; temp.regD=$3; temp.type=10; ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel); offset+=3; }
  | SECTION_BODY XCHG REG ',' REG ENDSTM { 
    dataManager temp;
    temp.regD=$3;
    temp.regS=$5;
    xchg(cur_section,offset,first,temp,g_sec);
    offset+=2; }
  | SECTION_BODY STR_OP ENDSTM
  | SECTION_BODY LDR_OP ENDSTM
  | SECTION_BODY STRING ':' ENDSTM {label(cur_section,offset,first,$2,g_sec,g_sym);}
  | GLOBAL_DIR ENDSTM
  | EXTERN_DIR ENDSTM
  | WORD_DIR ENDSTM
  | SKIP INT ENDSTM{
     //cout << "Skip spaces: " << $2 <<endl;
    skip(cur_section,offset,first,$2,g_sec);offset+= $2*1;
  }
  | EQ STRING ',' INT ENDSTM{
    //cout << "EQ Symbol: " << $2 << $4 << endl; 
    dataManager temp;
    temp.literal = $4;
    temp.symbol =$2;
    equ(cur_section,offset,first,temp,g_sec,g_sym);
    free($2);
  }
  | ARIT_OP ENDSTM
  | LOGI_OP ENDSTM
  | MOVE_OP ENDSTM
  | JUMP_OP ENDSTM
  | HALT ENDSTM { halt(cur_section,offset,first,g_sec); offset+=1; }
  | INTR REG ENDSTM { intr(cur_section,offset,first,$2,g_sec,g_sym,g_rel); offset+=2; }
  | IRET ENDSTM { iret(cur_section,offset,first,g_sec); offset+=1; }
  | CALL_OP ENDSTM
  | RET ENDSTM { ret(cur_section,offset,first,g_sec); offset+=1; }
  | PUSH REG ENDSTM { dataManager temp; temp.regD=$2; temp.type=10; str(cur_section,offset,first,temp,g_sec,g_sym,g_rel); offset+=3; }
  | POP REG ENDSTM  { dataManager temp; temp.regD=$2; temp.type=10; ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel); offset+=3; }
  | XCHG REG ',' REG ENDSTM { 
    dataManager temp;
    temp.regD=$2;
    temp.regS=$4;
    xchg(cur_section,offset,first,temp,g_sec);
    offset+=2; }
  | STR_OP ENDSTM 
  | LDR_OP ENDSTM
  | STRING ':' ENDSTM {label(cur_section,offset,first,$1,g_sec,g_sym);}
  | ENDSTM
;
GLOBAL_DIR:
  GLOBAL_DIR ',' STRING{
    //cout << "Global symbol: "<< $3 << endl; 
    global(cur_section,offset,first,$3,g_sec,g_sym);
    free($3);
  }
  | GLOBAL STRING{
  //cout << "Global symbol: "<< $2 << endl; 
  global(cur_section,offset,first,$2,g_sec,g_sym);
  free($2);
  } 
;
EXTERN_DIR:
  EXTERN_DIR ',' STRING{
    //cout << "EXTERN symbol: "<< $3 << endl; 
    exter(cur_section,offset,first,$3,g_sym);
    free($3);
  }
  | EXTERN STRING{
  //cout << "EXTERN symbol: "<< $2 << endl; 
  exter(cur_section,offset,first,$2,g_sym);
  free($2);
  } 
;
WORD_DIR:
  WORD_DIR ',' STRING{
    //cout << "WORD skip: "<< $3 << endl; //cout<<offset<<endl;
    dataManager temp;
    temp.symbol =$3;
    temp.type=0;
    word(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=2;
    free($3);
  }
  | WORD_DIR ',' INT{
    //cout << "WORD skip: "<< $3 << endl;  //cout<<offset<<endl;
    dataManager temp;
    temp.literal =$3;
    temp.type=1;
    word(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=2;
  }
  | WORD STRING{
  //cout << "WORD skip: "<< $2 << endl; //cout<<offset<<endl;
  dataManager temp;
    temp.symbol =$2;
    temp.type=0;
    word(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    free($2); offset+=2;
  }
  | WORD INT{
  //cout << "WORD skip: "<< $2 << endl;  //cout<<offset<<endl;
  dataManager temp;
    temp.literal =$2;
    temp.type=1;
    word(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=2;
  }
;
HEADER_SECTION:
  SECTION STRING{
    //cout << "Sekcija: "<< $2 <<endl; 
    cur_section=$2; 
    offset=0;
    section(first,$2,g_sec,g_sym);
    free($2); 
  }
;
JUMP_OP:
  JUMP INT {
    dataManager temp;
    temp.literal= $2;
    temp.type=1;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | JUMP STRING {
    dataManager temp;
    temp.symbol= $2;
    temp.type=2;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=5;}
  | JUMP '%' STRING {
    dataManager temp;
    temp.symbol= $3;
    temp.type=3;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | JUMP '*' INT  {
    dataManager temp;
    temp.literal= $3;
    temp.type=4;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | JUMP '*' STRING {
    dataManager temp;
    temp.symbol= $3;
    temp.type=5;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | JUMP '*' REG {
    dataManager temp;
    temp.regD= $3;
    temp.type=6;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=3;
  }
  | JUMP '*' '[' REG ']' {
    dataManager temp;
    temp.regD= $4;
    temp.type=7;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=3;}
  | JUMP '*' '[' REG '+' INT ']' {
    dataManager temp;
    temp.regD= $4;
    temp.literal=$6;
    temp.type=8;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=5;}
  | JUMP '*' '[' REG '+' STRING ']' {
    dataManager temp;
    temp.regD= $4;
    temp.symbol=$6;
    temp.type=9;
    jmp(cur_section,offset,first,temp,$1,g_sec,g_sym,g_rel);
    offset+=5;}
;
CALL_OP:
  CALL INT {
    dataManager temp;
    temp.literal= $2;
    temp.type=1;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | CALL STRING {
    dataManager temp;
    temp.symbol= $2;
    temp.type=2;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | CALL '%' STRING {
    dataManager temp;
    temp.symbol= $3;
    temp.type=3;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | CALL '*' INT {
    dataManager temp;
    temp.literal= $3;
    temp.type=4;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | CALL '*' STRING {
    dataManager temp;
    temp.symbol= $3;
    temp.type=5;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | CALL '*' REG {
    dataManager temp;
    temp.regD= $3;
    temp.type=6;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=3;
  }
  | CALL '*' '[' REG ']'  {
    dataManager temp;
    temp.regD= $4;
    temp.type=7;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=3;
  }
  | CALL '*' '[' REG '+' INT ']' {
    dataManager temp;
    temp.regD= $4;
    temp.literal=$6;
    temp.type=8;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | CALL '*' '[' REG '+' STRING ']' {
    dataManager temp;
    temp.regD= $4;
    temp.symbol=$6;
    temp.type=9;
    call(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
;
ARIT_OP:
  ARIOP REG ',' REG {
    dataManager temp;
    temp.regD=$2;
    temp.regS=$4;
    ari(cur_section,offset,first,temp,$1,g_sec);
    offset+=2;}
;
LOGI_OP:
  LOGOP REG ',' REG {
    dataManager temp;
    temp.regD=$2;
    temp.regS=$4;
    log(cur_section,offset,first,temp,$1,g_sec);
    offset+=2;
  }
  | NOT REG {
    dataManager temp;
    temp.regD=$2;
    log(cur_section,offset,first,temp,$1,g_sec);
    offset+=2;
  }
  //not reg sve ostalo treba da se zanemari
;
MOVE_OP:
  MOVOP REG ',' REG {
    dataManager temp;
    temp.regD=$2;
    temp.regS=$4;
    mov(cur_section,offset,first,temp,$1,g_sec);
    offset+=2;
  }
;
LDR_OP:
  LDR REG ',' '$' INT {
    dataManager temp;
    temp.literal=$5;
    temp.regD=$2;
    temp.type=1;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;}
  | LDR REG ',' '$' STRING {
    dataManager temp;
    temp.symbol=$5;
    temp.regD=$2;
    temp.type=2;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | LDR REG ',' INT {
    dataManager temp;
    temp.literal=$4;
    temp.regD=$2;
    temp.type=3;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | LDR REG ',' STRING {
    dataManager temp;
    temp.symbol=$4;
    temp.regD=$2;
    temp.type=4;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | LDR REG ',' '%' STRING {
    dataManager temp;
    temp.symbol=$5;
    temp.regD=$2;
    temp.type=5;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | LDR REG ',' REG {
    dataManager temp;
    temp.regS=$4;
    temp.regD=$2;
    temp.type=6;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=3;}
  | LDR REG ',' '[' REG ']' {
    dataManager temp;
    temp.regS=$5;
    temp.regD=$2;
    temp.type=7;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=3;
  }
  | LDR REG ',' '[' REG '+' INT ']' {
    dataManager temp;
    temp.regS=$5;
    temp.regD=$2;
    temp.literal=$7;
    temp.type=8;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | LDR REG ',' '[' REG '+' STRING ']' {
    dataManager temp;
    temp.regS=$5;
    temp.regD=$2;
    temp.symbol=$7;
    temp.type=9;
    ld(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
;
STR_OP:
  //STR REG ',' '$' INT {offset+=5;}
  //| STR REG ',' '$' STRING {offset+=5;}
  //| STR REG ',' INT {offset+=5;}
  //| STR REG ',' STRING {offset+=5;}
  //| STR REG ',' '%' STRING {offset+=5;}
  //| STR REG ',' REG {offset+=3;}
  //| STR REG ',' '[' REG ']' {offset+=3;}
  //| STR REG ',' '[' REG '+' INT ']' {offset+=5;}
  //| STR REG ',' '[' REG '+' STRING ']' {offset+=5;}
//;
 //STR REG ',' '$' INT {
    //dataManager temp;
    //temp.literal=$5;
    //temp.regD=$2;
    //temp.type=1;
    //str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    //offset+=5;}
  STR REG ',' '$' STRING {
    dataManager temp;
    temp.symbol=$5;
    temp.regD=$2;
    temp.type=2;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | STR REG ',' INT {
    dataManager temp;
    temp.literal=$4;
    temp.regD=$2;
    temp.type=3;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | STR REG ',' STRING {
    dataManager temp;
    temp.symbol=$4;
    temp.regD=$2;
    temp.type=4;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | STR REG ',' '%' STRING {
    dataManager temp;
    temp.symbol=$5;
    temp.regD=$2;
    temp.type=5;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | STR REG ',' REG {
    dataManager temp;
    temp.regS=$4;
    temp.regD=$2;
    temp.type=6;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=3;}
  | STR REG ',' '[' REG ']' {
    dataManager temp;
    temp.regS=$5;
    temp.regD=$2;
    temp.type=7;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=3;
  }
  | STR REG ',' '[' REG '+' INT ']' {
    dataManager temp;
    temp.regS=$5;
    temp.regD=$2;
    temp.literal=$7;
    temp.type=8;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
  | STR REG ',' '[' REG '+' STRING ']' {
    dataManager temp;
    temp.regS=$5;
    temp.regD=$2;
    temp.symbol=$7;
    temp.type=9;
    str(cur_section,offset,first,temp,g_sec,g_sym,g_rel);
    offset+=5;
  }
;
ENDSTM://preko flexa sa \n*
  ENDSTM ENDL {lns++;}
  | ENDL {lns++;}
;
%%

int main(int argc, char** argv) {
  g_rel= new relTable();
  g_sec=new sectionTable(true);
  g_sym=new symTable();
  offset=0;
  first = true;
  char* inputFile;
  string outputFile;
  bool out;
  lns=1;
  if(argc==2){
  	inputFile = argv[1];
  
  	string tempInputFile = argv[1];
  	string tempOutputFile = tempInputFile.substr(0,tempInputFile.find("."));
  	tempOutputFile = tempOutputFile +".o";
  	outputFile= tempOutputFile;  
  	//strcpy(outputFile,tempOutputFile.c_str());	
  }
  else if(argc==4){
  	inputFile = argv[3];
	if(strcmp(argv[1],"-o")==0){
  	outputFile = argv[2];	
    out=true;	
	}
	else{
	cout<< "Calling error"<<endl;
	return -1;
	}  	
  }
  else{
  cout<< "Calling error"<<endl;
  return -1;
  }
  
  
  
  FILE *myfile = fopen(inputFile, "r");
  
  if (!myfile) {
    cout << "File error" << endl;
    return -1;
  }
  
  
  output.open(outputFile);

  yyin = myfile;
 
  yyparse();
  
  fclose(myfile);
  //cout<<offset<<endl;
  //cout<<cur_section<<endl;
  //DRugi prolaz
  first=false;
  offset=0;
  myfile = fopen(inputFile, "r");
  lns=1;

  if (!myfile) {
  cout << "File error" << endl;
    return -1;
  }
  
  yyparse();
  
  fclose(myfile);
  
  output.close();
  //cout<<outputFile<<endl;
  //cout<<"done"<<endl;
  //cout<<offset<<endl;
  //cout<<cur_section<<endl;
  string temp = argv[0];
  temp=temp+".txt";
  ofstream o(temp,ios::trunc);
  o.close();

  g_sec->printTable(temp);
  g_sym->printTable(temp);
  g_rel->printTable(temp);
  
  if(out){
  ofstream binary_output(outputFile,ios::binary|ios::out|ios::trunc);
  g_sec->writeToBinary(&binary_output);
  g_sym->writeToBinary(&binary_output);
  g_rel->writeToBinary(&binary_output);
  binary_output.close();
  }


  delete(g_sym);
  delete(g_sec);
  delete(g_rel);

  ifstream in("binOut.bin",ios::binary|ios::in);
  g_sec=new sectionTable();
  g_sym=new symTable();
  g_rel=new relTable();

  g_sec->readFromBinary(&in);
  g_sym->readFromBinary(&in);
  g_rel->readFromBinary(&in);

  //g_sec->printTable();
  //g_sym->printTable();
  //g_rel->printTable();

  delete(g_sec);
  delete(g_sym);
  delete(g_rel);
  in.close();
}

void yyerror(const char *s) {
  //cout << "EEK, parse error!  Message: " << s << endl;
  // might as well halt now:
  cout<<"Parse error at line: "<<lns<<endl;
  exit(-1);
}
