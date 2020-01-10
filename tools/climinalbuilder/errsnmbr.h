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

#ifndef ERRNMBRS_H
#define ERRNMBRS_H

#ifdef WIN32
#include "stdafx.h"
#endif

#define CLI_BUILDER_NO_ERROR              0
#define CLI_BUILDER_INCOSISTENT_STATE     -100

//create_param errors (-10000)
#define CR_PAR                            -10000  //starting point for CREATE PARAMETER ERRORS
#define CR_PAR_NO_ERROR                   CLI_BUILDER_NO_ERROR
#define CR_PAR_SINGLETON_NOT_INSTANCED    -10001  //Singleton never instanced. Does Main Context exist?
#define CR_PAR_CURR_PAR_NOT_NULL          -10002  //CliminalBuilder is processing another command. Previous end_param() Misses.
#define CR_PAR_PARENT_CMD_NOT_FOUND       -10003  //CliminalBuilder isn't processing any command. Unable to associate this parameter to a command
#define CR_PAR_DUPLICATED_PAR_IN_CMD      -10004  //Father command has another parameter with the same name
#define CR_PAR_UNABLE_TO_FIND_RELATED_CMD -10005  //Unable to associate this parameter to a SUBcommand
#define CR_PAR_DEFAULT_PAR_ALREADY_EXISTS -10006  //Father command already has a default parameter
#define CR_PAR_MAXVAL_SMALLER_THAN_MINVAL -10007  //(maxval < minval)
#define CR_PAR_HAS_SAME_NAME_OF_A_SUBCMD  -10008  //incoming parameter has the same name of a subcommand!


//end_param errors (-15000)
#define END_PAR                           -15000
#define END_PAR_NO_ERROR                  CLI_BUILDER_NO_ERROR
#define END_PAR_SINGLETON_NOT_INSTANCED   -15001
#define END_PAR_CURR_PAR_NOT_FOUND        -15002

//create_command errors (-20000)
#define CR_CMD                            -20000
#define CR_CMD_NO_ERROR                   CLI_BUILDER_NO_ERROR
#define CR_CMD_SINGLETON_NOT_INSTANCED    -20001
#define CR_CMD_CURR_PAR_NOT_NULL          -20002
#define CR_CMD_CURR_CMD_NOT_NULL          -20003
#define CR_CMD_CURR_CTX_NOT_FOUND         -20004
#define CR_CMD_DUPLICATED_CMD_IN_CTX      -20005
#define CR_CMD_DUPLICATED_SUBCMD_IN_CMD   -20006
#define CR_CMD_UNABLE_TO_FIND_RELATED_CMD -20007
#define CR_CMD_HAS_SAME_NAME_OF_A_PARAM   -20008  //incoming subcommand has the same name of a parameter!

//end_command errors (-25000)
#define END_CMD                           -25000
#define END_CMD_NO_ERROR                  CLI_BUILDER_NO_ERROR
#define END_CMD_SINGLETON_NOT_INSTANCED   -25001
#define END_CMD_CURR_PAR_NOT_NULL         -25002
#define END_CMD_CURR_CMD_NOT_FOUND        -25003

//create_context errors (-30000)
#define CR_CTX                            -30000
#define CR_CTX_NO_ERROR                   CLI_BUILDER_NO_ERROR
#define CR_CTX_SINGLETON_NOT_INSTANCED    -30001
#define CR_CTX_CURR_PAR_NOT_NULL          -30002
#define CR_CTX_PARENT_CMD_NOT_FOUND       -30003
#define CR_CTX_UNABLE_TO_FIND_RELATED_CMD -30004
#define CR_CTX_FATHER_CMD_ALR_HAS_SUBCTX  -30005
#define CR_CTX_DUPLICATED_CTX             -30006
#define CR_CTX_MAIN_CTX_NOT_FOUND         -30007

//end_context errors (-35000)
#define END_CTX                           -35000
#define END_CTX_NO_ERROR                  CLI_BUILDER_NO_ERROR
#define END_CTX_SINGLETON_NOT_INSTANCED   -35001
#define END_CTX_CURR_PRM_NOT_NULL         -35002
#define END_CTX_CURR_CMD_NOT_NULL         -35003
#define END_CTX_CURR_CTX_NOT_FOUND        -35004
#define END_CTX_FATHER_CTX_NOT_FOUND      -35005

//write_files errors (-40000)
#define WRT_FILE                          -40000
#define WRT_FILE_NO_ERROR                 CLI_BUILDER_NO_ERROR
#define WRT_FILE_NOTHING_TO_WRITE         -40001

#endif