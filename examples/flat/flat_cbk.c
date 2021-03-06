#include <unistd.h>
#include <string.h>
#include "flat.h"

int cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line, void *cookie)
{
    int param, val;

    if(cookie) {
        fprintf(out, "Cookie: %s\n", (char*)cookie);
    }

    fprintf(out, "Full command line: \"%s\"\n", line);

    if( CLIMINAL_GET_DEFVAL(info) )
        fprintf(out, "Default Value: \"%s\"\n", CLIMINAL_GET_DEFVAL(info));

    for( param=0; param<CLIMINAL_GET_PARAM_NUM(info); ++param ) {
        fprintf(out, "Parameter n.%d: \"%s\"\n", param, CLIMINAL_GET_PARAM_NAME(info, param));
        for( val=0; val<CLIMINAL_GET_PARAM_NUM_VAL(info, param); ++val) {
            fprintf(out, "Value n.%d: \"%s\"\n", val, CLIMINAL_GET_PARAM_VAL(info, param)[val]);
        }
        
    }
    return CLIMINAL_NO_ERROR;
}

