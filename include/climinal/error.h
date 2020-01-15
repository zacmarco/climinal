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

#ifndef _ERROR_H_
#define _ERROR_H_

#ifdef __cplusplus
extern "C"
{
#endif

    enum
    {
        CLIMINAL_NO_ERROR,
        CLIMINAL_E_GENERIC,
        CLIMINAL_E_NO_CONTEXT_SWITCH,
        CLIMINAL_E_EXIT,
        CLIMINAL_E_INVALID,
        CLIMINAL_E_NOT_FOUND,
        CLIMINAL_E_MEMORY,
        CLIMINAL_E_CONF,
        CLIMINAL_E_VALUE 
    };

#define CLIMINAL_SUCCESS(foo)    ( (foo) == CLIMINAL_NO_ERROR )
#define CLIMINAL_ERROR(foo)      ( (foo) != CLIMINAL_NO_ERROR )

#ifdef __cplusplus
}
#endif

#endif 
