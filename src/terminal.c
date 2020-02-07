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

#include <string.h>
#include <stdlib.h>
#include <math.h>

/* Check whether we need to include the ioctl.h in order to take the TIOCGWINSZ macro */
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#define GWINSZ_IN_SYS_IOCTL  (1)
#endif

#include "climinal/session.h"
#include "climinal/cli.h"

/* Flags used to get history entries */
#define HISTORY_AFTER   (0)
#define HISTORY_BEFORE  (1)

#if 0
#define CURS_L(outFile, num)      { int i; for(i=0; i<num; ++i) { fprintf(outFile, "%c%c%c", 0x1b, 0x5b, 0x44); } }
#define CURS_R(outFile, num)      { int i; for(i=0; i<num; ++i) { fprintf(outFile, "%c%c%c", 0x1b, 0x5b, 0x43); } } 
#endif
#define CLR_LIN_R(outFile)        { fprintf(outFile, "%c%c%c", 0x1b, 0x5B, 0x4B); }
#define CLR_ALL_D(outFile)        { fprintf(outFile, "%c%c%c", 0x1B, 0x5B, 0x4A); }

#define SET_AUTO_WRAP(outFile)    { fprintf(outFile, "%c%c%c%c%c", 0x1B, 0x5B, 0x3F, 0x37, 0x68); }


typedef enum
{
    C_ALPHA,
    C_QUOTE,
    C_NUM,
    C_BSP,
    C_DEL,
    C_TAB,
    C_CLEAR_LINE,
    C_NEWLINE,
    C_SPACE,
    C_ARROW_END,
    C_ARROW_HOME,
    C_ARROW_UP,
    C_ARROW_DOWN,
    C_ARROW_RIGHT,
    C_ARROW_LEFT,
    C_QUEST,
    C_NOPRINT,
    C_EOF
}
Chartype;

/* Gets the length of the common substring from the beginning of the strings */
static int longSubstr(const char *s, const char *t)
{
    int longest=0;
    char *s1=(char*)s, *s2=(char*)t;
    while( (*s1 != '\0') && (*s1 == *s2) ) {
        ++s1;
        ++s2;
        ++longest;
    }

    return longest;
}

/* Function to sort a string array (to be used in auto-completion) */
static void sort_str_array(char **a, int size)
{
    int i, j;
    for (i=0; i < size; ++i)
    {
        char *v = a[i];
        for (j = i - 1; j >= 0; j--)
        {
            if (strcmp( a[j], v)<0)
                break;
            a[j + 1] = a[j];
        }
        a[j + 1] = v;
    }
    return;
}

/* Adds a char to the terminal. Also session buffer is incremented */
static inline void addchar( Cliterm *term, const char c )
{
    int count;
    char *buf=term->buffer;

    /* This for loop is to be considered in case of adding a char not at the 'len' position */
    for( count=0; count<=(term->len - term->pos); ++count )
    {
        buf[term->len-count+1]=buf[term->len-count];
    }
    buf[term->pos]=c;

    /* Print the buffer on screen */
    for( count=0; count<=(term->len - term->pos); ++count )
        fputc(buf[term->pos+count], term->out);
    CURS_L(term->out, term->len - term->pos);

    term->len++;
    term->pos++;

    return;
}

void add_history( Clihistory *hist, const char *in_buf )
{
    char *newbuf            = NULL;

    if( (hist->newid == 0) && (hist->lastid == 0) )
    {
        /* History is empty... let's add the first entry */
        ++(hist->newid);
    }
    else
    {

        hist->lastid = hist->newid;
        hist->newid = ( hist->newid < (HISTORY_ARRAY_SIZE-1) ) ? hist->newid + 1 : 0;
    }

    newbuf = hist->entry[hist->lastid];

    if( newbuf )
    {
        strcpy(newbuf, in_buf);
        hist->getid = hist->lastid;
    }

    hist->cmdnum++;
}

int get_history( Clihistory *hist, char *out_buf, int direction )
{
    unsigned int retid=hist->getid;

    if( hist->newid != hist->lastid )
    {
        switch(direction)
        {
            /* In both cases we need to understand if history has been already wrapped or not */
            case HISTORY_AFTER:
                if( hist->getid != hist->lastid ) {
                    if( hist->getid < (hist->cmdnum-1) )
                        hist->getid++;
                    else
                        hist->getid = 0;
                }
                break;

            case HISTORY_BEFORE:
                if( hist->getid != hist->newid ) {
                    if( hist->getid > 0 )
                        hist->getid--;
                    else
                        hist->getid = (hist->cmdnum-1);
                }
                break;
        }

        retid = (hist->getid + 1)%HISTORY_ARRAY_SIZE;
        strcpy(out_buf, hist->entry[retid]);

    }

    return retid;
}

void init_history( Clihistory *hist )
{
    int count;

    hist->newid=0;
    hist->getid=0;
    hist->lastid = 0;
    hist->cmdnum=0;
    
    for(count=0; count<HISTORY_ARRAY_SIZE; ++count) {
        strcpy(hist->entry[count], "");
    }
}

static inline void print_and_free_matches( Clisession *session, char **matches, int last_word_pos )
{
    int num_matches=0, num_common_char=0, count, currlen, columns=80;
    Cliterm *term = &(session->term);

    /* These variables are used for displaying the right number of entries for each line */
    int spacing = 0, act_strlen, spacecounter, entries_per_line=1;
    div_t division;

    /* Let's count the total number of matches (I apologize for the additional cycle) */
    while( matches[num_matches] != NULL )
    {
        num_matches++;
    }

    currlen = term->pos - last_word_pos;

    /* If just one match, let's complete it */
    if( num_matches == 1 )
    {
        for( count=currlen; count<strlen(matches[0]); count++ )
        {
            addchar( term, matches[0][count] );
        }
        addchar(term, ' ');
        free( matches[0] );
    }
    else
    {
#ifdef TIOCGWINSZ
        struct winsize wsize;

        /* Let's take the column width from the output file descriptor. */
        if (ioctl(fileno(term->out), TIOCGWINSZ, &wsize) == 0)
        {
            columns = wsize.ws_col;
        } 

        /* If not available, reset to 80 lines */
        if( columns <= 0 )
            columns = 80;
#endif

        /* Let's understand the max strlen of matches, and the max common substring as well */

        /* First common substring is the first string itself */
        char common_substr[strlen( matches[0] )+1];
        strcpy(common_substr, matches[0]);

        for( count=0; count<num_matches; ++count )
        {
            act_strlen = strlen( matches[count] );
            if ( spacing < act_strlen )
                spacing = act_strlen;

            /* Let's find now the longest substring */
            if( count == 0 ) 
            {
                num_common_char = strlen( matches[count] );
            }
            else
            {
                if( num_common_char ) {
                    num_common_char = longSubstr( common_substr, matches[count] );
                    common_substr[ num_common_char ] = '\0';
                }
            }
        }

        /* Let's add the common part, then let's print all the matches */
        for( count=currlen; count<num_common_char; count++ )
        {
            addchar( term, common_substr[count] );
        }


        /* We adjust spacing so that there will be a distance at least of two charcaters between words */
        spacing +=2 ;

        /* Now we can understand how many entries per line */
        if( (spacing >= 0) && (columns >= spacing) )
            entries_per_line = (int)trunc( (double)(columns/spacing) );

        /* Put a separating newline */
        fprintf( term->out, "\n" );

        /* Now, let's go to sort matches */
        sort_str_array( matches, num_matches );

        for( count=0; count<num_matches; ++count )
        {
            if( matches[count] )
            {
                /* We must understand if we are able to print the next string into the same line */
                if( (count > 0) &&  (count >= entries_per_line) )
                {
                    division = div( count, entries_per_line );

                    /* With this control we are sure about whether to wrap or not */
                    if( (!division.rem) && (division.quot) )
                        fprintf( term->out, "\n" );
                }

                fprintf( session->term.out, "%s", matches[count]);

                /* This is a general protection to avoid an unsigned integer to become negative */
                if( strlen(matches[count]) < spacing )
                {
                    for( spacecounter=0; spacecounter < ( spacing-strlen(matches[count]) ); ++spacecounter )
                        fputc( ' ', session->term.out  );
                }

                free(matches[count]);
            }
        }
        /* Rewrite prompt and current buffer, so to have the same output as before the TAB */
        fprintf( term->out, "\n%s> %s", session->active_context->prompt, term->buffer );

        /* As a final step, let's go back with the cursor at the original position */
        CURS_L( term->out, strlen(term->buffer) - term->pos );
    }

    /* After all, let's free the matches array */
    free( matches );

    return;
}

static inline int cli_complete( Clisession *session )
{
    char **matches;
    int last_word_pos;
    Cliterm *term = &(session->term);

    /* Searching for where to put the new characters (in case we are completing not at the end of the buffer) */
    last_word_pos=term->pos;
    if( last_word_pos > 0 )
    {
        do
        {
            last_word_pos--;
        }
        while( (term->buffer[last_word_pos] != ' ') && (last_word_pos > 0) );
    }

    if( term->buffer[last_word_pos] == ' '  )
        last_word_pos++;

    /* Let's find all words matching the autocompletion */
    matches = (char**) session->completer( session, &(term->buffer[last_word_pos]), last_word_pos, term->pos );

    /* In case of matching strings, let's do autocompletion! */
    if( matches )
        print_and_free_matches( session, matches, last_word_pos );

    return 0;
}


static Chartype get_c_type( const char c, const Cliterm *term )
{
    /* First of all: special characters */
    if( (c == 0x7F) || (c==0x10) || (c==0x08) )
        return C_BSP;
    if( (c == 0x20) )
        return C_SPACE;
    if( (c == 0x09) )
        return C_TAB;
    if( (c == 0x0A) || (c==0x0D) )
        return C_NEWLINE;
    if( (c == 0x15) )
        return C_CLEAR_LINE;
    if( (c == '"') )
        return C_QUOTE;
    if( ((c >= '0') && (c <= '9')) || ((c == ',') || (c == ':') || (c == ';')) )
        return C_NUM;
    if( (c == 0x3F) )
        return C_QUEST;

    /* Escape sequence */
    if( (c == 0x1B) )
    {
        switch( fgetc(term->in) )
        {
            case 0x5B:
                switch( fgetc(term->in) )
                {
                    case 0x33:
                        switch( fgetc(term->in) )
                        {
                            case 0x7E:
                                return C_DEL;
                                break;
                        }
                        break;
                    case 0x30:
                        switch( fgetc(term->in) )
                        {
                            case 0x48:
                                return C_ARROW_HOME;
                                break;
                        }
                        break;
                    case 0x48:
                        return C_ARROW_HOME;
                        break;
                    case 0x41:
                        return C_ARROW_UP;
                        break;
                    case 0x42:
                        return C_ARROW_DOWN;
                        break;
                    case 0x43:
                        return C_ARROW_RIGHT;
                        break;
                    case 0x44:
                        return C_ARROW_LEFT;
                        break;
                }
                break;
            case 0x4F:
                switch( fgetc(term->in) )
                {
                    case 0x46:
                        return C_ARROW_END;
                        break;
                    case 0x48:
                        return C_ARROW_HOME;
                        break;
                }
                break;
            case 0x7E:
                return C_DEL;
                break;
        }

    }
    if( (c == EOF) ) 
        return C_EOF;

    return C_ALPHA;
}

/* Function to set terminal parameters in sync mode */
int set_terminal( Cliterm *term )
{
    int retVal = CLIMINAL_NO_ERROR;
#ifdef HAVE_TERMIOS_H
    struct termios temp_term;

    SET_AUTO_WRAP( term->out );

    retVal = tcgetattr( fileno(term->in), &temp_term );
    if( retVal != 0 )
        goto exit;

    /* Now we store the old value for term info, in order to restore them at exit time */
    term->term_io = temp_term;

    temp_term.c_lflag |=  (ECHONL) ;
    temp_term.c_lflag &=~ (ICANON | ECHO | ECHOE);
    temp_term.c_cc[VTIME] = 10;

    retVal = tcsetattr( fileno(term->in), TCSANOW, &temp_term );
exit:
#endif
    return retVal;
}

int res_terminal( Cliterm *term )
{
    int retVal = CLIMINAL_NO_ERROR;
#ifdef HAVE_TERMIOS_H
    retVal = tcsetattr( fileno(term->in), TCSANOW, &term->term_io );
#endif
    return retVal;
}


int readline( Clisession *session, const char *prompt )
{
    int exit=0;
    unsigned int count;
    Cliterm *term = &(session->term);
    char c, *buf=term->buffer;

    /* Reset terminal info */
    memset( buf, '\0', sizeof(term->buffer) );
    term->len = 0;
    term->pos = 0;

    fprintf(term->out, "%s> ", prompt);

    do
    {
    	c=fgetc(term->in);
        switch( get_c_type(c, term) )
        {
            case C_BSP:
	    	//fprintf(term->out, "pos=%d, len=%d\n", term->pos, term->len);
                if( term->pos > 0 )
                {
                    CURS_L(term->out, 1);
                    /* Print the delete sequence */
                    for( count=term->pos; count<=term->len; ++count )
                    {
                        buf[count-1]=buf[count];
                        fputc(buf[count], term->out);
                    }
                    fputc(' ', term->out);
                    term->len--;
                    term->pos--;
                    CURS_L(term->out, count-term->pos-1);
                }
#if 0
                term->buffer_modified=1;
#endif
                break;

            case C_DEL:
                if( (term->pos >= 0) && (term->pos < term->len) )
                {
                    CLR_LIN_R(term->out);
                    /* Print the delete sequence */
                    for( count=term->pos; count<term->len; ++count )
                    {
                        buf[count]=buf[count+1];
                        fputc(buf[count], term->out);
                    }
                    term->len--;
                    CURS_L(term->out, count-term->pos-1);
                }
#if 0
                term->buffer_modified=1;
#endif
                break;

            case C_TAB:
                cli_complete( session );
                break;

            case C_CLEAR_LINE:
                CURS_L(term->out, term->pos);
                memset(term->buffer, 0x0, term->len);
                term->pos = term->len = 0;
                CLR_LIN_R(term->out);
#if 0
                term->buffer_modified=1;
#endif
                break;

            case C_ALPHA:
            case C_NUM:
            case C_SPACE:
            case C_QUOTE:
                addchar( term, c );
#if 0
                term->buffer_modified=1;
#endif
                break;

            case C_QUEST:
                fprintf(term->out, "\n");
                printhelp( session );
                break;

            case C_NEWLINE:
                fprintf(term->out, "\n");
                exit=1;
                break;

            case C_ARROW_RIGHT:
                if( term->pos < term->len )
                {
                    CURS_R(term->out, 1);
                    term->pos++;
                }
                break;

            case C_ARROW_LEFT:
                if( term->pos > 0 )
                {
                    CURS_L(term->out, 1);
                    term->pos--;
                }
                break;

            case C_ARROW_END:
                CURS_R( term->out, strlen(term->buffer) - term->pos );
                term->pos = strlen(term->buffer);
                break;

            case C_ARROW_HOME:
                CURS_L( term->out, term->pos );
                term->pos = 0;
                break;

            case C_ARROW_UP:
#if 0
                if( term->buffer_modified )
                {
                    strcpy(term->history.entry[term->history.newid], term->buffer);
                    term->buffer_modified=0;
                }
#endif
                get_history( &(term->history), term->buffer, HISTORY_BEFORE );
                CURS_L(term->out, term->pos);
                CLR_LIN_R(term->out);
                fprintf(term->out, "%s", term->buffer);
                term->pos = term->len = strlen(term->buffer);
                break;

            case C_ARROW_DOWN:
#if 0            
                if( term->buffer_modified )
                {
                    strcpy(term->history.entry[term->history.newid], term->buffer);
                    term->buffer_modified=0;
                }
#endif
                get_history( &(term->history), term->buffer, HISTORY_AFTER );
                CURS_L(term->out, term->pos);
                CLR_LIN_R(term->out);
                fprintf(term->out, "%s", term->buffer);
                term->pos = term->len = strlen(term->buffer);
                break;
            case C_EOF:
                return 1;
                break;
            default:
                break;
        }
    }
    while( !exit );

    return 0;
}


void setcompleter( Clisession *session, Clicompleter completer )
{
    session->completer =  completer;
}


void initsession( Clisession *session, Clicontext *context, const FILE *in, const FILE *out, void *cookie )
{
    session->term.in  = (FILE*) ( (in)  ? in    : stdin   );
    session->term.out = (FILE*) ( (out) ? out   : stdout  );
    session->cookie   = cookie;

    init_history( &(session->term.history) );
    set_terminal( &(session->term) );

    session->active_context = session->main_context = context;
}
