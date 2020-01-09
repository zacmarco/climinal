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

//This class manages the state of the data-structure
//climinalbuilder needs it to validate incoming requests
//SINGLETON CLASS

#ifndef CCURRENTITEM_H
#define CCURRENTITEM_H

#ifdef WIN32
#include "stdafx.h"
#endif

#include "common.h"

#define SEPARATOR '_'

class CCurrentItem
{
protected:
  CCurrentItem();

public:
  static CCurrentItem* GetInstance();
  ~CCurrentItem();

  inline void EraseCurrentPrm(void){bCurrentPrm = false; };
  inline bool IsNullCurrentPrm(void) const{return this->bCurrentPrm == false;};

  void EraseCurrentCmd(void);
  inline bool IsNullCurrentCmd(void) const{return lstCmds->empty();};
  inline void setCurrentCmd(const string& s){lstCmds->push_back(s);};
  const string* getCurrentCmd();

  inline bool IsNullCurrentCtx(void) const{return stCtxs->size() == 0;};
  inline void setCurrentCtx(const string& s){stCtxs->push(s);}
  const string* getCurrentCtx() const{return &stCtxs->top();};
  int UpdateCtxWithSubCtx();
  int BackToFatherCtx();

  bool IsConsistent() const{return bConsistent;};
  void SetConsistency(bool isConsistent){bConsistent = isConsistent;};

  StackContexts *stCtxs;
  ListCommands *lstCmds;

private:
  string *sCurrentCtx;
  string *sCurrentCmd;
  bool    bCurrentPrm;

  bool bConsistent;

  static CCurrentItem* m_instance;
};

#endif
