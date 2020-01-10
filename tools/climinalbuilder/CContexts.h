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

#ifndef CCONTEXTS_H
#define CCONTEXTS_H

#ifdef WIN32
#include "stdafx.h"
#endif

#include "common.h"
#include "CCommands.h"

class CContexts
{
public:
  CContexts();
  ~CContexts();

  void getName(string &s){s = *this->name;};
  void setName(const string &s){*this->name = s;}; 

  void getPrompt(string &s) const{s = *this->prompt;};
  void setPrompt(const string &s){*this->prompt = s;}; 
  const string* getPromptPtr() const{return this->prompt;};

  void getFatherCtx(string &s){s = *this->prompt;};
  const string * getFatherCtxPtr() const {return this->fatherCtx;};
  void setFatherCtx(const string *s){*this->fatherCtx = (s == NULL) ? "NULL" : *s;}; 

  bool InsertCommand(const string,CCommands*);
  MapCommands *mapCommands;

private:
  string *name;
  string *prompt;

  string *fatherCtx;
};

typedef map<string,CContexts*> MapContexts;

#endif
