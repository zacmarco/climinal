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

#include "CConfig.h"

//constructor
CConfig::CConfig()
{
    history_size=256;
    context_depth=0;
}

CConfig::~CConfig() {}

int  CConfig::getHistorySize(void)
{
    return history_size;
}


void CConfig::setHistorySize(const unsigned int s)
{
    history_size=s;
}

int  CConfig::getContextDepth(void)
{
    return context_depth;
}


void CConfig::setContextDepth(const unsigned int d)
{
    context_depth=d;
}

