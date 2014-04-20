%{
/* RAII PREPROCESSOR */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


  //#include "raiipp.h"
#define YYSTYPE int 

  void raii_chmode(int mode );
  void raii_putc(YYSTYPE c);
  
  int yyparse(void);
  int yylex(void);
int yyerror(char *s)
{
	printf("%s\n",s);
	return 1;
 }


%}

%token COMMENT EVALUATION DECLARATION FERMETURE CODE CHAR EOL ATTR TAG TAGSTOP

%start Input

%%

Input:
	/* Vide */
	| Input Elem
	;

Elem:
	  COMMENT      { raii_chmode(COMMENT); }
	| EVALUATION   { raii_chmode(EVALUATION); }
	| DECLARATION  { raii_chmode(DECLARATION); }
	| FERMETURE    { raii_chmode(FERMETURE); }
  | CODE         { raii_chmode(CODE); }
  | TAG          { raii_chmode(TAG); }
  | TAGSTOP      { raii_chmode(TAGSTOP); }
  | ATTR         { raii_chmode(ATTR); }
	| CHAR         { raii_putc($1); }
  | EOL          { raii_putc('\n'); }
	;
%%

int raii_mode = FERMETURE;
int raii_lmode = FERMETURE;
int out_active=1;

int incodestring=0;

char buf_decl[4*1024*1024]="";
char buf_attr[4*1024*1024]="";
char buf_code[4*1024*1024]="";
char buf_tag_stack[4*1024*1024]="";
int tag_stack[1024]= {0,};
char *tag_stack_classes[1024][1024]={NULL,};
int brace_level=0;

int p_decl=0,
  p_attr=0,
  p_code=0;
char *filename;
int ln=1;

const char* dump_tag_stack(int downto) {
  buf_tag_stack[0]='\0';

  while ( brace_level >= downto ) {
	  while ( tag_stack[brace_level] > 0 ) {
		strcat(buf_tag_stack,"} while(false);}");
		--tag_stack[brace_level];
	  }
    --brace_level;
  }
 return buf_tag_stack;
}

int main(int argc, char ** argv)
{
  if ( strcmp("-",argv[1]) != 0 ) {
        close(0);
        fopen(argv[1],"r");
        filename=strdup(argv[1]);
  }
  else
        filename=strdup("stdin");

  if ( strcmp("-",argv[2]) != 0 ) {
        close(1);
        fopen(argv[2],"w");
  }
  p_code+=sprintf(&buf_code[p_code],"response << \"");

  yyparse();

	int bl = brace_level;

  printf("# 1 \"raiipp_stub\"\n");
  printf("#include <raii.H>\n");
  printf("using namespace raii;\n");
  printf("%s\n",buf_decl);
  printf("# 4 \"raiipp_stub\"\n");
  printf("#ifndef RAII_SERVLET_CLASS\n");
  printf("# define RAII_SERVLET_CLASS raii::HttpServlet\n");
  printf("#endif\n");
  printf("#ifdef RAII_NAMESPACE\n");
  printf("namespace RAII_NAMESPACE {\n");
  printf("#endif\n");
  printf("class SERVLET(RaiiServlet) : public RAII_SERVLET_CLASS {\n");
  printf("%s",buf_attr);
  printf("# 12 \"raiipp_stub\"\n");
  printf("void service(HttpServletRequest &request, HttpServletResponse& response)\n{\n");
  printf(" %s\";",buf_code);
  printf("%s",dump_tag_stack(0));
  printf("\n}\n};\n");
  //printf("extern \"C\" RaiiServlet_%u * servletFactory() { return new RaiiServlet_%u(); }\n",rn,rn);
  printf("# 13 \"raiipp_stub\"\n");
  printf("exportServlet(RaiiServlet);\n");
  printf("#ifdef RAII_NAMESPACE\n");
  printf("}\n");
  printf("#endif\n");

  if ( bl > 0 ) {
	fprintf(stderr,"\n\n%s: error: %d closing braces forgotten !\n",filename,bl);
	return 1;
  }
  else if ( bl < 0 ) {
	fprintf(stderr,"\n\n%s: error: too many closing braces => %d !\n",filename,-bl);
	return 1 ;
  }
  
  if ( raii_mode != FERMETURE )
    {
      fprintf(stderr,"\n\n%s: error: Balise ouverte !\n",filename);
      return 1;
    }
  return 0;
}


///
/// gestion des tags
///
typedef enum { inClass, inVar, inValue } WhereInTag;
char inString; // peut �tre   "  '  \0
WhereInTag where;
char className[16*1024];
char varName[16*1024];
char varValue[16*1024];
char bodyCond[16*1024];
int val_is_for_body_cond=0;
int body_cond_defined=0;
int chevron_level=0;
    
void raii_chmode(int mode )
{
  raii_lmode=raii_mode;
  raii_mode=mode;
  switch ( mode )
    {
    case COMMENT:
      out_active=0;
      break;
    case EVALUATION:
      p_code+=sprintf (&buf_code[p_code], "\"; response << (");
      break;
    case ATTR:
      //fait rien
      break;
    case DECLARATION:
      //fait rien
      break; 
    case CODE:
      p_code+=sprintf ( &buf_code[p_code],"\";");
      break;
    case TAG:
      p_code+=sprintf ( &buf_code[p_code],"\";");
      where=inClass;
      className[0]='\0';
      break;
    case TAGSTOP:
      p_code+=sprintf ( &buf_code[p_code],"\";");
      where=inClass;
      className[0]='\0';
      break;
    case FERMETURE:
      switch ( raii_lmode ) {
        case COMMENT:
          out_active=1;
          break;
        case EVALUATION:
          p_code+=sprintf (&buf_code[p_code],"); response << \"");
          break;
        case ATTR:
          //fait rien
          break;
        case DECLARATION:
          //fait rien
          break;
        case CODE:
          p_code+=sprintf ( &buf_code[p_code]," ; response << \"");
          break;
        case TAG:
        {
          if ( varValue[0] )
          {
            p_code+=sprintf ( &buf_code[p_code],"\n# %d \"%s\"\n",ln,filename);
            p_code+=sprintf ( &buf_code[p_code],"%s); ",varValue);
            varValue[0]=0;
          }
          if ( className[0] )
          {
            p_code+=sprintf ( &buf_code[p_code],"\n# %d \"%s\"\n",ln,filename);
            p_code+=sprintf ( &buf_code[p_code]," { %s self; ",className);
            tag_stack_classes[tag_stack[brace_level]+1][brace_level]=strdup(className);
            className[0]=0;
          }
          p_code+=sprintf ( &buf_code[p_code],"\n# %d \"%s\"\n",ln,filename);
          p_code+=sprintf ( &buf_code[p_code]," self.doStart(request,response);__typeof__(self)& super __attribute__((unused)) = self;");
          if ( body_cond_defined ) {
            p_code+=sprintf ( &buf_code[p_code]," if ( %s ) ",bodyCond);
            body_cond_defined=0;
            bodyCond[0]=0;
          }
	  p_code+=sprintf ( &buf_code[p_code]," do { response << \"");
		++tag_stack[brace_level];
          
        }
        break;
         case TAGSTOP:
         {
          if ( className[0] )
          {
		if ( ! tag_stack_classes[tag_stack[brace_level]][brace_level] ) {
			fprintf(stderr,"%s:%d: error: tag_stack_classes[%d][%d] is null: probably closing a non-existing tag\n",filename,ln,tag_stack[brace_level],brace_level);
			exit(1);
		}

	    while ( strcmp(tag_stack_classes[tag_stack[brace_level]][brace_level],className) ) {
		//tant que les classes ne sont pas les mêmes
		p_code+=sprintf( &buf_code[p_code],"} while(false);}");
		free(tag_stack_classes[tag_stack[brace_level]][brace_level]);tag_stack_classes[tag_stack[brace_level]][brace_level]=NULL;
		--tag_stack[brace_level];
		if ( ! tag_stack_classes[tag_stack[brace_level]][brace_level] ) {
			fprintf(stderr,"%s:%d: error: tag_stack_classes[%d][%d] is null: probably closing a non-existing tag\n",filename,ln,tag_stack[brace_level],brace_level);
			exit(1);
		}
	    }
            p_code+=sprintf ( &buf_code[p_code],"\n# %d \"%s\"\n",ln,filename);
            p_code+=sprintf ( &buf_code[p_code],"; %s& __test_type__ __attribute__((unused)) = self; } while ( ",className);
            className[0]=0;
            free(tag_stack_classes[tag_stack[brace_level]][brace_level]);tag_stack_classes[tag_stack[brace_level]][brace_level]=NULL;
            --tag_stack[brace_level];
          }
	  if ( ! varValue[0] ) {
            p_code+=sprintf( &buf_code[p_code],"false");
	  } else varValue[0]=0;
	  
	          p_code+=sprintf ( &buf_code[p_code],"\n# %d \"%s\"\n",ln,filename);
		p_code+=sprintf ( &buf_code[p_code],"); self.doStop(request,response); }; response << \"");
         }
         break;
      }
      break;
    default:
      printf("grosse erreur !\n");
    }
}

void raii_putc(YYSTYPE cc)
{
  int *p;
  char c=cc;
  static char lc=0;
  char *b;
  if ( raii_mode == DECLARATION ) {
    p=&p_decl;
    b=buf_decl;
  } else if ( raii_mode == ATTR ) {
    p=&p_attr;
    b=buf_attr;
  } else {
      p=&p_code;
      b=buf_code;
    }
    
  if ( ! out_active ) {
      lc=c;
      return;
  }
  
  if ( raii_mode == TAG ) {

    if ( c == '\n' ) ln++;
    
    if ( where == inValue ) {
      if ( c == ';' && ! inString ) {
        if ( val_is_for_body_cond ) {
          body_cond_defined=1;
          snprintf(bodyCond,16*1024,"%s",varValue);
        }
        else {
          *p+=sprintf (b+*p,"\n# %d \"%s\"\n",ln,filename);
          *p+=sprintf( b+*p,"%s);",varValue);
        }
        val_is_for_body_cond=0;
        varValue[0]='\0';
        where=inVar;
        return;
      } else if ( c == ' ' || c == '\t' || c == '\n' || c== '\r' ) {
        if ( varValue[0] == '\0' ) {
          return;
        } else {
          strncat(varValue,&c,1);
        }
      } else {
        strncat(varValue,&c,1);
      }

      if ( inString ) {
        if ( c == inString && lc != '\\' )
         inString=0;
      } else {
        if ( c == '\'' || c == '"' )
          inString=c;
      }
    }
        
    if ( where == inVar ) {
      inString=0;
      if ( c == '=' ) {
	if ( ! strcmp( varName, "cond") ) {
          val_is_for_body_cond=1;
	}
	else {
          *p+=sprintf (b+*p,"\n# %d \"%s\"\n",ln,filename);
          *p+=sprintf( b+*p,"self.set%c%s(",toupper(varName[0]),varName+1);
	  val_is_for_body_cond=0;
	}
        where=inValue;
        varName[0]='\0';
        return;
      } else if ( c == ' ' || c == '\t' || c == '\n' || c== '\r' ) {
        if ( varName[0] == '\0' )
          return;
        else
          strncat(varName,&c,1);
      } else {
        strncat(varName,&c,1);
      }
    }


    if ( where == inClass ) {
      if ( c == '<' ) ++chevron_level;
      if ( c == '>' ) --chevron_level;
      if ( chevron_level > 0 || ( c!= ' ' && c != '\t' && c != '\n' && c!= '\r' ) ) {
        strncat(className,&c,1);
      } else {
        if ( className[0] == '\0' )
          return;
        *p+=sprintf (b+*p,"\n# %d \"%s\"\n",ln,filename);
        *p+=sprintf( b+*p," { %s self; ",className);
        where=inVar;
         tag_stack_classes[tag_stack[brace_level]+1][brace_level]=strdup(className);
        className[0]='\0';
        return;
      }
    }
  } else if ( raii_mode == TAGSTOP ) {

    if ( c == '\n' ) {
	ln++;
	if ( lc != '\\' )
	        *p+=sprintf (b+*p,"\n# %d \"%s\"\n",ln,filename);
	else
		*p+=sprintf(b+*p,"\n");
    }

    if ( where == inValue ) {
      switch(c) {
        case '\n': /*nop déjà traité*/ break;
        default: {
		if ( c != '\t' && c != ' ' )
			varValue[0]='1';
		*p+=sprintf( b+*p,"%c",c);
	}
      }
    }

    if ( where == inClass ) {
      if ( c == '<' ) ++chevron_level;
      if ( c == '>' ) --chevron_level;
      if ( chevron_level > 0 || ( c != ' ' && c != '\t' && c != '\n' && c!= '\r' ) ) {
        strncat(className,&c,1);
      } else {
        if ( className[0] == '\0' )
          return;

	if ( ! tag_stack_classes[tag_stack[brace_level]][brace_level] ) {
		fprintf(stderr,"%s:%d: error: tag_stack_classes[%d][%d] is null: probably closing a non-existing tag\n",filename,ln,tag_stack[brace_level],brace_level);
		exit(1);
	}
	    while ( strcmp(tag_stack_classes[tag_stack[brace_level]][brace_level],className) ) {
		//tant que les classes ne sont pas les mêmes
		p_code+=sprintf( &buf_code[p_code],"} while(false);}");
		free(tag_stack_classes[tag_stack[brace_level]][brace_level]);tag_stack_classes[tag_stack[brace_level]][brace_level]=NULL;
		--tag_stack[brace_level];
		if ( ! tag_stack_classes[tag_stack[brace_level]][brace_level] ) {
			fprintf(stderr,"%s:%d: error: tag_stack_classes[%d][%d] is null: probably closing a non-existing tag\n",filename,ln,tag_stack[brace_level],brace_level);
		exit(1);
		}
	    }
        *p+=sprintf (b+*p,"\n# %d \"%s\"\n",ln,filename);
        *p+=sprintf( b+*p,"; %s& __test_type__ __attribute__((unused)) = self; } while ( ",className);
        where=inValue;
        className[0]='\0';
            free(tag_stack_classes[tag_stack[brace_level]][brace_level]);tag_stack_classes[tag_stack[brace_level]][brace_level]=NULL;
        --tag_stack[brace_level];
        return;
      }
    }

  } else switch ( c ) {

    case '\n':
      ln++;
      if ( raii_mode == FERMETURE ) {
        *p+=sprintf (b+*p,"\\n\"\n# %d \"%s\"\n\"",ln,filename);
      } else {
        if ( lc != '\\' )
          *p+=sprintf (b+*p,"\n# %d \"%s\"\n",ln,filename);
        else
          *p+=sprintf (b+*p,"\n");
      }
      break;

    case '"':
      if ( raii_mode == FERMETURE )
        *p+=sprintf (b+*p,"\\\"");
      else {
          if ( raii_mode == CODE ) {
             if ( incodestring ) {
                if ( lc != '\\' )
                   incodestring=0;
             }
             else {
               incodestring=1;
             }
          }
          *p+=sprintf (b+*p,"\"");
      }
      break;

    case '%':
      *p+=sprintf (b+*p,"%%");
      break;
    
    case '\\':
      if ( raii_mode == FERMETURE )
        *p+=sprintf (b+*p,"\\\\");
      else
        *p+=sprintf (b+*p,"\\");
      break;
 
    case '{':
	if ( raii_mode == CODE && !incodestring ) {
		++brace_level;
	}
      *p+=sprintf (b+*p,"%c",c);
	break;

    case '}':
	if ( raii_mode == CODE && !incodestring ) {
		*p+=sprintf(b+*p,"%s",dump_tag_stack(brace_level));
	}
      *p+=sprintf (b+*p,"%c",c);
	break;
                             
    default:
      *p+=sprintf (b+*p,"%c",c);
  }

  lc=c;
}
