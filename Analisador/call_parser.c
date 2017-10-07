#include <stdlib.h>
#include <stdio.h>
#include "y.tab.h"
#include "serverlib.h"

int main(int argc, char** argv)
{
	int r , i , j , sz ;
	char *req;
	struct command_list * result = NULL;

	printf("\nStarting program to call the parser and process a request...\n\n");
	FILE * fin;

	/* argv[1] -> Arquivo contendo a requisicao */
	if((fin = fopen(argv[1], "r")) == NULL){
		printf("Error to open file %s.\n", argv[1]);
		exit (0);
	}

	/* Leitura de dados e escrita no buffer */
	fseek(fin, 0, SEEK_END);
	sz = (int) ftell(fin);
	req = ( char *) malloc(sz+1);
	fseek(fin, 0, SEEK_SET);
	fread (req, 1, sz, fin);
	req[sz] = '\0 ';

	/* Processando a requisicao no buffer "req" usando o parser */
	yy_scan_string(req);
	yyparse();

	/* Funcao para obter resultado do parser */
	result = symtab_get_parse_result();
	getOutput(result);

	/* Fechando o arquivo... */
	close(fin);

	printf("\nFinished!\n");
}
