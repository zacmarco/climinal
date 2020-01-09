/*
   CliminalBuilder Tool for
   Climinal
   Copyright (C) 2014, Marco Zaccheria, Rocco Affinito

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

#include "CContexts.h"

//constructor
CContexts::CContexts()
{
  name = new string();
  prompt = new string();
  fatherCtx = new string();

  mapCommands = new MapCommands();
}

//destructor
CContexts::~CContexts()
{
  if(name)
  {
    delete name;
    name = NULL;
  }

  if(prompt)
  {
    delete prompt;
    prompt = NULL;
  }

  if(fatherCtx)
  {
    delete fatherCtx;
    fatherCtx = NULL;
  }

  if(mapCommands)
  {
    MapCommands::iterator it;
    for( it = mapCommands->begin(); it != mapCommands->end(); ++it ) 
    {
      delete it->second;
    }
    mapCommands->clear();
    delete mapCommands;
    mapCommands = NULL;
  }
}

bool CContexts::InsertCommand(const string s,CCommands* cmd)
{
  if(!mapCommands)
  {
    mapCommands = new MapCommands();
    mapCommands->insert(make_pair(s,cmd));

    return true;
  }

  pair<MapCommands::iterator,bool> res;
  res = mapCommands->insert(make_pair(s,cmd));

  return res.second;
}
