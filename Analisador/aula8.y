
%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <time.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include "serverlib.h"

    #define CHUNK 1024
    #define ADDRESS_SIZE 500
    //cores de print
    #define RED   "\x1B[31m"
    #define GRN   "\x1B[32m"
    #define YEL   "\x1B[33m"
    #define BLU   "\x1B[34m"
    #define MAG   "\x1B[35m"
    #define CYN   "\x1B[36m"
    #define WHT   "\x1B[37m"
    #define RESET "\x1B[0m"

    command_list *add_command_list(char *command);
    command_list *symtab_get_parse_result();
    void add_param_list_begin(char *param);
    void print_list();
    void separaLogin(char *comandoTxt);

    struct command_list * list = NULL;
    struct command_list * requisicao = NULL;
    //flags de controle
    int comando_detectado = 0;
    int nLinha = 0;
    char comandoTxt[200];
    char paramTxt[200];
    char frase[200];
    char nome[200];
    char senha[200];
    //char nomevalido[200] = "EA872";
    //char senhavalida[200] = "123";
    %}

%union {
    char str[200];
}

%token <str> PALAVRA COMANDO_HTTP PARAMETRO_SP PARAMETRO_UA COMANDO LOGINDATA
%token VIRGULA NL DP INVALIDO FIM_REQ

%type <str> palavra DP parametro parametro_NL comando_HTTP comando_sp

%%

linhas  :   linhas linha
        |   linha


linha   :   comando parametro_final {nLinha++;
                                    comando_detectado = 0;}
            | comando NL            {nLinha++;
                                    comando_detectado = 0;}
            | comando_HTTP NL       {nLinha++;
                                    comando_detectado = 0;}
            | comando_sp NL         {add_param_list_begin(&frase);
                                    nLinha++;
                                    comando_detectado = 0;}
            | erro                  {nLinha++;
                                    comando_detectado = 0;
                                    fprintf(stderr,RED "Erro(l_%d): Nao ha comando valido. \n" RESET, nLinha);}
            | FIM_REQ               {nLinha++;
                                    comando_detectado = 0;}
            |LOGINDATA				{strcpy(comandoTxt, $1);
									separaLogin(comandoTxt);
									}

comando_HTTP : COMANDO_HTTP         {strcpy(comandoTxt, $1);
                                    requisicao = add_command_list(&comandoTxt);}
            | comando_HTTP PARAMETRO_SP {strcpy(paramTxt, $2);
                                    add_param_list_begin(&paramTxt);}

comando_sp : COMANDO                {frase[0] = '\0';
                                    strcpy(comandoTxt, $1);
                                    add_command_list(&comandoTxt);}
            |comando_sp VIRGULA     {strcat(frase, ",");}
            |comando_sp DP          {strcat(frase, ":");}
            |comando_sp palavra     {strcat(frase, " ");
                                    strcat(frase, $2);}

comando : palavra DP                {comando_detectado = 1;
                                    strcpy(comandoTxt, $1);
                                    add_command_list(&comandoTxt);}

erro    :  DP parametro_final
        |  DP NL


parametro_final :   parametros parametro_NL
                |   parametro_NL

parametros  :   parametro
            |   parametros parametro

parametro_NL: palavra NL            { if(comando_detectado){
                                        strcpy(paramTxt, $1);
                                        add_param_list_begin(&paramTxt);
                                    }};

parametro: palavra VIRGULA          { if(comando_detectado){
                                        strcpy(paramTxt, $1);
                                        add_param_list_begin(&paramTxt);
                                    }};

palavra : PALAVRA



%%

//funcao apenas para suprimir o print "syntax error"
void yyerror(){
    return;
}

void print_list() {
    command_list * current = list;

    while (current != NULL) {

        printf("%s\n", current->command);

        while(current->params != NULL){
            printf("\tParâmetro: %s\n", current->params->param);
            current->params = current->params->next;
        }

        current = current->next;
    }
}

command_list *add_command_list(char *command) {
    //se a lista nao tiver sido criada, cria o primeiro elemento
    if(list == NULL){

        list = malloc(sizeof(command_list));

        strcpy(list->command, command);
        list->next = NULL;
        list->params = NULL;

        return list;
    }

    command_list * current = list;

    while (current->next != NULL) {
        current = current->next;
    }

    current->next = malloc(sizeof(command_list));
    strcpy(current->next->command, command);
    current->next->next = NULL;
    current->next->params = NULL;

    return current->next;
}


void add_param_list_begin(char *param) {

    command_list * current = list;

    while (current->next != NULL) {
        current = current->next;
    }

    //se a lista de parametros nao tiver sido criada, cria o primeiro elemento
    if(current->params == NULL){

        current->params = malloc(sizeof(param_list));

        strcpy(current->params->param, param);
        current->params->next = NULL;

        return;
    }
    param_list * new_node;
    new_node = malloc(sizeof(param_list));

    strcpy(new_node->param, param);
    new_node->next = current->params;
    current->params = new_node;

    return;
}

void separaLogin(char *comandoTxt){
	char * pch;
	pch = strtok(comandoTxt, "&");
	int i=0;
	while(pch){
		fprintf(stderr, "%s\n", pch);
		if(i==0) strcpy(nome,pch);
		if(i==1) strcpy(senha,pch);
		pch = strtok(NULL, "&");
		i++;
	}
	//fprintf(stderr,"nome:%s senha:%s\n",nome,senha);
	
	pch = strtok(nome, "=");
	pch = strtok(NULL, "=");
	strcpy(nome,pch);
	//fprintf(stderr, "------------------------nome: %s\n",nome);
	
	pch = strtok(senha, "=");
	pch = strtok(NULL, "=");
	strcpy(senha,pch);
	//fprintf(stderr, "------------------------senha: %s\n",senha);
}



/*
int acessoValido(){
	if(strcmp(nome, nomevalido)) return 0;
	if(strcmp(senha, senhavalida)) return 0;
	return 1;
}
*/



command_list *symtab_get_parse_result(){
  return requisicao;
}

char *getNome(){
  return nome;
}
char *getSenha(){
  return senha;
}