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

#ifndef _SESSION_H_
#define _SESSION_H_

#include <stdio.h>
#include "climinal/cli.h"
#include "climinal/terminal.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_STRLEN              (256)
#define MAX_NUM_COMPL_ENTRIES   (256)

    typedef struct Clisession   Clisession;
    typedef char                **(*Clicompleter)(Clisession*,const char*,int,int);

    void    initsession     ( Clisession *session, Clicontext *main, const FILE *in, const FILE *out );
    int     readline        ( Clisession *session, const char *prompt );
    void    setcompleter    ( Clisession *session, Clicompleter );
    void    printhelp       ( Clisession *session );

    struct Clisession
    {
        /* The active context */
        Clicmd          *active_cmd;
        Clicontext      *active_context;
        Clicontext      *main_context;

        char            prompt [MAX_STRLEN];

        Cliterm         term;
        Clicompleter    completer;
    };

#ifdef __cplusplus
}
#endif

#endif
