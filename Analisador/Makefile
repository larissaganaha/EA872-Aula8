#Makefile
all: gcc lib yacc flex

lib: serverlib.c
	gcc -o serverlib.o -c serverlib.c -w

yacc: aula8.y
	yacc -d aula8.y

flex: aula8.l
	flex aula8.l

gcc: lib yacc flex
	gcc -o aula8 call_parser.c y.tab.c lex.yy.c serverlib.o -ly -ll -w
