
%{
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  
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
  
  typedef struct command_list{
      char command[200];
      struct command_list *next;
      struct param_list *params;
  } command_list;

  typedef struct param_list{
      char param[200];
      struct param_list *next;
  } param_list;

  
  command_list *add_command_list(char *command);
  void add_param_list_begin(char *param);
  void print_list();
  void getOutput(command_list * requisicao);
  int escreveArquivo(char address[ADDRESS_SIZE], struct stat fileStat);
  int get_access(char *local );


  struct command_list * list = NULL;
  struct command_list * requisicao = NULL;

  //flags de controle
  int comando_detectado = 0;
  int nLinha = 0;
  char comandoTxt[200];
  char paramTxt[200];
  char frase[200];
  
  int quebra_linha = 0;

  
%}

%union {
  char str[200];
}

%token <str> PALAVRA COMANDO_HTTP PARAMETRO_SP PARAMETRO_UA COMANDO
%token VIRGULA NL DP INVALIDO

%type <str> palavra DP parametro parametro_NL comando_HTTP comando_sp

%%

linhas  :   linhas linha
        | linha


linha   :   comando parametro_final {nLinha++;
                                     quebra_linha = 0;
                                    comando_detectado = 0;}
            | comando NL    {nLinha++;
                             quebra_linha = 0;
                            comando_detectado = 0;}
            | comando_HTTP NL {nLinha++;
                               quebra_linha = 0;
                              comando_detectado = 0;}
            | comando_sp NL {add_param_list_begin(&frase);
                            nLinha++;
                             quebra_linha = 0;
                            comando_detectado = 0;
                            }
            | erro      {nLinha++;
                         quebra_linha = 0;
                        comando_detectado = 0;
                        fprintf(stderr,RED "Erro(l_%d): Nao ha comando valido.\n" RESET, nLinha);}
            | NL        {nLinha++;
                         if(!quebra_linha){
                            quebra_linha++;
                         }
                         else {
                           quebra_linha = 0;
                           printf("--------Tratando %s -------\n", requisicao->command);
                           getOutput(requisicao);
                         }
                         comando_detectado = 0;}

comando_HTTP : COMANDO_HTTP {
                            strcpy(comandoTxt, $1);
                            requisicao = add_command_list(&comandoTxt);
                            printf("--------Requisicao %s encontrada, coletando parametros-------\n", requisicao->command);

                          }
            | comando_HTTP PARAMETRO_SP{
                            strcpy(paramTxt, $2);
                            add_param_list_begin(&paramTxt);
                          }

comando_sp : COMANDO   {frase[0] = '\0';
                        strcpy(comandoTxt, $1);
                        add_command_list(&comandoTxt);}
            |comando_sp VIRGULA {strcat(frase, ",");}
            |comando_sp DP      {strcat(frase, ":");}
            |comando_sp palavra {strcat(frase, " ");
                                strcat(frase, $2);}

comando : palavra DP {comando_detectado = 1;
                        strcpy(comandoTxt, $1);
                        add_command_list(&comandoTxt);}

erro    :  DP parametro_final
        |  DP NL


parametro_final :   parametros parametro_NL
                |   parametro_NL

parametros  :   parametro
            |   parametros parametro

parametro_NL: palavra NL   { if(comando_detectado){
                                strcpy(paramTxt, $1);
                                add_param_list_begin(&paramTxt);
                             }
                           };

parametro: palavra VIRGULA { if(comando_detectado){
                                strcpy(paramTxt, $1);
                                add_param_list_begin(&paramTxt);
                             }
                           };

palavra : PALAVRA


%%
  
void getOutput(command_list * requisicao){
    if(requisicao == NULL){
      printf("Nada para tratar\n");
      return;
    }
    if (strcmp(requisicao->command, "GET") == 0){
      printf("\n");
      if(requisicao->next != NULL){ //há parametros
        //imprime cabecalho padrao
        get_access(requisicao->params->next->param); //imprime o HTML
      }
      else{
        get_access(requisicao->params->next->param); //imprime o HTML
      }
    }
    else if(strcmp(requisicao->command, "HEAD") == 0){
      if(requisicao->next != NULL){ //há parametros
        //func
      }
      
    }
    else if(strcmp(requisicao->command, "TRACE") == 0){
      if(requisicao->next != NULL){ //há parametros
        //func
      }
      
    }
    else if(strcmp(requisicao->command, "OPTIONS") == 0){
      if(requisicao->next != NULL){ //há parametros
        //func
      }
      
    }
    
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

/*
retorno:
0 = nao ha permissao de leitura
1 = o arquivo foi impresso corretamente
*/
int escreveArquivo(char address[ADDRESS_SIZE], struct stat fileStat){
  int file, size;
  char  buf[CHUNK];
  //verifica permissao de leitura
  if( !(fileStat.st_mode & S_IRUSR) ){
        fprintf(stderr, "403 - Forbidden\n");   return 0;
  }

  //tenta abrir o arquivo
  if ((file = open(address, O_RDONLY, 0600)) < 0){
    fprintf(stderr, "Erro na abertura de %s (tem permissao de leitura).\n", address);
    exit(0);
  }

  //le e escreve o arquivo na saida padrao
  while( (size =  read(file, buf, sizeof(buf))) > 0 ){
    if(write( 1, buf, size) < 0){
      fprintf(stderr, "Erro na escrita de %s.\n", address);
      exit(0);
    }
  }
  return 1;
}

//Acessa o recurso com o get para imprimir seu conteudo
int get_access(char *local ) {
    char address[ADDRESS_SIZE], temp_address[ADDRESS_SIZE];
    struct stat fileStat;
    int aux = 0;
    
    strcpy(address, "."); //procura a partir do diretorio local
    strcat(address, local);

    //verifica se o arquivo existe, e associa ele a fileStat
    if(stat(address,&fileStat) < 0){
        fprintf(stderr, "404 - Not Found.\n");
        return 1;
    }
    
    //verifica se o recurso eh um diretorio
    if( S_ISDIR(fileStat.st_mode) ) {
      //se ele possui permissao de execucao (acesso)
    if( fileStat.st_mode & S_IXUSR ){

      //concatena address com index
        strcpy(temp_address, "\0");
        strcat(temp_address, address);
        if(address[strlen(address)-1] != '/')   //se o endereco nao termina em '/'
          strcat(temp_address, "/");        //insere '/' no final do endereco
        strcat(temp_address, "index.html");

        //verifica se o index.html existe, e associa ele a fileStat
        if(stat(temp_address,&fileStat) >= 0){
          if( escreveArquivo(temp_address, fileStat) ) return 0;
          aux = 1; //index nao tem permissao de leitura
        }

        //concatena address com welcome
        strcpy(temp_address, "\0");
        strcat(temp_address, address);
        if(address[strlen(address)-1] != '/')   //se o endereco nao termina em '/'
          strcat(temp_address, "/");        //insere '/' no final do endereco
        strcat(temp_address, "welcome.html");

        //verifica se o welcome.html existe, e associa ele a fileStat
        if(stat(temp_address,&fileStat) >= 0){
          if( escreveArquivo(temp_address, fileStat) ) return 0;
            fprintf(stderr, "403 - Forbidden\n");
          return 1;
          }

        if(aux){
          fprintf(stderr, "403 - Forbidden\n");
        return 1;
        }

        //nenhum dos arquivos existem
        fprintf(stderr, "404 - Not Found\n");
          return 1;
      }
    //se ele nao possui permissao de execucao (acesso)
    else{
      fprintf(stderr, "403 - Forbidden\n");
      return 1;
    }
    }
    //se o recurso eh um arquivo
    else {      
    return escreveArquivo(address, fileStat);     
    }
  
    return 1;
}

void main() {
    yyparse();
    print_list();
}
