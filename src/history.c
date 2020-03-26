#include <stdlib.h>
#include <string.h>
#include "climinal/history.h"
#include "climinal/error.h"

void
add_history (Clihistory * hist, const char *in_buf)
{
    char *newbuf = NULL;

    if ((hist->newid == 0) && (hist->lastid == 0)) {
        /* History is empty... let's add the first entry */
        ++(hist->newid);
    } else {

        hist->lastid = hist->newid;
        hist->newid = (hist->newid < (hist->size)) ? hist->newid + 1 : 0;
    }

    newbuf = hist->entry[hist->lastid];

    if (newbuf) {
        strcpy (newbuf, in_buf);
        hist->getid = hist->lastid;
    }

    hist->cmdnum++;
}

int
get_history (Clihistory * hist, char *out_buf, int direction)
{
    unsigned int retid = hist->getid;

    if (hist->newid != hist->lastid) {
        switch (direction) {
            /* In both cases we need to understand if history has been already wrapped or not */
            case HISTORY_AFTER:
                if ( (hist->getid != hist->lastid) ) {
                    if ( hist->getid < (hist->size) ) {
                        hist->getid++;
                    } else {
                        hist->getid=0;
                    }
                }
                break;

            case HISTORY_BEFORE:
                if (hist->getid != hist->newid) {
                    if (hist->getid > 0)
                        hist->getid--;
                    else
                        hist->getid = (hist->size);
                }
                break;
        }

        retid = (hist->getid + 1) % (hist->size + 1);
        strcpy (out_buf, hist->entry[retid]);

    }

    return retid;
}

int
islast_history (Clihistory * hist, char *buf)
{
    int retid = 0;

    if (hist->newid != hist->lastid) {
        if(strcmp(buf, hist->entry[hist->lastid])==0) {
            retid=1;
        }
    }
    return retid;
}

int
init_history (Clihistory * hist, const unsigned int size)
{
    int count, retval = CLIMINAL_E_MEMORY;

    hist->newid = 0;
    hist->getid = 0;
    hist->lastid = 0;
    hist->cmdnum = 0;
    hist->size = size;

    hist->entry = malloc ((size + 1) * sizeof (HistoryBuffer));
    if (hist->entry) {
        for (count = 0; count < ((hist->size) + 1); ++count) {
            strcpy (hist->entry[count], "");
        }
        retval = CLIMINAL_NO_ERROR;
    }

    return retval;
}

void
free_history (Clihistory * hist)
{
    if (hist->entry) {
        free (hist->entry);
    }
}

