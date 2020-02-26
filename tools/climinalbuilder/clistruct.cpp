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

#ifdef WIN32
#include "stdafx.h"
#endif

#include "CliminalBuilder.h"
#include "errsnmbr.h"
#include "clistruct.h"

CCliminalBuilder *cli;

/*
FUNCTION: get_config

Copies the content of current config for custom manipulation
*/
int get_config(config_t *cfg)
{
  if(!cli)
  {
    cli = CCliminalBuilder::getInstance();
  }

  CConfig *config = cli->getConfigPointer();
  cfg->history_size = config->getHistorySize();
  return 0;
}

/*
FUNCTION: set_config

Overwrites current config with the one provided
*/
int set_config(const config_t *cfg)
{
  if(!cli)
  {
    cli = CCliminalBuilder::getInstance();
  }

  CConfig *config = cli->getConfigPointer();
  config->setHistorySize(cfg->history_size);

  return 0;
}

/*
FUNCTION: create_param

You can create a param:
 - if CLI BUILDER data structure exists (it means that a "Main" Context exists)
 - if there isn't already an opened parameter
 - if incoming parameter has a father command
 - if there isn't a parameter with the same name for father command
*/
int create_param ( const char *name, const char *description, const int required , const int numval, const char *values)
{
  if(!cli)
  {
    return CR_PAR_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus)
  {
    return CR_PAR_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus->IsConsistent())
  {
    return CLI_BUILDER_INCOSISTENT_STATE;
  }

  if(!cli->cStatus->IsNullCurrentPrm())
  {
    return CR_PAR_CURR_PAR_NOT_NULL;
  }

  if(cli->cStatus->IsNullCurrentCmd())
  {
    return CR_PAR_PARENT_CMD_NOT_FOUND;
  }

  CCommands* cmd = cli->getCurrentCommandDataStructurePointer();
  if(!cmd)
  {
    return CR_PAR_UNABLE_TO_FIND_RELATED_CMD;
  }

  if(!name && cmd->HasDefaultParameter())
  {
    return CR_PAR_DEFAULT_PAR_ALREADY_EXISTS;
  }

  if(name && cmd->IsSubCommandNameAlreadyPresent(name))
  {
    return CR_PAR_HAS_SAME_NAME_OF_A_SUBCMD;
  }

  CParameters *cp = new CParameters();
  if(name)
    cp->setName(name);
  if(description)
    cp->setDescription(description);
  cp->setRequired(required);
  cp->setNumVal(numval);
  if(values) 
  {
    cp->setValues(values);
  }

  if(cmd->InsertParameter(cp) != 0)
  {
    delete cp;
    return CR_PAR_DUPLICATED_PAR_IN_CMD;
  }

  cli->incrementDepth();

  return CR_PAR_NO_ERROR;
}


/*
FUNCTION: create_command (and subcommands)

You can create a command:
 - if CLI BUILDER data structure exists (it means that a "Main" Context exists)
 - if there isn't an already opened parameter
 - if there is a father context

 #SUBCOMMAND CASE
 - if it has a father command
 - if there isn't another subcommand with the same name for father command

 #COMMAND CASE
 - if there isn't another command with the same name for father context
*/
int create_command ( const char *name, const char *brief, const char *help, const char *callback )
{
  if(!cli)
  {
    return CR_CMD_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus)
  {
    return CR_CMD_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus->IsConsistent())
  {
    return CLI_BUILDER_INCOSISTENT_STATE;
  }

  if(!cli->cStatus->IsNullCurrentPrm())
  {
    return CR_CMD_CURR_PAR_NOT_NULL;
  }

  if(cli->cStatus->IsNullCurrentCtx())
  {
    return CR_CMD_CURR_CTX_NOT_FOUND;
  }

  //create new Command DATA
  CCommands *cmd = new CCommands();
  cmd->setName(name);
  cmd->setBrief(brief);
  cmd->setHelp(help);
  cmd->setCallback(callback);

  //understand if it's a command or a subcommand
  if(cli->cStatus->IsNullCurrentCmd())
  { 
    //it's a new command
    const string *sCtx = cli->cStatus->getCurrentCtx();
    CContexts* ctx = cli->mapCtx->find(*sCtx)->second;

    if(!ctx->InsertCommand(name,cmd))
    {
      delete cmd;
      return CR_CMD_DUPLICATED_CMD_IN_CTX;
    }
  }
  else
  {
    //it's a subcommand
    CCommands* cmd_ds = cli->getCurrentCommandDataStructurePointer();
    if(!cmd_ds)
    {
      delete cmd;
      return CR_CMD_UNABLE_TO_FIND_RELATED_CMD;
    }

    //check if it has the same name of a father command's parameter
    if(cmd_ds->IsParameterNameAlreadyPresent(name))
    {
      delete cmd;
      return CR_CMD_HAS_SAME_NAME_OF_A_PARAM;
    }

    if(cmd_ds->InsertSubCommand(name,cmd) != 0)
    {
      delete cmd;
      return CR_CMD_DUPLICATED_SUBCMD_IN_CMD;
    }
  }

  cli->cStatus->setCurrentCmd(name);

  return CR_CMD_NO_ERROR;
}

/*
FUNCTION: create_context

There are 2 kinds of contexts: MAIN (or first) context and Subcontexts (the other contexts)

You can create the MAIN context
 - if CLI BUILDER data structure doesn't exists (it means that incoming Context is the first)

You can create a SUBCONTEXT
 - if CLI BUILDER data structure exists (it means that a "Main" Context exists)
 - if there isn't an already opened parameter
 - if there is a father command (a subcontext MUST be son of a father command)
 - if father command hasn't already a subcontext
*/
int create_context ( const char *prompt )
{
  if(!cli) //first context --> main
  {
    cli = CCliminalBuilder::getInstance();
  
    if(!cli->cStatus)
    {
      return CR_CTX_SINGLETON_NOT_INSTANCED;
    }

    cli->cStatus->setCurrentCtx("main");

    CContexts *ctx = new CContexts();
    ctx->setPrompt(prompt);
    ctx->setName("main");
    ctx->setFatherCtx(NULL);

    cli->mapCtx->insert(make_pair(*cli->cStatus->getCurrentCtx(),ctx));

    return CR_CTX_NO_ERROR;
  }

  if(!cli->cStatus->IsConsistent())
  {
    return CLI_BUILDER_INCOSISTENT_STATE;
  }

  if(!cli->cStatus->IsNullCurrentPrm())
  {
    return CR_CTX_CURR_PAR_NOT_NULL;
  }

  if(cli->cStatus->IsNullCurrentCmd())
  {
    return CR_CTX_PARENT_CMD_NOT_FOUND;
  }

  CCommands *ccmd = cli->getCurrentCommandDataStructurePointer();
  if(!ccmd)
  {
    return CR_CTX_UNABLE_TO_FIND_RELATED_CMD;
  }

  //retrieve indexes for data-structure
  string previousCtx = *cli->cStatus->getCurrentCtx();
  cli->cStatus->UpdateCtxWithSubCtx();
  string presentCtx = *cli->cStatus->getCurrentCtx();

  //create incoming CONTEXT in data-structure
  CContexts *ctx = new CContexts();
  ctx->setPrompt(prompt);
  ctx->setName(presentCtx);
  ctx->setFatherCtx(&previousCtx);

  //update command with its subcontext
  if(ccmd->setSubContext(presentCtx))
  {
    delete ctx;
    return CR_CTX_FATHER_CMD_ALR_HAS_SUBCTX;
  }

  //insert new context
  int res = cli->InsertSubContext(presentCtx,ctx);

  if(!res)
  {
    //TODO Error Handling
  }

  return res;
}


/*
FUNCTION: end_param

You can end a param:
 - if CLI BUILDER data structure exists (it means that a "Main" Context exists)
 - if it was opened before
*/
int end_param(void)
{
  if(!cli)
  {
    return END_PAR_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus) 
  {
    return END_PAR_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus->IsConsistent())
  {
    return CLI_BUILDER_INCOSISTENT_STATE;
  }

  if(cli->cStatus->IsNullCurrentPrm())
  {
    return END_PAR_CURR_PAR_NOT_FOUND;
  }

  cli->cStatus->EraseCurrentPrm();
  return END_PAR_NO_ERROR;
}

/*
FUNCTION: end_command

You can end a command:
 - if CLI BUILDER data structure exists (it means that a "Main" Context exists)
 - if there isn't an already opened parameter
 - if it was opened before
*/
int end_command(void)
{
  if(!cli)
  {
    return END_CMD_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus) 
  {
    return END_CMD_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus->IsConsistent())
  {
    return CLI_BUILDER_INCOSISTENT_STATE;
  }

  if(!cli->cStatus->IsNullCurrentPrm())
  {
    return END_CMD_CURR_PAR_NOT_NULL;
  }

  if(cli->cStatus->IsNullCurrentCmd())
  {
    return END_CMD_CURR_CMD_NOT_FOUND;
  }

  cli->cStatus->EraseCurrentCmd();
  return END_CMD_NO_ERROR;
}

/*
FUNCTION: end_context

You can end a context:
 - if CLI BUILDER data structure exists (it means that a "Main" Context exists)
 - if there isn't an already opened parameter
 - if there isn't an already opened command
 - if it was opened before
*/
int end_context(void)
{
  if(!cli)
  {
    return END_CTX_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus) 
  {
    return END_CTX_SINGLETON_NOT_INSTANCED;
  }

  if(!cli->cStatus->IsConsistent())
  {
    return CLI_BUILDER_INCOSISTENT_STATE;
  }

  if(!cli->cStatus->IsNullCurrentPrm())
  {
    return END_CTX_CURR_PRM_NOT_NULL;
  }

  if(!cli->cStatus->IsNullCurrentCmd())
  {
    return END_CTX_CURR_CMD_NOT_NULL;
  }

  if(cli->cStatus->IsNullCurrentCtx())
  {
    return END_CTX_CURR_CTX_NOT_FOUND;
  }

  if(cli->cStatus->BackToFatherCtx())
  {
    return END_CTX_FATHER_CTX_NOT_FOUND;
  }

  return END_CTX_NO_ERROR;
}

/*
FUNCTION: write_files

Writes .c and .h files if DataStructure is consistent AND FREES MEMORY
You MUST call this function at the end of parsing even if an intermediate control has failed!
*/
int write_files(const char *filenames)
{
  if(!cli || !cli->cStatus)
  {
    return WRT_FILE_NOTHING_TO_WRITE;
  }

  int ret = CLI_BUILDER_INCOSISTENT_STATE;

  if(cli->cStatus->IsConsistent())
  {
    string s;
    s = filenames;
    CExportFiles* cef = CExportFiles::GetInstance(s,cli->mapCtx);
    cef->InitFiles();
    cef->WriteDepth(cli->getDepth());
    cef->WriteHndlDefs();
    cef->WriteCtxDefs();
    cef->WriteCmdDefs();
    cef->WritePrmDefs();
    cef->WriteRules();
    cef->WriteCallbackHeader();
    cef->WriteValuesCallbackHeader();
    cef->WriteConfig(cli->getConfigPointer());
    cef->WriteHandleEntryPoint();
    cef->EndFiles();
    delete cef;
    cef = NULL;

    ret = WRT_FILE_NO_ERROR;
  }

  if(cli)
  {
    delete cli;
    cli = NULL;
  }

  return ret;
}
