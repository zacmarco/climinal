/*
   Climinal
   Copyright (C) 2020, Marco Zaccheria

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

#include <stdlib.h>
#include <string.h>

#include "climinal/terminal.h"
#include "climinal/cli.h"
#include "climinal/session.h"


static Cmdinfo *createcmdinfo  ( const Clisession *session, const Clicmd *cmd, char *next_data );
static void    destroycmdinfo  ( Cmdinfo *cmdinfo );
static void    dump_history    ( const Clisession *session );
static int     word_distance   ( const char *start, const char *current, const char*endbuf );

/* This function returns the address of next word in a buffer, giving also as output the strlen of the word */
static char *next_word( const char *in, const char *in_end, unsigned int *len )
{
    char *word_end      = NULL;
    char *word_start    = NULL;

    if (len)
        *len=0;

    /* Let's remove any trailing blankspace */
    while( (in < in_end) && (*in == ' ') )
    {
        ++in;
    }

    if( in < in_end )
    {

        /* Let's search for the next blankspace of end of buffer */
        word_end = strchr( in, ' ' );
        if( !word_end )
            word_end = strchr( in, '\0' );

        if( word_end > in )
        {
            /* We have a word! */
            if( len )
                *len = word_end - in;
            word_start = (char*)in;
        }
    }

    /* Let's return what we have found (NULL if none) */
    /* printf("WS: %s\n", word_start); */
    return word_start;
}

static char *last_word( const char *in, const char *in_end, unsigned int *len )
{
    char *word_end      = NULL;
    char *word_start    = NULL;

    word_end=(char*)in_end;

    /* Let's find the last space char, as we need the last complete word */
    while( (in < word_end) && (*(word_end-1) != ' ') ) {
        --word_end;
    }

    /* Let's remove any trailing blankspace */
    while( (in < word_end) && (*(word_end-1) == ' ') )
    {
        --word_end;
    }

    word_start=word_end;
    while( (in < word_start) && (*(word_start-1) != ' ') )
    {
        --word_start;
    }

    if(len)
        *len=word_end-word_start;

    /* Let's return what we have found (NULL if none) */
    /* printf("WS: %s\n", word_start); */
    return word_start;
    
}

static inline int execute( const Clisession *session, const Clicmd *cmd, const char *line, char *next_data )  
{
    int         retval      = CLIMINAL_E_INVALID;
    Cliparam    *param      = cmd->param;
    Cmdinfo     *cmdinfo    = NULL;
    Cmdparam    *defparam;
    int         ok = 1;
    int         defparam_req = 0;

    /* First of all, we have to check whether the callback is registered */
    if( cmd->cbk && line )
    {
        /* Let's create the Cmdinfo structure */
        cmdinfo = createcmdinfo( session, cmd, next_data );

        if( cmdinfo )
        {

            /* Let's just check whether all the required parameters are there */
            while( ok && param && param->name )
            {
                if( strlen(param->name) )
                {
                    if( param->required && strlen(param->name) && (!findparam(cmdinfo, param->name)) )
                    {
                        ok = 0;
                        fprintf(session->term.out, ">> MISSING PARAMETER: \"%s\"\n", param->name);
                    }
                }
                else if( (param->required) && !(cmdinfo->defval) )
                {
                    ok = 0;
                    fprintf(session->term.out, ">> MISSING DEFAULT PARAMETER\n");
                }
                if( ok )
                {
                    ++param;
                }
            }


            if( ok ) {
                /* Set back terminal to standard mode */
                res_terminal( (Cliterm*) &session->term );
                
                /* All OK... let's execute the callback! */
                retval = cmd->cbk(session->term.in, session->term.out, cmdinfo, line, session->cookie);

                /* Set back terminal to interactive mode */
                set_terminal( (Cliterm*) &session->term );
            }
            /* Finally destroy the structure sent to the callback */
            destroycmdinfo( cmdinfo ); 
        }
    }
    else
    {
        fprintf(session->term.out, ">> NO REGISTERED ACTION\n");
    }

    return retval;
}

static Clicmd *find_active_cmd( Clisession *session, char **nextdata )
{
    unsigned    int count = 0, cmdfound = 0, cmdlen = 0;
    char        *name, *cmd;
    char        *buf     = session->term.buffer;
    const char  *end_in_buf  = buf+MAX_BUFLEN;
    Clicmd      *ret_val = NULL;
    Clicmd      *cmdlist = session->active_context->cmd;

    do
    {
        /* If we've already found a command, let's try with its subcommands */
        if( ret_val != NULL )
            cmdlist = ret_val->subcmd;

        /* Let's search for the next word to find */
        cmd = next_word( buf, end_in_buf, &cmdlen );

        /* In case there's no a new word, or in case our cursor is not positioned after the found word (+1 blank), let's exit */
        if( !(cmd && cmdlist) )
            goto exit;


        count = cmdfound = 0;

        /* Let's search for the desired entry in the selected command list */
        do
        {
            name = cmdlist[count].name;
            if(name)
                /* If they are the same string, we've found the right command */
                if( (name) && (strlen(name)==cmdlen) && (!strncmp(name, cmd, cmdlen)) )
                {
                    ret_val = &(cmdlist[count]);
                    cmdfound = 1;

                    /* If this API was called with the "nextdata", we have to return also the pointer to the next data,
                       tipically the pointer where to start searching for the parameters list. */
                    if( nextdata )
                        *nextdata   = cmd+cmdlen;
                }
            count++;
        }
        while( (name != NULL) && (!cmdfound) );

        /* Let's move to the end of the word, in order to search for the next word */
        buf += (cmdlen+1);
    }
    while( cmdfound && ( (cmd+cmdlen) != (buf+session->term.pos) ));

exit:
    return ret_val;
}

static Cliparam *find_active_param( Clisession *session )
{
    char *next_data;
    Cliparam *param, *active_param=NULL;
    char word[MAX_BUFLEN];
    unsigned int len=0;
    char *last;

    if( !session->active_cmd ) {
        session->active_cmd=find_active_cmd(session, NULL);
    }

    last=last_word(session->term.buffer, session->term.buffer+session->term.pos, &len);
    if(last){
        strncpy(word, last, len);
    }
    word[len]='\0';

    param=session->active_cmd->param;

    while(param && param->name && (!active_param)){
        if(!strcmp(param->name, word)) {
            active_param=param;
        }
        param++;
    }

    return active_param;
}

static char *get_main_cmd_offset( Clisession *session )
{
    char *c_ptr = session->term.buffer;
    while( *c_ptr == ' ' )
    {
        c_ptr++;
    }

    return c_ptr;
}

static void get_main_cmd( Clisession *session, char *out_buf )
{
    int num=0;
    char *c_ptr = get_main_cmd_offset(session);

    while ( (*c_ptr != ' ') && (*c_ptr != '\0') && (*c_ptr != '\n') )
    {
        out_buf[num++]=*c_ptr++;
    }
    out_buf[num]='\0';
}

static char *command_generator ( Clisession *session, const char *text, int textlen, int state)
{
    static int      list_index, exit_matched, end_matched, bye_matched, history_matched, param_num, subcmd_num;
    static Clicmd   *cmdlist;
    int             count;
    char            *name=NULL, *retval=NULL, *nextname=NULL;
    char            buf[MAX_STRLEN+2];
    const char      *end_in_buf  = session->term.buffer+MAX_BUFLEN;

    count = 0;

    /*
       If this is a new word to complete, initialize now.  This includes
       saving the length of TEXT for efficiency, and initializing the index
       variable to 0.
     */
    if (!state)
    {
        /* This means we are starting a new list... so let's initialize all variables */
        list_index  = 0;
        param_num   = subcmd_num = 0;
        cmdlist     = session->active_context->cmd;
        bye_matched = end_matched = exit_matched = history_matched = 0;

        char *next_data;

        /* Let's understand whether we have a reasonable word to complete */
        session->active_cmd = find_active_cmd(session, &next_data);


        if( (session->active_cmd) )
        {
            if( *next_data==' ' ) { 
                /* The list of commands to be completed is active_cmd->subcmd */
                cmdlist = session->active_cmd->subcmd;

                /* Let's count, only the first time, the number of parameters */
                if( session->active_cmd->param )
                {
                    while( session->active_cmd->param[count++].name )
                    {
                        param_num++;
                    }
                }
            }
        }
        else
        {
            /* 
               If no active commands and there's a word before the one we are completing,
               let's exit without completing anything
             */
            nextname = next_word(session->term.buffer, end_in_buf, NULL);
            if( !nextname )
                goto exit;

            name = strchr( nextname, ' ' );
            if( name && ( name <= text ) )
                goto exit;

            /* Ok... let's keep on completing */
            if( !bye_matched )
            {
                name="bye";
                if(!strncmp(name, text, strlen(text))) {
                    bye_matched = 1;
                    retval = malloc(strlen(name)+1);
                    strcpy(retval, name);
                    goto exit;
                }
            }
            if( !history_matched )
            {
                name="history";
                if(!strncmp(name, text, strlen(text))) {
                    history_matched = 1;
                    retval = malloc(strlen(name)+1);
                    strcpy(retval, name);
                    goto exit;
                }
            }
            if( session->main_context->depth>1 ) {
                if( !exit_matched  )
                {
                    name="exit";
                    if(!strncmp(name, text, strlen(text))) {
                        exit_matched = 1;
                        retval = malloc(strlen(name)+1);
                        strcpy(retval, name);
                        goto exit;
                    }
                }
                if( !end_matched  )
                {
                    name="end";
                    if(!strncmp(name, text, strlen(text))) {
                        end_matched = 1;
                        retval = malloc(strlen(name)+1);
                        strcpy(retval, name);
                        goto exit;
                    }
                }
            }

            /* Because there's no active command, let's set param_num to 0 to avoid useless param completion */
            param_num = 0;
        }

        /* Now, let's count the number of commands */
        if( cmdlist )
        {
            count = 0;
            while( cmdlist[count++].name )
            {
                subcmd_num++;
            }
        }
    }

    /* Ready to check parameters */
    while( (list_index < (param_num+subcmd_num)) && !retval )
    {
        if( list_index < param_num )
            name = session->active_cmd->param[list_index].name;
        else if( list_index < (param_num+subcmd_num) )
            name = cmdlist[(list_index)-param_num].name;
        else {
            name=NULL;
            goto exit;
        }

        list_index++;

        if ( strncmp (name, text, textlen) == 0 )
        {
            /*  
                Checking if the whole word is already present in the command
                Note that words containing the searched one don't match
             */
            sprintf(buf,"%s%c", name, ' ');
            if( ! strstr(session->term.buffer, buf) )
            {
                retval = malloc(strlen(name)+1);
                strcpy(retval, name);
                goto exit;
            }
        }
    }

exit:
    /* If no names matched, then return NULL. */
    return retval;
}

static char **completion_matches( Clisession *session, const char *text, unsigned int len )
{
    char *tempBuf;
    char **retval=NULL;
    int state=0, matches=0;
    unsigned int tempSize=0;


    /* Let's first try to understand whether there is a param value to complete */
    if( (session->active_cmd) && (session->active_cmd->param) )
    {
        /* Let's complete param values if needed */
        char *last, *to_complete, word[MAX_BUFLEN], word_to_complete[MAX_BUFLEN];
        /* Potential matches */
        int p_matches=0, count;
        char *p_val[MAX_NUM_COMPL_ENTRIES];
        unsigned int len=0, i;
        last=last_word(session->term.buffer, session->term.buffer+session->term.pos, &len);
        strncpy(word, last, len);
        word[len]='\0';

        /* Let's find the partial word to be completed */
        to_complete=next_word(last+len, session->term.buffer+session->term.pos, &len);
        if(len) {
            strncpy(word_to_complete, to_complete, len);
        }
        word_to_complete[len]='\0';

        /* Step 1: looking at regular params first */
        Cliparam *param=session->active_cmd->param;
        while( (param->name) && (!p_matches) ) {
            if( strlen(param->name)>0 ) {
                char *param_in_buf=strstr(session->term.buffer, param->name);
                if( param_in_buf &&/*(!strcmp(param->name, word)) &&*/ 
                    (param->val) &&
                    (word_distance(param_in_buf, &(session->term.buffer[session->term.pos]), &(session->term.buffer[MAX_BUFLEN])) <= param->numval) ) {

                    /* Let's call custom callback */
                    p_matches=param->val(p_val, session->cookie);
                }
            } 
            param++;
        } 

        /* Step 2: if no regular param found, completing value for default param */
        if(!p_matches) {
            param=session->active_cmd->param;
            while( (param->name) && (!p_matches) ) {
                if( (strlen(param->name)==0) && (param->val) ) {
                    /* Let's call customer callback */
                    p_matches=param->val(p_val, session->cookie);
                } 
                param++;
            } 
        }

        /* If some values have been completed, print and free */
        if(p_matches) {
            for(count=0; count<p_matches; ++count) {
                if( (! (strncmp(to_complete, p_val[count], len) && to_complete) ) ) {
                    if( (!retval) && !(retval = malloc( MAX_NUM_COMPL_ENTRIES * sizeof(char*)) ) ) {
                        for(i=0; i<p_matches;++i) {
                            free(p_val[i]);
                        }
                        return NULL;
                    }
                    retval[matches++]=p_val[count];
                } else {
                    free(p_val[count]);
                }
            }
        }
    }

    /* Let's complete commands only in case of no parameters to complete */
    if(!retval) {
        do
        {
            tempBuf = command_generator( session, (const char*)text, len, state );
            if( tempBuf )
            {
                if( !retval )
                {
                    retval = malloc( MAX_NUM_COMPL_ENTRIES * sizeof(char*) );
                    if( !retval )
                        return NULL;
                    state = 1;
                }
                retval[matches++] = tempBuf;
            }
        }
        while( tempBuf );
    }

    /* The matching array MUST be NULL terminated */
    if( matches )
        retval[matches] = NULL;

    return retval;
}

static char **completion (Clisession *session, const char *text, int start, int end)
{
    char **matches = NULL;

    /* Let's understand whether we have a reasonable word to complete */
    session->active_cmd = find_active_cmd(session, NULL);

/*
    char *last;
    unsigned int len;
    last=last_word(session->term.buffer, session->term.buffer+session->term.len, &len);
    strncpy(word, last, len);
    word[len]='\0';
    printf("last word: \"%s\"\n", word); 
*/
    /* Ok... let's complete it! */
    matches = completion_matches (session, text, end-start);

    return (matches);
}

static int parsecommon( Clisession *session, const char *main_cmd )
{
    int retval  = CLIMINAL_NO_ERROR;

    if( !strcmp(main_cmd, "bye") ) {
        retval = CLIMINAL_E_EXIT;
    }
    else if( (session->main_context->depth>1) && (!strcmp(main_cmd, "exit")) ) {
        if( session->active_context->father ) {
            session->active_context=session->active_context->father;
        }
    }
    else if( (session->main_context->depth>1) && (!strcmp(main_cmd, "end")) ) {
        while( session->active_context->father != NULL ) {
            session->active_context = session->active_context->father;
        }
    }
    else if( !strcmp(main_cmd, "history") ) {
        dump_history(session);
    }
    else {
        retval = CLIMINAL_E_NOT_FOUND;
    }
    return retval;
}

/* Calculates the distance in terms of number of words between the two pointers */
/* start: pointer to the left word in buffer */
/* current: pointer to the current (right) word in buffer */
static int word_distance( const char *start, const char *current, const char *endbuf ) 
{
    int negative;
    char *left, *right, *curr, *next;
    int count=0;
    unsigned int len;

    if( start<current ) {
        left=(char*)start;
        right=(char*)current;
        negative=0;
    } else {
        left=(char*)current;
        right=(char*)start;
        negative=1;
    }

    next=left;
    next_word(next, endbuf, &len);

    do{
        curr=next;
        next=next_word(curr+len, endbuf, &len);
        //printf("\nCURR:%X NEXT:%X RIGHT:%X ENDBUF:%X\n", curr, next, right, endbuf);
        if((next+len)<right) {
            count++;
        }
        //printf("left: %16X, right: %16X, curr: %16X\n", left, right, curr);
        //printf("STRING: %s\n", left);
    } while( (next) && (next<right) && (curr<next) );

    if(negative) {
        count=-count;
    }

    //printf("COUNT: %d\n", count);
    return count;

}

static int parseline( Clisession *session )
{
    int      retval  = CLIMINAL_NO_ERROR;
    Clicmd          *cmd    = NULL;
    char            *buf    = session->term.buffer;
    char            main_cmd[MAX_STRLEN] = "";
    char            *next_data;
    int             bufempty;

    /* Get the new line */
    if(readline( session, session->active_context->prompt )) {
        retval=CLIMINAL_E_EXIT;
        goto exit;
    }
    bufempty=(strlen(buf))?0:1;

    if( !bufempty  )
    {
        /* If a command, do it!!! */
        get_main_cmd( session, main_cmd );
        if( strlen(main_cmd) > 0 )
        {
            /* Let's check if we are trying to run a command in history */
            if( strlen(main_cmd)>1 && main_cmd[0] == '!' ) {
                unsigned int num, entry_id;
                Clihistory *hist;
                if( strlen(main_cmd) > 1 ) {
                    /* Let's check if there are other word in the buffer */
                    char *word_in_buffer=strstr(session->term.buffer, main_cmd);
                    if(next_word(&(word_in_buffer[strlen(main_cmd)]), session->term.buffer+MAX_BUFLEN, NULL)) {
                        fprintf(session->term.out, ">>SYNTAX ERROR\n");
                        return CLIMINAL_NO_ERROR;
                    }

                    hist=&(session->term.history);
                    num=strtol( &(main_cmd[1]), NULL, 10 );
                    if( num<=0 || (num >= (hist->cmdnum-1)) || (hist->cmdnum>HISTORY_SIZE && num<(hist->cmdnum-HISTORY_SIZE)) ) {
                        fprintf(session->term.out, ">>COMMAND NUMBER %u NOT AVAILABLE\n", num);
                        return CLIMINAL_NO_ERROR;
                    }
                    entry_id=num%(HISTORY_ARRAY_SIZE);

                    memset( session->term.buffer, '\0', sizeof(session->term.buffer) );
                    strcpy(session->term.buffer, hist->entry[entry_id]);
                    session->term.len=strlen(hist->entry[entry_id]);

                    /* Let's re-evaluate main_cmd, as we have changed the content of term.buffer */
                    get_main_cmd( session, main_cmd );

                    /* Printing the whole command to be repeated */
                    fprintf(session->term.out, " %s\n", hist->entry[entry_id]);
                }
            }

            if( (retval = parsecommon(session, main_cmd)) == CLIMINAL_E_NOT_FOUND )
            {
                if( ( cmd=find_active_cmd(session, &next_data) ) )
                {
                    /* Execute the callback all the cases */
                    retval = execute(session, cmd, buf, next_data) ;

                    /* If it's only a context change, and no errors in callback, do it!!! */
                    if( cmd->subcontext && CLIMINAL_SUCCESS(retval) )
                    {
                        ((Clicontext*)(cmd->subcontext))->father=session->active_context;
                        session->active_context = ((Clicontext*)(cmd->subcontext));
                    }
                    
                }
                else
                {
                    fprintf(session->term.out, ">> COMMAND NOT FOUND\n");
                }
            }
        }
        add_history( &(session->term.history), buf );
        session->term.history.entry[session->term.history.newid][0]='\0';
    }
exit:    
    return retval;
}

int climinal_main(const FILE *in, FILE *out, Clicontext *maincontext, void *cookie)
{
    int exit = CLIMINAL_NO_ERROR;
    Clisession *session=malloc(sizeof(Clisession));

    if(!session)
        return 1;

    initsession( session, maincontext, in, out, cookie );
    setcompleter( session, completion );

    /* Adding an extra empty line to be used as a separator */
    add_history( &(session->term.history), "" ); 

    do
    {
        exit = parseline(session);
    }
    while( exit != CLIMINAL_E_EXIT );

    res_terminal( &(session->term) );

    free(session);

    return 0;
}

static void printusage( Clisession *session, Clicmd *cmd )
{
    int count = 0, i, maxparamlen, len, isdefval, numofblanks;
    const char defstring[]="(default)";
    Cliparam *param=NULL;
    Clicmd   *subcmd=NULL;

    /* Parameters */
    if( cmd->param )
    {
        fprintf( session->term.out, "\nPARAMETERS:\n");

        /* Let's do a first loop to calculate the longest parameter name */
        maxparamlen=strlen(defstring);
        param = &(cmd->param[0]);
        while( param->name )
        {
            len=strlen( param->name );
            if(len>maxparamlen) 
            {
                maxparamlen=len;
            }
            param = &(cmd->param[++count]);
        }

        count=0;
        param = &(cmd->param[count]);
        while( param->name )
        {
            isdefval = (strlen(param->name)==0);
            fprintf( session->term.out, " - %s", (isdefval)? defstring:param->name );

            if( isdefval ) 
            {
                if( maxparamlen>strlen(defstring) ) 
                {
                    numofblanks = maxparamlen - strlen(defstring);
                }
                else
                {
                    numofblanks = 1;
                }
            }
            else
            {
                numofblanks = maxparamlen-strlen(param->name);
            }
            for( i=0; i<numofblanks; ++i )
            {
                fprintf( session->term.out, " ");
            }

            if( isdefval )
            {
                fprintf( session->term.out, "%s\n", (param->required)?"< required >":"");
            }
            else
            {
                fprintf( session->term.out, " < %svalues=%u >\n", (param->required) ? "required, ":"", param->numval );
            }
            
            param = &(cmd->param[++count]);
        }
    }

    /* Subcommands */
    if( cmd->subcmd )
    {
        count = 0;
        fprintf( session->term.out, "\nSUBCOMMANDS:\n");

        subcmd = &(cmd->subcmd[count]);
        while( subcmd->name )
        {
            fprintf( session->term.out, " - %s \n", subcmd->name );
            subcmd = &(cmd->subcmd[++count]);
        }
    }
}

static inline void printcommoncmds( Clisession *session ) 
{
    if( session->main_context->depth > 1 ) {
        fprintf( session->term.out, "COMMON COMMANDS:\n" );
    }
    fprintf( session->term.out, "%s - %s\n", "bye    ", "exit from the application" );
    if( session->main_context->depth > 1 ) {
        fprintf( session->term.out, "%s - %s\n", "end    ", "back to main context" );
        fprintf( session->term.out, "%s - %s\n", "exit   ", "back to previous context" );
    }
    fprintf( session->term.out, "%s - %s\n", "history", "show command history" );
}

static inline void printallcmd( Clisession *session )
{
    Clicmd *cmd = session->active_context->cmd;
    while( cmd && cmd->name )
    {
        fprintf( session->term.out, "%s - %s\n", cmd->name, cmd->brief );
        ++cmd;
    }
    printcommoncmds(session);
}

void printhelp( Clisession *session )
{
    Clicmd *cmd;
    Cliparam *param;

    cmd = find_active_cmd(session, NULL);
    if( cmd && cmd->help && cmd->brief ) {
        param=find_active_param(session);
        /* In case we have a parameter, let's show the parameter description, otherwise let's show the full command help */
        if(param && (strlen(param->description)>0)) {
            fprintf(session->term.out, "PARAMETER: %s\n  %s\n", param->name, param->description);
        } else {
            /* Command name */
            fprintf( session->term.out, "\nCOMMAND:\n%s - %s\n", cmd->name, cmd->brief );

            /* First of all. let's print the command usage */
            printusage( session, cmd );

            /* At the end, let's print the command help string */
            fprintf(session->term.out, "\nDESCRIPTION:\n%s\n\n", cmd->help);
        }
    }
    else
    {
        printallcmd( session );
    }
    fprintf( session->term.out, "%s> %s", session->active_context->prompt, session->term.buffer );
    CURS_L( session->term.out, (session->term.len)-(session->term.pos));
}

/*
   Implementation for helper functions to be given to the
   callbacks writer
 */

static Cliparam *findparam_cmd( const Clicmd *cmd, const char *paramname, int paramlen)
{
    Cliparam *param = cmd->param;

    while( param && param->name )
    {
        if( !strncmp( param->name, paramname, paramlen ) )
        {
            return param;
        }
        param++;
    }
    return NULL;
}

static Cmdinfo *createcmdinfo( const Clisession *session, const Clicmd *cmd, char *next_data )
{
    /* "current" is the pointer representing the current iterator in the input buffer */
    Cliparam        *param;
    unsigned int    curlen=0, parnum, valnum, count, param_found, defval_found=0;
    Cmdinfo         *cmdinfo;
    const char      *end_in_buf     = session->term.buffer+MAX_BUFLEN;
    char            *current        = next_data;

    cmdinfo = malloc( sizeof(Cmdinfo) );
    if( ! cmdinfo )
        goto exit;

    /* All pointers to NULL and fields to ZERO */
    memset( cmdinfo, 0x0, sizeof(Cmdinfo) );

    /* With this command, we both take the active command and the start of the buffer for parameters lookup */

    do
    {
        current = next_word( current+curlen, end_in_buf, &curlen );

        if( curlen )    
        {
            /* As a first step, let's search for a parameter with the "current" name */
            parnum=0;
            param_found=cmdinfo->paramnum;

            do
            {
                param = &(cmd->param[parnum]);
                if( param && param->name && strlen(param->name) )
                {
                    if( (curlen==strlen(param->name)) && !strncmp(current, param->name, strlen(param->name)) )
                    {
                        valnum = count = 0;

                        /* It's good, we've found a parameter. So let's add it to the output structure (remember to free!!!) */
                        cmdinfo->param[cmdinfo->paramnum] = malloc( sizeof(Cmdparam) );
                        memset( cmdinfo->param[cmdinfo->paramnum], 0x0, sizeof(Cmdparam) );

                        cmdinfo->param[cmdinfo->paramnum]->name = malloc(strlen(param->name)+1);
                        strcpy(cmdinfo->param[cmdinfo->paramnum]->name, param->name);

                        /* Let's add values if needed */
                        while( (count < param->numval) && (count++ == valnum) )
                        {
                            /* The next word is the value of the given parameter */
                            current = next_word( current+curlen, end_in_buf, &curlen );

                            if( current && strlen(current) )
                            {
                                /* if the value is a subcommand, let's exit immediately */
                                if( ! findparam_cmd( (const Clicmd*)cmd, current, curlen ) )
                                {
                                    cmdinfo->param[cmdinfo->paramnum]->value[valnum]=strndup(current, curlen);
                                    if( !cmdinfo->param[cmdinfo->paramnum]->value[valnum] )
                                    {
                                        destroycmdinfo(cmdinfo);
                                        goto exit;
                                    }
                                    ++valnum ;
                                }
                            }

                        }
                        cmdinfo->param[cmdinfo->paramnum]->numval = valnum;

                        if( (param->name[0] != '\0') && (valnum < param->numval) )
                        {
                            fprintf( session->term.out, ">> MISSING VALUE FOR PARAMETER \"%s\"\n", param->name );
                            destroycmdinfo( cmdinfo );
                            cmdinfo = NULL;
                            goto exit;
                        }
                        ++(cmdinfo->paramnum);
                    }
                }
                else
                {
                    defval_found=1;
                }
                ++parnum;
            }
            while( param && param->name && ( strncmp(param->name, current, curlen)!=0 ) );

            if( (!cmdinfo->defval) && (defval_found) && (param_found == cmdinfo->paramnum) )
            {
                /* If we are here, we've found the default value */
                cmdinfo->defval = malloc(curlen+1);
                strncpy(cmdinfo->defval, current, curlen);
                cmdinfo->defval[curlen]='\0';

            }
        }
    }
    while( current && curlen );

exit:
    return cmdinfo;
}

void destroycmdinfo( Cmdinfo *cmdinfo )
{
    unsigned int paramcount, valcount;

    if( cmdinfo->defval )
    {
        free( cmdinfo->defval );
    }

    for( paramcount=0; paramcount<cmdinfo->paramnum; ++paramcount )
    {
        valcount=0;
        if( cmdinfo->param[paramcount]->name )
            free( cmdinfo->param[paramcount]->name );
        while( cmdinfo->param[paramcount]->value[valcount] )
        {
            free( cmdinfo->param[paramcount]->value[valcount] );
            valcount++;
        }
        free( cmdinfo->param[paramcount] );
    }

    free( cmdinfo );
}

void dump_history( const Clisession *session )
{
    unsigned int index, valid, cmdnum;
    const Clihistory *hist=&(session->term.history);

    index = hist->newid;

    cmdnum = (hist->cmdnum<HISTORY_ARRAY_SIZE) ? 1 : hist->cmdnum - HISTORY_SIZE;

    do {
        valid = strlen(hist->entry[index]);
        if( valid ) {
            fprintf(session->term.out, "%d\t%s\n", cmdnum, hist->entry[index]);
            cmdnum++;
            }
        index=(index<HISTORY_ARRAY_SIZE-1) ? index+1 : 0;
    }
    while( (index != hist->newid) );
}

Cmdparam *findparam ( const Cmdinfo *info, const char *name )
{
    unsigned int count;

    for( count=0; count<info->paramnum; ++count )
    {
        if( !strcmp( name, info->param[count]->name ) )
            return info->param[count];
    }
    return NULL;
}

Cmdparam *finddefparam ( const Cmdinfo *info )
{
    unsigned int count;

    for( count=0; count<info->paramnum; ++count )
    {
        if( !strlen( info->param[count]->name ) )
            return info->param[count];
    }
    return NULL;
}


