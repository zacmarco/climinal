#include "cpp.h"
#include <iostream>
#include <cstdio>
#include <string>

/* COMMAND: test		( main )		*/
int test_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line, void *cookie)
{
    std::string task(*((std::string*)cookie));
    fprintf(out, "Test callback called on task %s\n", task.c_str());

    return 0;
}
