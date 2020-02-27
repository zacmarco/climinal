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

#ifndef CCONFIG_H
#define CCONFIG_H

#ifdef WIN32
#include "stdafx.h"
#endif

#include "common.h"
#include <atomic>

class CConfig
{
public:
  CConfig();
  ~CConfig();

  int  getHistorySize(void);
  void setHistorySize(const unsigned int s); 

  int  getContextDepth(void);
  void setContextDepth(const unsigned int d); 

private:
  unsigned int history_size;
  unsigned int context_depth;
};

#endif
