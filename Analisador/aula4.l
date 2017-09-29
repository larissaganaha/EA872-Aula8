%{

  #include "y.tab.h"
  #include <string.h>

%}

%x				comandoSpec

%%
^GET			{BEGIN comandoSpec;
  				sscanf(yytext, "%s", &yylval.str);
          return (COMANDO_HTTP);
          }

^HEAD			{BEGIN comandoSpec;
				sscanf(yytext, "%s", &yylval.str);
        return (COMANDO_HTTP);
        }

^TRACE			{BEGIN comandoSpec;
				sscanf(yytext, "%s", &yylval.str);
        return (COMANDO_HTTP);
        }

^OPTIONS	    {BEGIN comandoSpec;
				sscanf(yytext, "%s", &yylval.str);
        return (COMANDO_HTTP);
        }

<comandoSpec>[a-zA-Z0-9_Ç-Û;/+.=*()-]+	{
              	sscanf(yytext, "%s", &yylval.str);
  				return (PARAMETRO_SP);
              }

<comandoSpec>\n {
				BEGIN 0;
	          	strcpy(yylval.str, yytext);
			  	return(NL);
				}

^User-Agent:	{
			sscanf("User-Agent", "%s", &yylval.str);
			return (COMANDO);
		}

[a-zA-Z0-9_Ç-Û;/+.=*()-]+ 		{ sscanf(yytext, "%s", &yylval.str);
		  return (PALAVRA);
		}

localhost:[0-9]+				{sscanf(yytext, "%s", &yylval.str);
		  return (PALAVRA);
		}

:		{ sscanf(yytext, "%s", &yylval.str);
		  return(DP);
		}
,		{ sscanf(yytext, "%s", &yylval.str);
		  return(VIRGULA);
		}
\n		{
		  BEGIN 0;
          strcpy(yylval.str, yytext);
		  return(NL);
		}
[ ]		;
[#].*   ;

[Mm]ensagem[ ][Rr]ecebida:  ;

	/*a seguinte linha pode vir a ser usada para decidir se a conexao foi bem estabelecida, e se os parametros devem ser coletados*/
.*já[ ]está[ ]aceitando[ ]conexões[ ]de[ ]clientes[ ]HTTP.	;

.		return(INVALIDO);

%%
