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

^POST	       {BEGIN comandoSpec;
				sscanf(yytext, "%s", &yylval.str);
        return (COMANDO_HTTP);
        }

^PUT			{BEGIN comandoSpec;
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

[ ]		;
[#].*   ;
^\r\n		{return(FIM_REQ);}
\r\n		{return(NL);}

.		return(INVALIDO);

%%
