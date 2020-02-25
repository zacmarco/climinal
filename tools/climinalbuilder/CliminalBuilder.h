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

//SINGLETON CLASS

#ifndef CCLIMINALBUILDER_H
#define CCLIMINALBUILDER_H

#ifdef WIN32
#include "stdafx.h"
#endif

#include "cli_data.h"
#include "errsnmbr.h"

class CCliminalBuilder
{
private:
  static CCliminalBuilder* m_instance;
  unsigned int depth=0;
  CConfig config;

protected:
  CCliminalBuilder(void);

public:
  static CCliminalBuilder* getInstance();
  ~CCliminalBuilder(void);

  CCommands* getCurrentCommandDataStructurePointer();
  int InsertSubContext(const string s,CContexts* ctx);

  void incrementDepth() { depth++; }
  unsigned int getDepth() { return depth; }

  CConfig *getConfigPointer() { return &config; };

  CCurrentItem* cStatus;
  MapContexts* mapCtx;
};

#endif
