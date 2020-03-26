#ifndef _HISTORY_H_
#define _HISTORY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef HISTORY_SIZE
#define HISTORY_SIZE    (1023)
#endif

#ifndef MAX_BUFLEN
#define MAX_BUFLEN      (256)
#endif

#define HISTORY_ARRAY_SIZE    (HISTORY_SIZE+1)

/* Flags used to get history entries */
#define HISTORY_AFTER   (0)
#define HISTORY_BEFORE  (1)

#define CURS_L(outFile, num)      { int i; for(i=0; i<num; ++i) { fprintf(outFile, "%c%c%c", 0x1b, 0x5b, 0x44); } }
#define CURS_R(outFile, num)      { int i; for(i=0; i<num; ++i) { fprintf(outFile, "%c%c%c", 0x1b, 0x5b, 0x43); } } 

    typedef char   HistoryBuffer[MAX_BUFLEN];
    typedef struct Clihistory Clihistory;

    int  init_history    ( Clihistory *history, const unsigned int size );
    void free_history    ( Clihistory *history);
    void add_history     ( Clihistory *history, const char *in_buf );
    int  get_history     ( Clihistory *history, char *out_buf, int direction );
    int  islast_history  ( Clihistory *history, char *str);

    struct Clihistory 
    {
        unsigned int     newid;
        unsigned int     getid;
        unsigned int     lastid;
        unsigned int     cmdnum;
        unsigned int     size;
        HistoryBuffer    *entry;
    };

#ifdef __cplusplus
}
#endif

#endif

