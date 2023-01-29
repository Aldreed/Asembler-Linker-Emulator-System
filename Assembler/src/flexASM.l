%{
  #include <cstdio>

  #include "./bisonASM.tab.h"  // to get the token types from Bison
  extern "C" int yylex();

%}

jumps jmp|jeq|jne|jgt
ariOp add|sub|mul|div|cmp
logOp and|or|xor|test
movOp shl|shr
reg r0|r1|r2|r3|r4|r5|r6|sp|r7|pc|psw


%%
\.global           { return GLOBAL; }
\.extern           { return EXTERN; }
\.section          { return SECTION; }
\.word             { return WORD; }
\.skip             { return SKIP; }
\.equ              { return EQ; }
\.end(.|\n)*       { return END; }
\n                 { return ENDL; }
,                  { return ','; }
:                  { return ':'; }
\+                 { return '+'; }
\%                 { return '%'; }
\[                 { return '['; }
\]                 { return ']'; }
\*                 { return '*'; }
\$                 { return '$'; }
halt               { return HALT; }
int                { return INTR; }
iret               { return IRET; }
call               { return CALL; }
ret                { return RET; }
ldr                { return LDR; }
str                { return STR; }
push               { return PUSH; }
pop                { return POP; }
xchg               { return XCHG; }
{jumps}            { yylval.opval = strdup(yytext); return JUMP; }
{ariOp}            { yylval.opval = strdup(yytext); return ARIOP; }
{logOp}            { yylval.opval = strdup(yytext); return LOGOP; }
not                { yylval.opval=strdup(yytext); return NOT; }
{movOp}            { yylval.opval = strdup(yytext); return MOVOP; }
{reg}              { yylval.regval = strdup(yytext); return REG; }
[ \t]           ;
#.*             ;
[0-9]+\.[0-9]+    { yylval.fval = atof(yytext); return FLOAT; }
[0-9]+            { yylval.ival = atoi(yytext); return INT; }
0x[0-9FEDCBAfedcba]{1,4} {yylval.ival = strtol(yytext,NULL,16); return INT;}
[a-zA-Z0-9`_\.]+      {
  yylval.sval = strdup(yytext);
  return STRING;
}
%%
