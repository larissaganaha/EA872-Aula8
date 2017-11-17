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

int getSize(char *endereco){
    char * buffer = 0;
    long length;
    FILE * f = fopen (endereco, "rb");

    if (f){
      fseek (f, 0, SEEK_END);
      length = ftell (f);
      fseek (f, 0, SEEK_SET);
      buffer = malloc (length);
      if (buffer) {
	return fread (buffer, 1, length, f);
      }
      fclose (f);
    }
    return 0;
}

void getCabecalho(char *local, command_list * requisicao, tipo_cabecalho tipo, FILE * fout){
    char address[200];
    char date[100];
    struct stat fileStat;
    int fail = 0;
    char buffer[30];
    struct timeval tv;
    time_t curtime;
    int file;
    char endereco[1024];
    int tamanho;

    //obtem data e hora
    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;
    strftime(buffer,30,"%c %Z",localtime(&curtime));

    if(local != NULL){
      strcpy(address, ".");
      strcat(address, local);
    }

    if(tipo > 3){
      //temos um cabecalho de erro
      switch (tipo) {
        case BAD_REQUEST:
          fprintf(fout,"HTTP/1.1 \t400 Bad Request\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Content-Length: 219\n");
          fprintf(fout,"Connection: \tclose\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          break;
        case FORBIDDEN:
          fprintf(fout,"%s ", requisicao->params->param); //http/1.1
          fprintf(fout,"403 Forbidden\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Content-Length: 203\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          break;
        case NOT_FOUND:
          fprintf(fout,"%s ", requisicao->params->param); //http/1.1
          fprintf(fout,"404 Not Found\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Content-Length: 200\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          break;
        case NOT_ALLOWED:
          fprintf(fout,"%s ", requisicao->params->param); //http/1.1
          fprintf(fout,"405 Method Not Allowed\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Allow: \t\tGET, HEAD, TRACE, OPTIONS\n");
          fprintf(fout,"Content-Length: 212\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          break;
        case INTERNAL_ERROR:
          fprintf(fout,"%s ", requisicao->params->param); //http/1.1
          fprintf(fout,"500 Internal Server Error\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Content-Length: 214\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          break;
        case NOT_IMPLEMENTED:
          fprintf(fout,"%s ", requisicao->params->param); //http/1.1
          fprintf(fout,"501 Not Implemented\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Content-Length: 206\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          break;
      }
    }
    //temos um cabecalho de requisicao satisfeita
    else{
      //tenta abrir o arquivo
      if((file = open(address, O_RDONLY)) < 0){fprintf(stderr,"--\n**SHOULD NOT ENTER HERE1**Couldn't open file: %s\n--\n",address);
          return;
      }
      if(fstat(file,&fileStat) < 0) {fprintf(stderr,"**SHOULD NOT ENTER HERE2**\n");return;}
      
      get_access(requisicao->params->next->param, 0, NULL, endereco);
      tamanho = getSize(endereco);
      //printf("-------E1: %s -----------E2: %s --------------tam: %d -------\n",requisicao->params->next->param, endereco, tamanho);

      fprintf(fout,"%s ", requisicao->params->param); //http/1.1
      switch (tipo) {
        case GET:
          fprintf(fout," \t200 OK\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Last-Modified: \t%s\n", formatdate(date, fileStat.st_mtime));
          fprintf(fout,"Accept-Ranges: \tbytes\n");
          fprintf(fout,"Content-Length: %d bytes\n",tamanho);
          fprintf(fout,"Vary: \t\tAccept-Encoding\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          if(findParam(requisicao, "Connection"))
              fprintf(fout,"Connection: \t%s\n", findParam(requisicao, "Connection")->params->param);
          break;
        case HEAD:
          fprintf(fout," \t200 OK\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Last-Modified: \t%s\n", formatdate(date, fileStat.st_mtime));
          fprintf(fout,"Accept-Ranges: \tbytes\n");
          fprintf(fout,"Content-Length: %d bytes\n",tamanho);
          fprintf(fout,"Vary: \t\tAccept-Encoding\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          if(findParam(requisicao, "Connection"))
              fprintf(fout,"Connection: \t%s\n", findParam(requisicao, "Connection")->params->param);
          break;
        case TRACE:
          fprintf(fout," \t200 OK\n");
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          if(findParam(requisicao, "Connection"))
              fprintf(fout,"Connection: \t%s\n", findParam(requisicao, "Connection")->params->param);
          fprintf(fout,"Content-Type: \ttext/html\n");
          fprintf(fout,"Content-Length: %d bytes\n",tamanho);
          break;
        case OPTIONS:
          fprintf(fout," \t200 OK\n");
          fprintf(fout,"Date: \t\t%s\n",buffer);
          fprintf(fout,"Server: \tServidor HTTP ver. 0.2 Grupo 7\n");
          fprintf(fout,"Allow: \t\tGET, HEAD, TRACE, OPTIONS\n");
          fprintf(fout,"Content-Length: 0\n");
          fprintf(fout,"Content-Type: \ttext/html\n");
          break;

      }
      fprintf(fout,"\n");
    }
    return;
}



void getOutput(command_list * requisicao, FILE * fout){
    struct stat fileStat;
    char address[ADDRESS_SIZE];
    //caso BAD_REQUEST
    if(requisicao == NULL){
      getCabecalho(NULL, NULL, BAD_REQUEST, fout);
      fprintf(fout,"\n\n");
      erro(BAD_REQUEST, fout);
    }
    //caso BAD_REQUEST por falta de Host (por enquanto pode ser qualquer host)
    else if(!(findParam(requisicao, "Host")) ){
      getCabecalho(NULL, NULL, BAD_REQUEST, fout);
      fprintf(fout,"\n\n");
      erro(BAD_REQUEST, fout);
    }
    //caso FORBIDDEN
    else if (get_access(requisicao->params->next->param, 0, fout, NULL) == FORBIDDEN) {
      getCabecalho(requisicao->params->next->param, requisicao, FORBIDDEN, fout);
      fprintf(fout,"\n\n");
      erro(FORBIDDEN, fout);
    }
    //caso NOT_FOUND
    else if (get_access(requisicao->params->next->param, 0, fout, NULL) == NOT_FOUND) {
      getCabecalho(requisicao->params->next->param, requisicao, NOT_FOUND, fout);
      fprintf(fout,"\n\n");
      erro(NOT_FOUND, fout);
    }
    //caso GET
    else if (strcmp(requisicao->command, "GET") == 0){
        fprintf(fout,"\n");
        getCabecalho(requisicao->params->next->param, requisicao, GET, fout); //imprime o cabecalho
        get_access(requisicao->params->next->param, 1, fout, NULL);   //imprime o HTML
    }
    else if(strcmp(requisicao->command, "HEAD") == 0){
        getCabecalho(requisicao->params->next->param, requisicao, HEAD, fout); //imprime o cabecalho
    }
    else if(strcmp(requisicao->command, "TRACE") == 0){
        getCabecalho(requisicao->params->next->param, requisicao, TRACE, fout); //imprime o cabecalho
    }
    else if(strcmp(requisicao->command, "OPTIONS") == 0){
        getCabecalho(requisicao->params->next->param, requisicao, OPTIONS, fout); //imprime o cabecalho
    }
    else{
      //not allowed
      getCabecalho(requisicao->params->next->param, requisicao, NOT_ALLOWED, fout);
      fprintf(fout,"\n\n");
      erro(NOT_ALLOWED, fout);
    }
    return;
}

/*
 retorno:
 0 = nao ha permissao de leitura
 1 = o arquivo foi impresso corretamente
 */
int escreveArquivo(char address[ADDRESS_SIZE], struct stat fileStat, FILE * fout){
    int file, size;
    char  buf[CHUNK];
    //verifica permissao de leitura
    if( !(fileStat.st_mode & S_IRUSR) ){
        erro(FORBIDDEN, fout); //forbidden
        return 0;
    }

    //tenta abrir o arquivo
    if ((file = open(address, O_RDONLY, 0600)) < 0){
        fprintf(stderr, "Erro na abertura de %s (tem permissao de leitura).\n", address);
        exit(0);
    }

    //le e escreve o arquivo no fout
    while( (size =  read(file, buf, sizeof(buf))) > 0 ){
        buf[size] = '\0';
        if(fprintf( fout, buf) < 0){
            fprintf(stderr, "Erro na escrita de %s.\n", address);
            exit(0);
        }
    }
    return 1;
}

//Acessa o recurso com o get para verificar o arquivo e possivelmente imprimi-lo
tipo_cabecalho get_access(char *local , int imprimir, FILE * fout, char *endereco) {
    char address[ADDRESS_SIZE], temp_address[ADDRESS_SIZE];
    char *buffer;
    struct stat fileStat;
    int aux = 0;

    strcpy(address, "."); //procura a partir do diretorio local
    strcat(address, local);

    //verifica se o arquivo existe, e associa ele a fileStat
    if(stat(address,&fileStat) < 0){
        return NOT_FOUND;
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
              //gambiarra extrema to com sono
              if( (fileStat.st_mode & S_IRUSR) ){
		  if(endereco) strcpy(endereco, temp_address);
                  if(imprimir) escreveArquivo(temp_address, fileStat, fout);
                  return 0;
              }
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
              //gambiarra extrema to com sono
              if( (fileStat.st_mode & S_IRUSR) ){
		  if(endereco) strcpy(endereco, temp_address);
                  if(imprimir) escreveArquivo(temp_address, fileStat, fout);
                  return 0;
              }
                return FORBIDDEN;
            }

            if(aux){
                return FORBIDDEN;
            }

            //nenhum dos arquivos existem
            return NOT_FOUND;
        }
        //se ele nao possui permissao de execucao (acesso)
        else{
            return FORBIDDEN;
        }
    }
    //se o recurso eh um arquivo
    else {
	if(endereco) strcpy(endereco, address);
        if(imprimir) escreveArquivo(address, fileStat, fout);
    }
    return 0;
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

void erro(tipo_cabecalho tipo,FILE * fout){
  char title[50], h1[50], paragraph[200];

  switch (tipo) {
    case BAD_REQUEST:
      strcpy(title, "400 Bad Request");
      strcpy(h1, "Bad Request");
      strcpy(paragraph, "Your browser sent a request that this server could not understand.");
      break;
    case FORBIDDEN:
      strcpy(title, "403 Forbidden");
      strcpy(h1, "Forbidden");
      strcpy(paragraph, "You do not have the permission to access this resource");
      break;
    case NOT_FOUND:
      strcpy(title, "404 Not Found");
      strcpy(h1, "Not Found");
      strcpy(paragraph, "The requested URL /OI was not found on this server.");
      break;
    case NOT_ALLOWED:
      strcpy(title, "405 Method Not Allowed");
      strcpy(h1, "Not Allowed");
      strcpy(paragraph, "The requested method was not allowed on this server.");
      break;
    case INTERNAL_ERROR:
      strcpy(title, "500 Internal Server Error");
      strcpy(h1, "Internal Server Error");
      strcpy(paragraph, "The server encountered an internal error.");
      break;
    case NOT_IMPLEMENTED:
      strcpy(title, "501 Not Implemented");
      strcpy(h1, "Not Implemented");
      strcpy(paragraph, "The method was not implemented on this server");
      break;
    default:
      strcpy(title, "500 Internal Server Error");
      strcpy(h1, "Internal Server Error");
      strcpy(paragraph, "The server encountered an internal error.");
      break;
  }

  fprintf(fout,"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n<html><head>\n<title>%s</title>\n</head><body>\n<h1>%s</h1>\n", title, h1);
  fprintf(fout,"<p>%s\n</p>\n</body></html>\n\n", paragraph);
  return;
}
