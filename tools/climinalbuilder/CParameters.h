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

#ifndef CPARAMETERS_H
#define CPARAMETERS_H

#ifdef WIN32
#include "stdafx.h"
#endif

#include "common.h"

class CParameters
{
public:
  CParameters();
  ~CParameters();

  void getName(string&) const;
  void setName(const string&);
  const string* getNamePtr() const {return name;};

  void getDescription(string&) const;
  void setDescription(const string&);
  const string* getDescriptionPtr() const {return description;};
  
  inline int getRequired() const {return this->required;};
  inline void setRequired(const int val) {this->required = val;};

  inline int getNumVal() const{return this->numval;};
  inline void setNumVal(const int val) {this->numval = val;};

  inline int getReqVal() const {return this->reqval;};
  inline void setReqVal(const int val) {this->reqval = val;};

  bool isValuesNull() const{return values->length() == 0;};
  inline string* getValues() const {return this->values;};
  inline void setValues(const string& val) {*this->values = val;};

private:
  string *name;
  string *description;
  int required;
  int numval;
  int reqval;
  string *values;

};

#endif
