/*
   Climinal
   Copyright (C) 2020, Marco Zaccheria

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General
   Public License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA
 */
#ifndef _CLI_H_
#define _CLI_H_

#include <stdio.h>
#include "climinal/error.h"

/* To be removed, and to be solved by adding the num of parameters to the Clicmd structure */
#define MAX_PARAM_NUM (32)
#define MAX_VAL_NUM (32)

#ifdef __cplusplus
extern "C"
{
#endif

    /* 
       The "Cmdinfo" struct contains the pointer passed to the callback. It should be inspected through
       the provided macros
     */
    typedef struct Cmdinfo  Cmdinfo;
    typedef struct Cmdparam Cmdparam;

    /* This is the definition of the user callback to be registered */
    typedef int (*Clicbk)(FILE *in, FILE *out, const Cmdinfo *cmdinfo, const char *cmd, void *cookie);

    /* This is the definition of the user callback to complete values at runtime */
    typedef int (*Valcbk)(char **val, void *cookie);

    typedef struct Clicmd       Clicmd;
    typedef struct Cliparam     Cliparam;
    typedef struct Clicontext   Clicontext;

    typedef struct Cliconfig    Cliconfig;
    typedef struct Clihandle    Clihandle;


    /* These are the MACROs to handle the Cmdinfo structure without caring of its internals */
#define CLIMINAL_GET_DEFVAL(cmdinfo)                 (cmdinfo->defval)
#define CLIMINAL_GET_PARAM_NUM(cmdinfo)              (cmdinfo->paramnum)
#define CLIMINAL_GET_PARAM_NAME(cmdinfo, parnum)     (   ((cmdinfo->param[parnum]) && (parnum<=cmdinfo->paramnum))? \
        cmdinfo->param[parnum]->name:NULL)
        
#define CLIMINAL_GET_PARAM_NUM_VAL(cmdinfo, parnum)  (   ((cmdinfo->param[parnum]) && (parnum<=cmdinfo->paramnum))? \
        cmdinfo->param[parnum]->numval:0)
#define CLIMINAL_GET_PARAM_VAL(cmdinfo, parnum)      (   ((cmdinfo->param[parnum]) && (parnum<=cmdinfo->paramnum))? \
        cmdinfo->param[parnum]->value:NULL)

#define CLIMINAL_SET_NEXT_PROMPT(cmdinfo, prompt)    (strcpy(((Cmdinfo*)info)->new_prompt, prompt))

    /* The main CLI entry point */
    int climinal_main(const FILE *in, FILE *out, Clihandle *handle, void *cookie);


    /***********************************************************************************
      These are the definitions of required structures. Normally a callback developer shouldn't care of them,
      cause they should use the provided MACROs to navigate through the strucure
     ***********************************************************************************/

    /* Detailed types definition */
    struct Cliparam
    {
        char            *name;
        char            *description;
        unsigned int    required;
        unsigned int    numval;
        Valcbk          val;
    };

    struct Clicmd 
    {
        char            *name;
        char            *brief;
        char            *help;
        Cliparam        *param;
        void            *subcontext;
        Clicmd          *subcmd;
        Clicbk          cbk;
        Valcbk          defval;
    };

    struct Clicontext
    {
        Clicontext          *father;
        Clicmd              *cmd;
        char                *prompt;
        unsigned int        depth;
    };

    struct Cliconfig
    {
        unsigned int history_size;
        unsigned int context_depth;
    };

    struct Clihandle
    {
        Clicontext          *maincontext;
        Cliconfig           *config;
    };


    /* Cmdparam:
       - name : the name of the provided parameter
       - value : its value
     */
    struct Cmdparam
    {
        char            *name;
        unsigned int    numval;
        char            *value[MAX_VAL_NUM];
    };

    /* Cmdinfo:
       - defval : the default parameter value
       - param : an array of pointers to parameters given by the user
     */
    struct Cmdinfo
    {
        char            *defval;
        unsigned int    paramnum;
        struct Cmdparam *param[MAX_PARAM_NUM];
        char            new_prompt[256];
    };

    Cmdparam *findparam( const Cmdinfo *info, const char *name );

#ifdef __cplusplus
}
#endif

#endif 
