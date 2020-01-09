#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "nettool.h"

typedef struct priv_data_S
{
    char ifname[32];
}
priv_data_t;

static priv_data_t priv_data = {0};

int shell_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    char cmd[256];
    if( CLIMINAL_GET_DEFVAL(info) )
        sprintf(cmd, "$SHELL -c \'%s\'", CLIMINAL_GET_DEFVAL(info));
    else
        strcpy(cmd, "$SHELL");
    fprintf(out, "executing %s\n", cmd);
    fflush(out);
    system(cmd);
    return CLIMINAL_NO_ERROR;
}

int info_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    Cmdparam *param=findparam(info, "system");
    if(!strcmp(param->value[0], "os"))
        system("uname -o");
    else if(!strcmp(param->value[0], "hw"))
        system("uname -i");
    else if(!strcmp(param->value[0], "pci"))
        system("lspci");
    else
        fprintf(out, "Invalid subsystem\n");
    return CLIMINAL_NO_ERROR;
}

int info_all_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    fprintf(out,"System info:\n");
    system("uname -a ; lspci");
    return CLIMINAL_NO_ERROR;
}

int config_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    return CLIMINAL_NO_ERROR;
}

int config_net_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    fprintf(out, "Please select one of the possible subcommands\n");
    return CLIMINAL_E_INVALID;
}

int config_net_interface_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    if( CLIMINAL_GET_DEFVAL(info) )
        strcpy(priv_data.ifname, CLIMINAL_GET_DEFVAL(info));
    else
        fprintf( out, "Current interface: \"%s\"\n", priv_data.ifname );
    return CLIMINAL_NO_ERROR;
}

int config_net_ip_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    char command[256];
    sprintf(command, "sudo ifconfig %s %s", priv_data.ifname, info->defval);
    fprintf(out, "executing -- %s\n", command);
    system(command);
    return CLIMINAL_NO_ERROR;
}

int config_net_mask_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    char command[256];
    sprintf(command, "sudo ifconfig eth0 netmask %s", info->defval);
    fprintf(out, "executing -- %s\n", command);
    system(command);
    return CLIMINAL_NO_ERROR;
}

int config_user_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    char command[256];
    sprintf(command, "su - %s", info->defval);
    fprintf(out, "executing -- %s\n", command);
    system(command);
    return CLIMINAL_NO_ERROR;
}

int credits_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    fprintf(out, "Not much to say:\n - Marco Zac (zacmarco@gmail.com)\n\nENJOY!!!\n");
    return CLIMINAL_NO_ERROR;
}

int config_interface_up_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    return CLIMINAL_NO_ERROR;
}

int config_interface_down_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    return CLIMINAL_NO_ERROR;
}

int info_system_val_cbk(char **val, const char *cookie) {
    int i=0;
    val[i++]=strdup("os");
    val[i++]=strdup("hw");
    val[i++]=strdup("pci");

    return i;
}
