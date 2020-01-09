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

#include "CParameters.h"

//constructor
CParameters::CParameters()
{
  name = new string();
  description = new string();
  values = new string();
}

//destructor
CParameters::~CParameters()
{
  if(name)
    delete name;
}

//name get
void CParameters::getName(string &s) const
{
  s = *this->name;
}

//name set
void CParameters::setName(const string &s)
{
  *this->name = s;
}

//description get
void CParameters::getDescription(string &s) const
{
  s = *this->description;
}

//description set
void CParameters::setDescription(const string &s)
{
  *this->description = s;
}
