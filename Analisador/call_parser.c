#include <stdlib.h>
#include <stdio.h>
#include "y.tab.h"
#include "serverlib.h"

int main(int argc, char** argv)
{
	int r , i , j , sz ;
	char *req;
	struct command_list * result = NULL;

	printf("Starting program to call the parser and process a request... \n");
	FILE * fin, *fout;

	/* argv[1] -> Arquivo contendo a requisicao */
	if((fin = fopen(argv[1], "r")) == NULL){
		printf("Error to open file %s.\n", argv[1]);
		exit (0);
	}
	if((fout = fopen(argv[2], "w")) == NULL){
		printf("Cannot write into %s\n", argv[2]);
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
	if( yyparse() ){
		//requisicao esta mal formada
		getOutput(NULL, fout);
		return 0;
	}

	/* Funcao para obter resultado do parser */
	result = symtab_get_parse_result();
	getOutput(result, fout);

	/* Fechando o arquivo... */
	fprintf(fout, "\n\0" );
	close(fin);
	close(fout);

	printf("Finished!\n");
}
