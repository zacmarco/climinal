%{

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include "y.tab.hpp"
#include "clistruct.h"

#define FREE(buf)       ( free((void*)buf) )
#define BOOL2INT(str)   ( ( strcmp((const char*)str,"true") ) ? 0:1 )

extern "C" int  yylex(void);
void            yyerror(const char*);

static char *values=NULL;
static char *description=NULL;

%}

%destructor { free((void*)$$); } TXT STR

%token  NAME
        ENDNAME
        CTX  
        CMD  
        PAR
        ENDCTX
        ENDCMD
        ENDPAR
        REQUIRED
        BRIEF
        HELP
        FUNCTION
        PROMPT
        NUMVAL
        REQVAL
        DESCRIPTION
        VALUES
        STR
        NUM
        TXT
        EQ
        DQ
        ENDBRIEF
        ENDHELP
        ENDFUNCTION
        ENDREQUIRED
        ENDNUMVAL
        ENDREQVAL
        ENDDESCRIPTION
        ENDVALUES
        ENDPROMPT
        BOOL



%%

rules:
        |
        rules rule 
        ;

rule:
        contexts
        ;

contexts:
        | contexts context;

context:
        context_start commands context_end
        ;

context_start:
        CTX PROMPT TXT ENDPROMPT
        {
            int ret;
            ret = create_context((const char*)$3);
            //printf("Create context:%s, return:%d\n", $4, ret);
            FREE($3);
        }
        ;

context_end:
        ENDCTX
        {
            int ret;
            ret = end_context();
            //printf("End context\n");
        }
        ;

commands:
        | commands command;

command:
        command_start command_content command_end
        ;

command_content:
        params commands
        |
        params commands context
        ;

command_start:
        CMD NAME TXT ENDNAME BRIEF STR ENDBRIEF HELP STR ENDHELP FUNCTION TXT ENDFUNCTION
        {
            int ret;
            ret = create_command((const char*)$3, (const char*)$6, (const char*)$9, (const char*)$12);
            //printf("Create command:%s, return:%d\n", $3, ret);
            FREE($3);
            FREE($6);
            FREE($9);
            FREE($12);
        }
        ;

command_end:
        ENDCMD
        {
            end_command();
        }
        ;

params:
        | params param
        ;
param:
        PAR NAME TXT ENDNAME param_description REQUIRED BOOL ENDREQUIRED NUMVAL NUM ENDNUMVAL REQVAL BOOL ENDREQVAL param_values ENDPAR 
        {                                                           
            int ret;
            ret = create_param( (const char*)$3, (const char*)description, $7, $10, $13, (const char*)values );           
            //printf("Create param:%s, return:%d\n", $3, ret);
            FREE($3);
            end_param();
            if(values) {
                free(values);
                values=NULL;
            }
            if(description) {
                free(description);
                description=NULL;
            }
        }
        |
        PAR REQUIRED BOOL ENDREQUIRED param_values ENDPAR
        {
            int ret;
            ret = create_param( NULL, NULL, $3, 1, 1, values );
            //printf("Create def param, return:%d\n", ret);
            end_param();
            if(values) {
                free(values);
                values=NULL;
            }
        }

        ;

param_description:
        DESCRIPTION STR ENDDESCRIPTION 
        {
            description=strdup((const char*)$2);
            FREE($2);
        }
        |
        ;

param_values:
        VALUES TXT ENDVALUES 
        {
            values=strdup((const char*)$2);
            FREE($2);
        }
        |
        ;
%%

extern FILE *yyin;
extern int yylineno;
extern int  yylex_destroy(void);


void yyerror(const char *s)
{
    printf("Parsing error at line %d: %s\n", yylineno, s);
}

int main(int argc, const char **argv)
{
    if( argc == 3 )
    {
        yyin = fopen(argv[1], "r");
        if( yyin )
        {
            if ( yyparse() == 0 ) {
                /* Parsing gone well: let's write the two files */
                write_files(argv[2]);
            }
            fclose( yyin );
        }
        else
            printf( "%s: No such file or directory\n", argv[1] );
    }
    else
        printf("Usage: climinalbuilder <definition_file> <outfile_prefix>\n");
    yylex_destroy();

    return 0;
}


