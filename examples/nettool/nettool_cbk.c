#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ifaddrs.h>

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

int config_net_interface_values(char **val, const char *cookie) {
    struct ifaddrs *ifaddr, *ifa;
    int n;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    /* Walk through linked list, maintaining head pointer so we
       can free list later */

    ifa=ifaddr;
    n=0;

    while(ifa) {
        if(ifa->ifa_addr->sa_family==AF_PACKET) {
            val[n++]=strdup(ifa->ifa_name);
        }
        ifa=ifa->ifa_next;
    }

    return n;
}

int config_net_interface_cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line)
{
    struct ifaddrs *ifaddr, *ifa;
    int found=0;
    char *val;
    int err=CLIMINAL_NO_ERROR;

    val=CLIMINAL_GET_DEFVAL(info);

    if(val) {
        if (getifaddrs(&ifaddr) == -1) {
            err=CLIMINAL_E_INVALID;
            goto exit;
        }

        ifa=ifaddr;
        while( (ifa) && (!found) ) {
            if(ifa->ifa_addr->sa_family==AF_PACKET) {
                if(!strcmp(ifa->ifa_name, val)) {
                    found=1;
                }
            }
            ifa=ifa->ifa_next;
        }

        freeifaddrs(ifaddr);

        if( found ) {
            strcpy(priv_data.ifname, val);
            fprintf( out, "Current interface: \"%s\"\n", priv_data.ifname );
        } else {
            fprintf( out, "Interface \'%s\' not found\n", val );
            err=CLIMINAL_E_NOT_FOUND;
            goto exit;
        }
    } else {
        if( strlen(priv_data.ifname) > 0 ) {
            fprintf( out, "Current interface: \"%s\"\n", priv_data.ifname );
            err=CLIMINAL_E_GENERIC;
            goto exit;
        } else {
            fprintf( out, "No interface selected yet\n" );
            err=CLIMINAL_E_NOT_FOUND;
            goto exit;
        }
    }
exit:
    return err;
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
