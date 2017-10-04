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


char *formatdate(char *buff, time_t val);
void getCabecalho(char *local, command_list * requisicao);
void getOutput(command_list * requisicao);
int escreveArquivo(char address[ADDRESS_SIZE], struct stat fileStat);
int get_access(char *local );
command_list * findParam(command_list * requisicao, char * param);

