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

#include "CCurrentItem.h"

CCurrentItem* CCurrentItem::m_instance = NULL;

CCurrentItem* CCurrentItem::GetInstance()
{
  if(m_instance == NULL)
    m_instance = new CCurrentItem();
  return m_instance;
}

//constructor
CCurrentItem::CCurrentItem()
{
  sCurrentCtx = new string();
  sCurrentCmd = new string();
  
  bCurrentPrm = false;

  bConsistent = true;

  stCtxs = new StackContexts();
  lstCmds = new ListCommands();
}

//destructor
CCurrentItem::~CCurrentItem()
{
  if(sCurrentCtx)
    delete sCurrentCtx;

  if(sCurrentCmd)
    delete sCurrentCmd;

  if(stCtxs)
  {
    while(!stCtxs->empty())
    {
      stCtxs->pop();
    }
    delete stCtxs;
    stCtxs = NULL;
  }

  if(lstCmds)
  {
    ListCommands::iterator lit = lstCmds->begin();
    while(lit != lstCmds->end())
    {
      ListCommands::iterator thisone = lit;
      ++lit;
      lstCmds->erase(thisone);
    }
    delete lstCmds;
    lstCmds = NULL;
  }
}

void CCurrentItem::EraseCurrentCmd()
{
  if(lstCmds->size() == 0) 
    return;

  ListCommands::iterator lit = lstCmds->end();
  --lit;
  lstCmds->erase(lit);
}

const string* CCurrentItem::getCurrentCmd()
{ 
  if(!lstCmds->size())
  {
    sCurrentCmd->erase();
  }
  else
  {
    ListCommands::iterator lit = lstCmds->begin();
    *sCurrentCmd = *lit;
    while(++lit != lstCmds->end())
    {
      *sCurrentCmd += SEPARATOR;
      *sCurrentCmd += *lit;
    }
  }

  return sCurrentCmd;
}

int CCurrentItem::UpdateCtxWithSubCtx()
{
  getCurrentCmd();
  if(!sCurrentCmd->length())
    return -1; //TODO Errors

  *sCurrentCtx = *getCurrentCtx();
  *sCurrentCtx += SEPARATOR;
  *sCurrentCtx += *sCurrentCmd;
  stCtxs->push(*sCurrentCtx);

  //align Commands
  lstCmds->clear();

  return 0;
}

int CCurrentItem::BackToFatherCtx()
{
  if(!stCtxs->size())
    return -1;

  string sActualCtx = stCtxs->top();
  
  //align Contexts
  stCtxs->pop();
  
  if(!stCtxs->size())
    return 0; //you've closed the Main ctx!!!!

  //align Commands lists
  string sPreviousCtx = stCtxs->top();

  if(sPreviousCtx.length() > sActualCtx.length())
    return -2;

  string sPendingCommands = sActualCtx.substr(sPreviousCtx.length()+1);
  lstCmds->clear();
  string::size_type right,left=0;
  while((right = sPendingCommands.find("_",left)) != string::npos)
  {
    lstCmds->push_back(sPendingCommands.substr(left,right-left));
    left = right+1;
  }
  lstCmds->push_back(sPendingCommands.substr(left));

  return 0;
}