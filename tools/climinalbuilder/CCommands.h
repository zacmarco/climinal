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

#ifndef CCOMMANDS_H
#define CCOMMANDS_H

#ifdef WIN32
#include "stdafx.h"
#endif

#include "common.h"
#include "CParameters.h"

typedef list<const CParameters*> ListParameters;

class CCommands;
typedef map<string,CCommands*> MapCommands;

class CCommands
{
public:
  CCommands();
  ~CCommands();
 
  //Name get/set
  void getName(string&) const;
  void setName(const string&);
  const string* getNamePtr() const{return name;};

  //Brief and Help get/set
  void getBrief(string&) const;
  void setBrief(const string&);
  void getHelp(string&) const;
  void setHelp(const string&);
  bool isBriefNull() const{return brief->length() == 0;};
  bool isHelpNull() const{return help->length() == 0;};
  const string* getBriefPtr() const{return brief;};
  const string* getHelpPtr() const{return help;};

  //Callback get/set
  void getCallback(string&) const;
  void setCallback(const string&);
  bool isCallbackNull() const{return callback->length() == 0;};
  const string* getCallbackPtr() const{return callback;};

  //SubContext get/set
  int setSubContext(string &s);
  bool isSubCtxNull() const{return subContext == NULL;};
  const string* getSubContextPtr() const{return subContext;};

  //Parameters entry-points
  bool HasParameters() const{return (lpParams != NULL);};
  int InsertParameter(const CParameters* const cp);
  bool HasDefaultParameter() const;
  bool IsParameterNameAlreadyPresent(const char* name) const;

  //SubCommands entry-points
  bool HasSubCommands() const{return (mcSubCommands != NULL);};
  int InsertSubCommand(const string,CCommands*);
  bool IsSubCommandNameAlreadyPresent(const char* name) const;

  ListParameters *lpParams;   //std::list for parameters
  MapCommands *mcSubCommands; //std::map for subcommands

private:
  string *name;
  string *brief;
  string *help;
  string *callback;
  string *subContext;
};

#endif
