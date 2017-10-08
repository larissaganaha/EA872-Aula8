#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "serverlib.h"

char *formatdate(char *buff, time_t val){
    strftime(buff,200, "%c %Z", localtime(&val));
    return buff;
}

void getCabecalho(char *local, command_list * requisicao){
    char address[200];
    char date[100];
    int fail = 0;

    strcpy(address, ".");
    strcat(address, local);

    //printf("˜˜˜˜˜˜˜˜~addresss ˜˜˜˜˜˜˜˜~\n");
    //printf(address);
    printf("\n");

    struct stat fileStat;

    int file=0;

    // nao conseguiu abrir o endereço
    if((file = open(address, O_RDONLY)) < 0){
        printf("------------------------------------------------------\nCouldn't open file: %s\n------------------------------------------------------\n",address);
        fail = 1;
    }
    //printf("***** Opened file: file = %d *****\n", file);

    if(fstat(file,&fileStat) < 0)
    fail = 1;

    char buffer[30];
    struct timeval tv;
    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    strftime(buffer,30,"%c %Z",localtime(&curtime));

    if(!fail)
        printf("------------------------------------------------------\nInformation for %s\n------------------------------------------------------\n",address);
    if(findParam(requisicao, "Host") && !fail) //atualmente aceita qualquer host
        printf("HTTP 1.1 200 ok\n");
    else
        printf("HTTP 1.1 400 Bad Request\n");


    printf("Server: \tServidor HTTP ver. 0.1 Grupo 7\n");
    if(findParam(requisicao, "Connection"))
        printf("Connection: \t%s\n", findParam(requisicao, "Connection")->params->param);
    printf("Date: \t\t%s\n",buffer);
    if(!fail){
        printf("Content-Length: %d bytes\n",fileStat.st_size);
        printf("Last-Modified: \t%s\n", formatdate(date, fileStat.st_mtime));
        }
    printf("Content-Type: \ttext/html\n");
    printf("\n\n\n");


}



void getOutput(command_list * requisicao){
    if(requisicao == NULL){
        //printf("Nada para tratar\n");
        return;
    }
    if (strcmp(requisicao->command, "GET") == 0){
        printf("\n");
        if(requisicao->next != NULL){ //há parametros
            getCabecalho(requisicao->params->next->param, requisicao); //imprime o cabecalho
            get_access(requisicao->params->next->param);   //imprime o HTML
        }
        else{
            get_access(requisicao->params->next->param);   //imprime o HTML
        }
    }
    else if(strcmp(requisicao->command, "HEAD") == 0){
        if(requisicao->next != NULL){ //há parametros
            getCabecalho(requisicao->params->next->param, requisicao); //imprime o cabecalho
        }
        else{
            getCabecalho(requisicao->params->next->param, requisicao); //imprime o cabecalho
        }
    }
    else if(strcmp(requisicao->command, "TRACE") == 0){
        if(requisicao->next != NULL){ //há parametros
            getCabecalho(requisicao->params->next->param, requisicao); //imprime o cabecalho
        }
        else{
            getCabecalho(requisicao->params->next->param, requisicao); //imprime o cabecalho
        }
    }
    else if(strcmp(requisicao->command, "OPTIONS") == 0){
        if(requisicao->next != NULL){ //há parametros
            getCabecalho(requisicao->params->next->param, requisicao); //imprime o cabecalho
        }
        else{
            getCabecalho(requisicao->params->next->param, requisicao); //imprime o cabecalho
        }
    }

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
        fprintf(stderr, "403 - Forbidden\n");        return 0;
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

//retorna o ponteiro para o parametro("comando") procurado
command_list * findParam(command_list * requisicao, char * param){
    command_list * current = requisicao;

    while (current->next != NULL) {
        current = current->next;

        if(strcmp(current->command , param) == 0 ){
            return current;
        }
    }
    return NULL;
}
