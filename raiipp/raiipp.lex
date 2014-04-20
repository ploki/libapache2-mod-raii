%{
/* RAII PREPROCESSOR */
#include <stdio.h>

#include "raiipp.bison.h"
#define YYSTYPE char

  static void yyunput ( int c, char *buf_ptr) __attribute__ ((unused));

%}

char		.


%%
"<%#"  return(DECLARATION);
"<%--" return(COMMENT);
"<%="  return(EVALUATION);
"<%!"  return(ATTR);
"<%:"  return(TAG);
"<%/"  return(TAGSTOP);
"<%"   return(CODE);
"%>"   return(FERMETURE);
{char}	{
          yylval=yytext[0];
	        return(CHAR);
        }
"\n"   return(EOL);
