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

#include "CCommands.h"

//constructor
CCommands::CCommands()
{
  //Mandatory fiedls
  name  = new string();
  brief = new string();
  help  = new string();
  callback = new string();
  
  //Optional fields
  subContext = NULL;
  lpParams = NULL;
  mcSubCommands = NULL;
}

//destructor
CCommands::~CCommands()
{
  if(name)
  {
    delete name;
    name = NULL;
  }

  if(brief)
  {
    delete brief;
    brief = NULL;
  }

  if(help)
  {
    delete help;
    help = NULL;
  }

  if(callback)
  {
    delete callback;
    callback = NULL;
  }

  if(subContext)
  {
    delete subContext;
    subContext = NULL;
  }

  if(lpParams)
  {
    ListParameters::iterator it;
    for(it = lpParams->begin(); it != lpParams->end(); ++it)
    {
      delete *it;
    }
    lpParams->clear();
    delete lpParams;
    lpParams = NULL;
  }

  if(mcSubCommands)
  {
    MapCommands::iterator it;
    for( it = mcSubCommands->begin(); it != mcSubCommands->end(); ++it ) 
    {
      delete it->second;
    }
    mcSubCommands->clear();
    delete mcSubCommands;
    mcSubCommands = NULL;
  }
}

//name get
void CCommands::getName(string &s) const
{
  s = *this->name;
}

//name set
void CCommands::setName(const string &s)
{
  *this->name = s;
}

//brief get
void CCommands::getBrief(string &s) const
{
  s = *this->brief;
}

//brief set
void CCommands::setBrief(const string &s)
{
  *this->brief = s;
}

//help get
void CCommands::getHelp(string &s) const
{
  s = *this->help;
}

//help set
void CCommands::setHelp(const string &s)
{
  *this->help = s;
}

//callback get
void CCommands::getCallback(string &s) const
{
  s = *this->callback;
}

//callback set
void CCommands::setCallback(const string &s)
{
  *this->callback = s;
}

//subcontext set
int CCommands::setSubContext(string &s)
{
  if(subContext)
    return -1; //subcontext already present

  subContext = new string();
  *subContext = s;

  return 0;
}

//Insert a new Parameter related to this command
int CCommands::InsertParameter(const CParameters* const cp)
{
  if(!lpParams) //it's the first command's parameter
  {
    lpParams = new ListParameters();
    lpParams->push_back(cp);

    return 0;
  }

  //other parameters already present for this command
  if(IsParameterNameAlreadyPresent(cp->getNamePtr()->c_str()))
    return -1;

  lpParams->push_back(cp);
  return 0;
}

//parameter's name already present for this command?
bool CCommands::IsParameterNameAlreadyPresent(const char* name) const
{
  if(!lpParams)
    return false;

  ListParameters::iterator it;
  for(it = lpParams->begin(); it != lpParams->end(); ++it)
  {
    if(*(*it)->getNamePtr() == name)
      return true; //duplicated parameters (same name!!!)
  }
  return false;
}

//subcommand's name already present for this command?
bool CCommands::IsSubCommandNameAlreadyPresent(const char* name) const
{
  if(!mcSubCommands)
    return false;

  MapCommands::iterator mit;
  mit = mcSubCommands->find(name);
  if(mit != mcSubCommands->end())
    return true;

  return false;
}

//Insert a new SubCommand for this command
int CCommands::InsertSubCommand(const string s,CCommands* cmd)
{
  if(!mcSubCommands) //first subcommand for this command
  {
    mcSubCommands = new MapCommands();
    mcSubCommands->insert(make_pair(s,cmd));

    return 0;
  }

  /*
  Other subcommands already present
  Let's perform unicity check while inserting. 
  Since subcommands are stored in a std::map data-structure 
  we'll use native insert function which performs uniqueness-check
  */
  pair<MapCommands::iterator,bool> res;
  res = mcSubCommands->insert(make_pair(s,cmd));

  //res.second = FALSE if "insert" has failed (duplicated subcommand).
  return (res.second) ? 0 : -1;
}

//checks if this command has a default parameter
bool CCommands::HasDefaultParameter() const
{
  if(!lpParams)
    return false;

  //search in std::list
  ListParameters::iterator it;
  for(it = lpParams->begin(); it != lpParams->end(); ++it)
  {
    if((*(*it)->getNamePtr()).length() == 0)
      return true; //parameter with zero-lenght name already exist
  }

  return false;
}
