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

#include "CExportFiles.h"

CExportFiles* CExportFiles::m_instance = NULL;

//Singleton Init
CExportFiles* CExportFiles::GetInstance(string &s, MapContexts *ctx)
{
  if(m_instance == NULL)
    m_instance = new CExportFiles(s,ctx);

  return m_instance;
}

//private constructor
CExportFiles::CExportFiles(string& fn,MapContexts* ctx)
{
  filename = new string();
  *filename = fn;

  fcpp = NULL;
  fh = NULL;
  oss = new ostringstream();
  supp_str = new string();
  mCtx = ctx;
}

//destructor
CExportFiles::~CExportFiles()
{
  if(fcpp)
  {
    fcpp->close();
    delete fcpp;
    fcpp = NULL;
  }

  if(fh)
  {
    fh->close();
    delete fh;
    fh = NULL;
  }

  if(oss)
  {
    delete oss;
    oss = NULL;
  }

  if(filename)
  {
    delete filename;
    filename = NULL;
  }

  if(supp_str)
  {
    delete supp_str;
    supp_str = NULL;
  }

  mCtx = NULL; //MapContext destruction is demanded to CCliminalBuilder class destructor
}


//private function to retrieve Ctx Variable Name
const string* CExportFiles::getCtxDefName(const string& ctxname)
{
  supp_str->erase();
  if(ctxname == "NULL")
    return &ctxname;

  *supp_str = ctxname + separator + "ctx";

  return supp_str;
}

//private function to retrieve Cmd Variable Name
const string* CExportFiles::getCmdDefName(const string* ctxname)
{
  supp_str->erase();
  *supp_str = *ctxname + separator + "cmd";

  return supp_str;
}

//private function to retrieve PrmVariable Name
const string* CExportFiles::getPrmDefName(const string* ctxname,const string* cmdname)
{
  supp_str->erase();
  if(ctxname)
    *supp_str = *ctxname + separator;
  *supp_str += *cmdname + separator + "prm";

  return supp_str;
}

//private function to retrieve SubCmds Name
const string* CExportFiles::getSubCmdDefName(const string* ctxname,const string* cmdname)
{
  supp_str->erase();
  if(ctxname)
    *supp_str = *ctxname + separator;
  *supp_str += *cmdname + separator + "subcmd";

  return supp_str;
}

//private function to retrieve Help Variable Name
const string* CExportFiles::getBriefDefName(const string* ctxname,const string* cmdname)
{
  supp_str->erase();
  if(ctxname)
    *supp_str = *ctxname + separator;
  *supp_str += *cmdname + separator + "brf";

  return supp_str;
}

//private function to retrieve Help Variable Name
const string* CExportFiles::getHelpDefName(const string* ctxname,const string* cmdname)
{
  supp_str->erase();
  if(ctxname)
    *supp_str = *ctxname + separator;
  *supp_str += *cmdname + separator + "hlp";

  return supp_str;
}

//function to initialize .c and .h files
void CExportFiles::InitFiles()
{
  if(oss == NULL)
    return;

  oss->str("");
  *oss << "\n/*CliminalBuilder version " << VER_MAJ << "." << VER_MIN << "." << VER_BUILD << "*/\n";

  if(fcpp == NULL)
  {
    string cpp;
    cpp = *filename + ".c";
    fcpp = new ofstream(cpp.c_str(),ios_base::trunc); //Overwrite possible old file
    *fcpp << "/*AutoGenerated c file for CLIMINAL*/";
    
    /*
    #ifdef WIN32
    fcpp->setf(ios_base::app);
    #else
    *fcpp << ios_base::app;
    #endif
    */
    *fcpp << oss->str();
  }

  if(fh == NULL)
  {
    string h;
    h = *filename + ".h";
    fh = new ofstream(h.c_str(),ios_base::trunc);
    *fh << "/*AutoGenerated h file for CLIMINAL*/";
    *fh << oss->str();

    //FILENAME to UPPER CASE
    for(unsigned int i=0; i<filename->length(); ++i) 
      if((filename->c_str()[i] >= 'a') && (filename->c_str()[i] <= 'z'))
        *supp_str += (filename->c_str()[i] - 32);
      else
        *supp_str += filename->c_str()[i];

    *fh << "\n#ifndef _" << *supp_str << "_H_\n";
    *fh << "#define _" << *supp_str << "_H_\n\n";

    /*
    #ifdef WIN32
    fh->setf(ios_base::app);
    #else
    *fh << ios_base::app;
    #endif
    */

    *fh << "#ifdef __cplusplus\nextern \"C\"\n{\n#endif\n\n";
    *fh << "#include <stdio.h>\n#include \"climinal/cli.h\"\n\n";

  }

  oss->str("");
  *oss << "\n#include \"" << *filename << ".h\"\n\n";
  *fcpp << oss->str();
  oss->str("");

  
}

void CExportFiles::WriteDepth(const unsigned int depth)
{
  if(oss == NULL)
    return;

  if(fh)
  {
    std::string value(*filename);
    for (std::string::iterator p = value.begin(); value.end() != p; ++p)
        *p = toupper(*p);
    *fh << "#define " << value << "_DEPTH (" << depth << ")" << std::endl;
  }
  
}

//public function to write Handle definitions
void CExportFiles::WriteHndlDefs()
{
  if(fcpp == NULL)
    return;

  if(mCtx == NULL)
    return;

  *oss << "/* Handle */\n";

  *oss << "static Cliconfig config;\n";
  *oss << "static Clihandle handle;\n";

  *oss << "\n";
  *fcpp << oss->str();
  oss->str("");
}

//public function to write Context definitions
void CExportFiles::WriteCtxDefs()
{
  if(fcpp == NULL)
    return;

  if(mCtx == NULL)
    return;

  *oss << "/* Contexts */\n";

  MapContexts::iterator it;
  for( it = mCtx->begin(); it != mCtx->end(); ++it)
  {
    *oss << "static Clicontext " << *getCtxDefName(it->first) << ";\n";
  }

  *oss << "\n";
  *fcpp << oss->str();
  oss->str("");
}

//public function to write Commands definitions
void CExportFiles::WriteCmdDefs()
{
  if(fcpp == NULL)
    return;

  if(mCtx == NULL)
    return;
  
  *oss << "/* Commands */\n";

  ostringstream ossBriefDef, ossBrief, ossHelpDef,ossHelp;
  ossHelpDef << "/* Help */\n";

  MapContexts::iterator it;
  for( it = mCtx->begin(); it != mCtx->end(); ++it)
  {
    WriteCommandDefs(&ossBrief, &ossBriefDef, &ossHelp,&ossHelpDef,it->second->mapCommands,&it->first);
  }
  
  *oss << "\n";
  ossBriefDef << "\n";
  ossBrief << "\n";
  ossHelpDef << "\n";
  ossHelp << "\n";
  *fcpp << oss->str();
  *fcpp << ossBriefDef.str();
  *fcpp << ossBrief.str();
  *fcpp << ossHelpDef.str();
  *fcpp << ossHelp.str();
  oss->str("");
}

void CExportFiles::WritePrmDefs()
{
  if(fcpp == NULL)
    return;

  if(mCtx == NULL)
    return;
  
  *oss << "/* Parameters */\n";

  MapContexts::iterator it;
  MapCommands::iterator itcmd;
  for( it = mCtx->begin(); it != mCtx->end(); ++it)
    WriteParametersDefs(it->second->mapCommands,&it->first);

  *oss << "\n";
  *fcpp << oss->str();
  oss->str("");
}


void CExportFiles::WriteRules()
{
  if(fcpp == NULL)
    return;

  if(mCtx == NULL)
    return;
  
  *oss << "\n/* Rules */\n";

  MapContexts::iterator it;
  for( it = mCtx->begin(); it != mCtx->end(); ++it)
  {
    string father_name = *getCtxDefName(*it->second->getFatherCtxPtr());
    if( father_name != "NULL" )
        father_name = "&" + father_name;
    
    //write Contexts Rules
    *oss << "\n/*CONTEXT " << *getCtxDefName(it->first) << "*/\n";
    *oss << "static Clicontext " << *getCtxDefName(it->first) << " = { " << father_name;
    *oss << ", " << *getCmdDefName(&it->first) << ", \"" << *it->second->getPromptPtr() << "\", " << mCtx->size() << " };\n";

    //write Commands Rules
    WriteCommandRules(it->second->mapCommands,&it->first);

    //write Parameters Rules
    WriteParametersRules(it->second->mapCommands,&it->first);
  }
  *oss << "\n";
  *fcpp << oss->str();
  oss->str("");
}

void CExportFiles::WriteCallbackHeader()
{
  if(fh == NULL)
    return;

  if(mCtx == NULL)
    return;
  
  *oss << "\n/* Callbacks */\n\n";

  MapContexts::iterator it;
  MapCommands::iterator itcmd;
  for( it = mCtx->begin(); it != mCtx->end(); ++it)
    WriteCallbackRules(it->second->mapCommands,&it->first);

  *oss << "\n";
  *fh << oss->str();
  oss->str("");
}

void CExportFiles::WriteValuesCallbackRules(const MapCommands* mc)
{
    MapCommands::const_iterator itcmd;
    ListParameters::iterator itpar;

    for(itcmd = mc->begin(); itcmd != mc->end(); ++itcmd)
    {
        if(itcmd->second->HasParameters()) {
            for(itpar = itcmd->second->lpParams->begin(); itpar != itcmd->second->lpParams->end(); ++itpar)
            {
                if( (*(*itpar)->getValues()).length() > 0 )
                {
                    *oss << "int " << (*(*itpar)->getValues()) << "(char **val, void *cookie);\n";
                }
            }
        }
        if(itcmd->second->HasSubCommands())
        {
            WriteValuesCallbackRules(itcmd->second->mcSubCommands);
        }
    }
}

void CExportFiles::WriteValuesCallbackHeader()
{
  if(fh == NULL)
    return;

  if(mCtx == NULL)
    return;
  
  *oss << "\n/* Values Callbacks */\n\n";

  MapContexts::iterator it;
  MapCommands::iterator itcmd;
  for( it = mCtx->begin(); it != mCtx->end(); ++it)
    WriteValuesCallbackRules(it->second->mapCommands);

  *oss << "\n";
  *fh << oss->str();
  oss->str("");
}


void CExportFiles::WriteConfig( CConfig *config )
{
  if(fcpp == NULL)
    return;

  if(fh == NULL)
    return;

  if(mCtx == NULL)
    return;
  
  *oss << "static Cliconfig config = { \n\t.history_size=" << config->getHistorySize() << ",\n\t.context_depth=" << config->getContextDepth() <<"\n};\n\n";
  *oss << "static Clihandle handle = { &main_ctx, &config };\n";
  *fcpp << oss->str();
  oss->str("");

}


void CExportFiles::WriteHandleEntryPoint()
{
  if(fcpp == NULL)
    return;

  if(fh == NULL)
    return;

  if(mCtx == NULL)
    return;
  
  *oss << "\nClihandle *climinalhandle_" << *filename << "()\n{\n\treturn &handle;\n}\n";
  *fcpp << oss->str();
  oss->str("");

  *oss << "Clihandle *climinalhandle_" << *filename << "();\n\n";
  *fh << oss->str();
  oss->str("");
}

void CExportFiles::EndFiles()
{
  if(fcpp == NULL)
    return;

  if(fh == NULL)
    return;

  *fh << "\n#ifdef __cplusplus\n}\n#endif\n\n#endif\n";
}


//writes Commands Rules --> RECOURSIVE FUNCTION to write commands and all related subcommands
void CExportFiles::WriteCommandRules(const MapCommands* mc,const string* sCtxName,const string* sCmdName)
{
  if(sCmdName)
  {
    *oss << "static Clicmd " << *getSubCmdDefName(sCtxName,sCmdName) << "[] = \n{\n";
  }
  else
  {
    *oss << "static Clicmd " << *getCmdDefName(sCtxName) << "[] = \n{\n";
  }

  MapCommands::const_iterator itcmd;

  *oss << "\t\t/*NAME\tBRIEF\tHELP\tPARAMETERS\tSUBCONTEXT\tSUBCOMMANDS\tCALLBACK*/\n";
  for(itcmd = mc->begin(); itcmd != mc->end(); ++itcmd)
  {
    string sNewCmdName = (sCmdName) ? *sCmdName + separator + itcmd->first : itcmd->first;
    *oss << "\t\t{\"" << *itcmd->second->getNamePtr() << "\",\t";

    if(!itcmd->second->isBriefNull())
      *oss << *getBriefDefName(sCtxName,&sNewCmdName) << ",\t";
    else
      *oss << "NULL,\t";

    if(!itcmd->second->isHelpNull())
      *oss << *getHelpDefName(sCtxName,&sNewCmdName) << ",\t";
    else
      *oss << "NULL,\t";

    if(itcmd->second->HasParameters())
      *oss << *getPrmDefName(sCtxName,&sNewCmdName) << ",\t";
    else
      *oss << "NULL,\t";

    if(!itcmd->second->isSubCtxNull())
      *oss << "&" << *getCtxDefName(*itcmd->second->getSubContextPtr()) << ",\t";
    else
      *oss << "NULL,\t";

    if(itcmd->second->HasSubCommands())
      *oss << /*"&" <<*/ *getSubCmdDefName(sCtxName,&sNewCmdName) << ",\t";
    else
      *oss << "NULL,\t";

    if(!itcmd->second->isCallbackNull())
      *oss << *itcmd->second->getCallbackPtr() << "}";
    else
      *oss << "NULL}";

    *oss << ",\n";
  }
  *oss << "\t\t{NULL,\tNULL,\tNULL,\tNULL,\tNULL}\n};\n\n";

  //search for subcommands
  for(itcmd = mc->begin(); itcmd != mc->end(); ++itcmd)
  {
    if(!itcmd->second->HasSubCommands())
      continue;

    string sNewCmdName;
    SwitchFromCtxCentricToCmdCentric(sCtxName,sCmdName,itcmd->second->getNamePtr(),sNewCmdName);

    WriteCommandRules(itcmd->second->mcSubCommands,NULL,&sNewCmdName);
  }
}

void CExportFiles::WriteCommandDefs(ostringstream* ossBrief,
                                    ostringstream* ossBriefDef,
                                    ostringstream* ossHelp,
                                    ostringstream* ossHelpDef,
                                    const MapCommands* mc,
                                    const string* sCtxName,
                                    const string* sCmdName)
{
  if(sCmdName)
  {
    *oss << "static Clicmd " << *getSubCmdDefName(sCtxName,sCmdName) << "[];\n";
  }
  else
  {
    *oss << "static Clicmd " << *getCmdDefName(sCtxName) << "[];\n";
  }

  MapCommands::const_iterator itcmd;

  for(itcmd = mc->begin(); itcmd != mc->end(); ++itcmd)
  {
    if(itcmd->second->isBriefNull() || itcmd->second->isHelpNull())
      continue;

    if(sCmdName)
    {
      string sNewCmdName = *sCmdName + separator + itcmd->first;
      *ossBriefDef << "static char " << *getBriefDefName(sCtxName,&sNewCmdName) << "[];\n"; 
      *ossBrief << "static char " << *getBriefDefName(sCtxName,&sNewCmdName) << "[] = \n"; 
    }
    else
    {
      *ossBriefDef << "static char " << *getBriefDefName(sCtxName,&itcmd->first) << "[];\n"; 
      *ossBrief << "static char " << *getBriefDefName(sCtxName,&itcmd->first) << "[] = \n"; 
    }
    *ossBrief << "\"" << *itcmd->second->getBriefPtr() << "\\\n\";\n\n";

    if(sCmdName)
    {
      string sNewCmdName = *sCmdName + separator + itcmd->first;
      *ossHelpDef << "static char " << *getHelpDefName(sCtxName,&sNewCmdName) << "[];\n"; 
      *ossHelp << "static char " << *getHelpDefName(sCtxName,&sNewCmdName) << "[] = \n"; 
    }
    else
    {
      *ossHelpDef << "static char " << *getHelpDefName(sCtxName,&itcmd->first) << "[];\n"; 
      *ossHelp << "static char " << *getHelpDefName(sCtxName,&itcmd->first) << "[] = \n"; 
    }
    *ossHelp << "\"" << *itcmd->second->getHelpPtr() << "\\\n\";\n\n";

    if(!itcmd->second->HasSubCommands())
      continue;

    string sNewCmdName;
    SwitchFromCtxCentricToCmdCentric(sCtxName,sCmdName,itcmd->second->getNamePtr(),sNewCmdName);

    WriteCommandDefs(ossBrief, ossBriefDef, ossHelp,ossHelpDef,itcmd->second->mcSubCommands,NULL,&sNewCmdName);
  }
}

void CExportFiles::WriteCallbackRules(const MapCommands* mc,const string* sCtxName,const string* sCmdName)
{
  MapCommands::const_iterator itcmd;

  for(itcmd = mc->begin(); itcmd != mc->end(); ++itcmd)
  {
    if(!itcmd->second->isCallbackNull() && (*itcmd->second->getCallbackPtr() != string("NULL")))
    {   
      //obtain CONTEXT (or SUBCOMMANDS) TREE with a slash (/) instead of separator "_"
      string s;
      if(sCtxName)
        s = *sCtxName;
      else if(sCmdName)
        s = *sCmdName;
      else continue;

      string::size_type found = s.find("_");
      while(found != string::npos)
      {
        s.replace(found,1,"/");
        found = s.find(separator,found);
      }
      //obtain COMMAND NAME
      *oss << "/* COMMAND: " << itcmd->first << "\t\t( " << s << " )\t\t*/\n";

      //obtain COMMAND's CALLBACK NAME
      *oss << "int " << *itcmd->second->getCallbackPtr() << "(FILE* in, FILE* out, const Cmdinfo *info, const char* line, void *cookie);\n\n";
    }

    if(itcmd->second->HasSubCommands())
    {
      string sNewCmdName;
      SwitchFromCtxCentricToCmdCentric(sCtxName,sCmdName,itcmd->second->getNamePtr(),sNewCmdName);

      WriteCallbackRules(itcmd->second->mcSubCommands,NULL,&sNewCmdName);
    }
  }
}

void CExportFiles::WriteParametersRules(const MapCommands* mc,const string* sCtxName,const string* sCmdName)
{
  MapCommands::const_iterator itcmd;
  ListParameters::iterator itpar;

  for(itcmd = mc->begin(); itcmd != mc->end(); ++itcmd)
  {
    if(itcmd->second->HasParameters())
    {
      string sNewCmdName = (sCmdName) ? *sCmdName + separator + itcmd->first : itcmd->first; 

      *oss << "static Cliparam " << *getPrmDefName(sCtxName,&sNewCmdName) << "[] = \n{\n";
      *oss << "\t\t /*NAME\tDESC\tREQUIRED\tNUMVAL\tVALUES\t*/\n";
  
      if(itcmd->second->HasDefaultParameter()) //check to write default parameter as first row in .c file
      {
        for(itpar = itcmd->second->lpParams->begin(); itpar != itcmd->second->lpParams->end(); ++itpar)
        {
          if((*(*itpar)->getNamePtr()).length() == 0)
          {
            WriteParameterLine(itpar);
            break;
          }
        }       
        for(itpar = itcmd->second->lpParams->begin(); itpar != itcmd->second->lpParams->end(); ++itpar)
        {
          if((*(*itpar)->getNamePtr()).length() == 0)
            continue;
          WriteParameterLine(itpar);
        }  
      }
      else //NO Default parameter
      {
        for(itpar = itcmd->second->lpParams->begin(); itpar != itcmd->second->lpParams->end(); ++itpar)
        {
          WriteParameterLine(itpar);
        }
      }

      *oss << "\t\t{NULL,\tNULL,\t0,\t0,\tNULL}\n};\n";
    }
      
    if(!itcmd->second->HasSubCommands())
      continue;

    string sNewCmdName;
    SwitchFromCtxCentricToCmdCentric(sCtxName,sCmdName,itcmd->second->getNamePtr(),sNewCmdName);

    WriteParametersRules(itcmd->second->mcSubCommands,NULL,&sNewCmdName);
  }
}

void CExportFiles::WriteParameterLine(const ListParameters::iterator lpIter)
{
  if((*(*lpIter)->getNamePtr()).length())
    *oss << "\t\t{\"" << *(*lpIter)->getNamePtr() << "\",\t";
  else
    *oss << "\t\t{\"\",\t";

  if((*(*lpIter)->getDescriptionPtr()).length())
      *oss << "\"" << *(*lpIter)->getDescriptionPtr() << "\",\t";
  else
      *oss << "\"\",\t";

  *oss << (*lpIter)->getRequired() << ",\t";
  *oss << (*lpIter)->getNumVal() << ",\t";
  if(((*lpIter)->getValues())->length() > 0 )
  {
    *oss << *((*lpIter)->getValues()) << "},";
  }
  else
  {
    *oss << "NULL" << "},";
  }

  if((*(*lpIter)->getNamePtr()).length() == 0)
    *oss << "\t /*DEFAULT PARAMETER*/";

  *oss << "\n";
}

void CExportFiles::WriteParametersDefs(const MapCommands* mc,const string* sCtxName,const string* sCmdName)
{
  MapCommands::const_iterator itcmd;

  for(itcmd = mc->begin(); itcmd != mc->end(); ++itcmd)
  {
    if(itcmd->second->HasParameters())
    {
      string s = (sCmdName) ? *sCmdName + separator + itcmd->first : itcmd->first;
      *oss << "static Cliparam " << *getPrmDefName(sCtxName,&s) << "[];\n";
    }

    if(!itcmd->second->HasSubCommands())
      continue;

    string sNewCmdName;
    SwitchFromCtxCentricToCmdCentric(sCtxName,sCmdName,itcmd->second->getNamePtr(),sNewCmdName);
  
    WriteParametersDefs(itcmd->second->mcSubCommands,NULL,&sNewCmdName);
  }
}

//private function to switch naming from CTX Centric to CMD Centric
void CExportFiles::SwitchFromCtxCentricToCmdCentric(const string* sCtxHierarchy,
                                                    const string* sCmdHierarchy,
                                                    const string* sPresentCmd,
                                                    string& newHierarchy)
{
  if(sCtxHierarchy)
    newHierarchy = *sCtxHierarchy + separator;
  if(sCmdHierarchy)
    newHierarchy += *sCmdHierarchy + separator;
  newHierarchy += *sPresentCmd;
}
