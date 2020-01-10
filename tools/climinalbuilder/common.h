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

#ifndef _COMMON_H
#define _COMMON_H

#ifdef WIN32
#include "stdafx.h"
#endif


#include <string>
#include <map>
#include <list>
#include <stack>

#include <ios>
#include <fstream>
#include <sstream>


using std::ofstream;
using std::ostringstream;
using std::map;
using std::string;
using std::list;
using std::make_pair;
using std::pair;
using std::ios_base;
using std::stack;

typedef stack<string> StackContexts;
typedef list<string> ListCommands;

#endif