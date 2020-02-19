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

//SINGLETON CLASS to create .c and .h files

#ifndef _CEXPORT_FILES_H_
#define _CEXPORT_FILES_H_

#ifdef WIN32
#include "stdafx.h"
#endif

#include "cli_data.h"
#include "clistruct.h"


class CExportFiles
{
private:
    static CExportFiles *m_instance;
    CExportFiles(string&,MapContexts*);

    string* filename;
    ofstream* fcpp;
    ofstream* fh;
    ostringstream* oss;
    MapContexts* mCtx;

    string* supp_str;
    static const char separator = '_';

    const string* getCtxDefName(const string&);
    const string* getCmdDefName(const string*);
    const string* getPrmDefName(const string*,const string*);
    const string* getSubCmdDefName(const string*,const string*);
    const string* getBriefDefName(const string*,const string*);
    const string* getHelpDefName(const string*,const string*);

    //recoursive methods to support SubCommands
    void WriteCommandRules(const MapCommands* mc,const string* sCtxName,const string* sCmdName = NULL);
    void WriteCommandDefs(ostringstream* ossBrief,ostringstream* ossBriefDef, ostringstream* ossHelp, ostringstream* ossHelpDef,
                          const MapCommands* mc,const string* sCtxName,const string* sCmdName = NULL);
    void WriteCallbackRules(const MapCommands* mc,const string* sCtxName,const string* sCmdName = NULL);
    void WriteValuesCallbackRules(const MapCommands* mc);
    void WriteParametersRules(const MapCommands* mc,const string* sCtxName,const string* sCmdName = NULL);
    void WriteParametersDefs(const MapCommands* mc,const string* sCtxName,const string* sCmdName = NULL);

    void SwitchFromCtxCentricToCmdCentric(const string* sCtxHierarchy,const string* sCmdHierarchy,
                                          const string* sPresentCmd,string& newHierarchy);

    void WriteParameterLine(const ListParameters::iterator lpIter);

public:
    static CExportFiles* GetInstance(string&,MapContexts*);
    ~CExportFiles();

    void InitFiles();
    void WriteDepth(const unsigned int);
    void WriteCtxDefs();
    void WriteCmdDefs();
    void WritePrmDefs();
    void WriteRules();
    void WriteCallbackHeader();
    void WriteValuesCallbackHeader();
    void WriteMainCtxEntryPoint();
    void EndFiles();

};

#endif
