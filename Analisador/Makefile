
all: gcc yacc flex

yacc: aula8.y
	yacc -d aula8.y

flex: aula8.l
	flex aula8.l

gcc: yacc flex
	gcc -o aula8 y.tab.c lex.yy.c -ly -ll -w
