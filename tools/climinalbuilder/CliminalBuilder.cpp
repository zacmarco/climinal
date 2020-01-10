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

#include "CliminalBuilder.h"

CCliminalBuilder* CCliminalBuilder::m_instance = NULL;

CCliminalBuilder* CCliminalBuilder::getInstance()
{
  if(m_instance == NULL)
    m_instance = new CCliminalBuilder();
  return m_instance;
}

CCliminalBuilder::CCliminalBuilder(void)
{
  cStatus = CCurrentItem::GetInstance();
  mapCtx = new MapContexts();
}

CCliminalBuilder::~CCliminalBuilder(void)
{
  if(cStatus)
  {
    delete cStatus;
    cStatus = NULL;
  }

  if(mapCtx)
  {
    MapContexts::iterator it;
    for( it = mapCtx->begin(); it != mapCtx->end(); ++it ) 
      delete it->second;

    mapCtx->clear();
    delete mapCtx;

    mapCtx = NULL;
  }


}

CCommands* CCliminalBuilder::getCurrentCommandDataStructurePointer()
{
  const string *sCurrCtxName = cStatus->getCurrentCtx();

  CContexts* ctx = mapCtx->find(*sCurrCtxName)->second;
  ListCommands::iterator lit = cStatus->lstCmds->begin();

  MapCommands::iterator mit = ctx->mapCommands->find(*lit);
  if(mit == ctx->mapCommands->end())
    return NULL;

  CCommands* cmd = mit->second;
  if(!cmd)
    return NULL;

  ++lit;
  for(; lit != cStatus->lstCmds->end(); ++lit)
  {
    mit = cmd->mcSubCommands->find(*lit);
    if(mit == cmd->mcSubCommands->end())
      return NULL;
    
    cmd = mit->second;
    if(!cmd)
      return NULL;
  }
  return cmd;
}

int CCliminalBuilder::InsertSubContext(const string s,CContexts* ctx)
{
  if(mapCtx->size() == 0)
    return CR_CTX_MAIN_CTX_NOT_FOUND; //there isn't the MAIN CTX

  pair<MapContexts::iterator,bool> res;
  res = mapCtx->insert(make_pair(s,ctx));

  return (res.second) ? CR_CTX_NO_ERROR : CR_CTX_DUPLICATED_CTX;
}