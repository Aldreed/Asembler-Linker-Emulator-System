bisonASM.tab.c bisonASM.tab.h: ./src/bisonASM.y
	bison -d ./src/bisonASM.y

lex.yy.c: ./src/flexASM.l bisonASM.tab.h
	flex ./src/flexASM.l

asembler: lex.yy.c bisonASM.tab.c bisonASM.tab.h ./src/symTable.cpp ./src/sectionTable.cpp ./src/relTable.cpp ./src/cmdManager.cpp ./inc/symTable.h ./inc/sectionTable.h ./inc/relTable.h ./inc/cmdManager.h 
	g++ ./src/symTable.cpp ./src/sectionTable.cpp ./src/relTable.cpp ./src/cmdManager.cpp bisonASM.tab.c lex.yy.c -lfl -o asembler