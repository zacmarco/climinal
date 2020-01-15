%{
#define YYSTYPE char *
#include "y.tab.hpp"
#include <stdlib.h>
#include <stdio.h>


extern "C" int     yylex(void);
extern FILE *yyin;

inline char *getString( const char *in )
{
    int count_in=0, count_out=0;
    char *out = NULL;

    out = (char*)malloc(strlen(in)+1);

    if(out)
    {
        while( count_in < strlen(in) )
        {
            if( in[count_in] == '\n' )
            {
                while( in[++count_in] == ' ' ) {}
                out[count_out++] = '\n';
            }
            else
                out[count_out++] = in[count_in++];
        }

        out[count_out] = '\0';
    }

    return out;
}

%}

%option noyywrap 
%option nounput
%option yylineno

%%

\<name\>                                                        return NAME; 
\<\/name\>                                                      return ENDNAME; 

\<context\>                                                     return CTX; 
\<command\>                                                     return CMD;
\<param\>                                                       return PAR;

\<\/context\>                                                   return ENDCTX;
\<\/command\>                                                   return ENDCMD;
\<\/param\>                                                     return ENDPAR;

true                                yylval=strdup("1");         return BOOL;
false                               yylval=strdup("0");         return BOOL;


\<brief\>                                                       return BRIEF;
\<help\>                                                        return HELP;
\<function\>                                                    return FUNCTION;
\<required\>                                                    return REQUIRED;
\<numval\>                                                      return NUMVAL;
\<reqval\>                                                      return REQVAL;
\<description\>                                                 return DESCRIPTION;
\<values\>                                                      return VALUES;
\<prompt\>                                                      return PROMPT;

\<\/brief\>                                                     return ENDBRIEF;
\<\/help\>                                                      return ENDHELP;
\<\/function\>                                                  return ENDFUNCTION;
\<\/required\>                                                  return ENDREQUIRED;
\<\/numval\>                                                    return ENDNUMVAL;
\<\/reqval\>                                                    return ENDREQVAL;
\<\/description\>                                               return ENDDESCRIPTION;
\<\/values\>                                                    return ENDVALUES;
\<\/prompt\>                                                    return ENDPROMPT;

\<!--([^-]|-[^-]|--+[^->])*-*--\>                                 ;
#+$                                                             ;


\"[^\^\^\"^\'$]+\"            {   
                                    yytext[strlen(yytext)-1] = '\0'; 
                                    yylval=getString(yytext+1);
                                    return STR; 
                              }



[0-9]+                           yylval=strdup(yytext);   return NUM;
[-_a-zA-Z0-9\.]+                 yylval=strdup(yytext);   return TXT;
\n                                                              ;
.                                                               ;

%%


