#define CHUNK 1024
#define ADDRESS_SIZE 500

typedef struct command_list{
        char command[200];
        struct command_list *next;
        struct param_list *params;
	} command_list;

typedef struct param_list{
        char param[200];
        struct param_list *next;
	} param_list;

  typedef enum {GET, HEAD, TRACE, OPTIONS, BAD_REQUEST, FORBIDDEN, NOT_FOUND, NOT_ALLOWED, INTERNAL_ERROR, NOT_IMPLEMENTED} tipo_cabecalho;

char *formatdate(char *buff, time_t val);
void getCabecalho(char *local, command_list * requisicao, tipo_cabecalho tipo, FILE * fout);
void getOutput(command_list * requisicao, FILE * fout);
int escreveArquivo(char address[ADDRESS_SIZE], struct stat fileStat, FILE * fout);
tipo_cabecalho get_access(char *local , int imprimir, FILE * fout, char *endereco);
command_list * findParam(command_list * requisicao, char * param);
void erro(tipo_cabecalho tipo, FILE * fout);
