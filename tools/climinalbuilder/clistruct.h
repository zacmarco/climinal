/*
   CliminalBuilder Tool for
   Climinal
   Copyright (C) 2020, Marco Zaccheria, Rocco Affinito

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

#ifndef _CLISTRUCT_H_
#define _CLISTRUCT_H_

//EXPORTED
int create_param   ( const char *name, const char* description, const int required , const int numval, const char *values);
int create_command ( const char *name, const char *brief, const char *help, const char *callback );
int create_context ( const char *prompt );
int end_param      (void);
int end_command    (void);
int end_context    (void);

int write_files   ( const char *filenames);

#endif
