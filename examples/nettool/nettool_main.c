#include "nettool.h"
#include "nettool_common.h"

static priv_data_t priv_data = {0};

int main()
{
    return climinal_main(stdin,stdout,climinalhandle_nettool(), &priv_data);  
}

