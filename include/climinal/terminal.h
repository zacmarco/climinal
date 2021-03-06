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

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include "climinal/history.h"

#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CURS_L(outFile, num)      { int i; for(i=0; i<num; ++i) { fprintf(outFile, "%c%c%c", 0x1b, 0x5b, 0x44); } }
#define CURS_R(outFile, num)      { int i; for(i=0; i<num; ++i) { fprintf(outFile, "%c%c%c", 0x1b, 0x5b, 0x43); } } 

    typedef struct Cliterm       Cliterm;

    int set_terminal  ( Cliterm *term );
    int res_terminal  ( Cliterm *term );

    struct Cliterm
    {
        /* In and Out streams */
        FILE            *in;
        FILE            *out;

        unsigned int    pos;
        unsigned int    len;

        char            buffer[MAX_BUFLEN];
        int             buffer_modified;

#ifdef HAVE_TERMIOS_H
        struct termios  term_io;
#endif

        Clihistory      history;
        char            *(*cmd_compl)(void) ;      
    };




#ifdef __cplusplus
}
#endif

#endif
