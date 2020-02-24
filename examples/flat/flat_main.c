#include "flat.h"

static char message[]="This is our session cookie";

int main()
{
    return climinal_main(stdin,stdout,climinalhandle_flat(), message);  
}

