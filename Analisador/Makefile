
all: gcc yacc flex

yacc: aula4.y
	yacc -d aula4.y

flex: aula4.l
	flex aula4.l

gcc: yacc flex
	gcc -o aula4 y.tab.c lex.yy.c -ly -ll -w
