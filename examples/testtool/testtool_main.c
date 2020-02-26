#define _GNU_SOURCE
#include <sys/types.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include "testtool.h"

#if __linux
static FILE *outfile=NULL;
static FILE *in, *out;
struct termios oldt;

ssize_t mywrite(void *c, const char *buf, size_t size)
{
    ssize_t ret;

    ret=write(fileno(c), buf, size);;
    sync();

    return ret;
}

ssize_t myread(void *c, char *buf, size_t size)
{
    ssize_t ret;
    ret=read(fileno(c), buf, size);;
    if(outfile && (ret>0)) {
        write(fileno(outfile), buf, ret);
    }
    return ret;
}


int myseek(void *c, off_t *offset, int whence)
{
    return (int)lseek(fileno(c), *offset, whence);
    return 0;
}

int myclose(void *c)
{
    return close(fileno(c));
}

void set_terminal(int signal)
{
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag |=  (ECHONL) ;
    newt.c_lflag &=~ (ICANON | ECHO | ECHOE);
    newt.c_cc[VTIME] = 10;
    tcsetattr(fileno(in), TCSANOW, &newt);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void reset_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    tcsetattr(fileno(in), TCSANOW, &oldt);
}
       
int main(int argc, const char **argv)
{
    int ret;

    if(argc == 2) {
        outfile=fopen(argv[1], "w+");
        if(!outfile) {
            printf("Unable to open file %s\n", argv[0]);
        }
    }

    cookie_io_functions_t  func = {
        .read  = myread,
        .write = mywrite,
        .seek  = myseek,
        .close = myclose
    };

    out=fopencookie(stdout, "a+", func);
    in=fopencookie(stdin, "r", func);

    setvbuf(out, NULL, _IONBF, 0);

    if(in && out) {
        signal(SIGCONT, set_terminal);
        ret=climinal_main(in,out,climinalhandle_testtool(), NULL);  
        reset_terminal();

        fclose(in);
        fclose(out);
    }


    if(outfile) {
        fclose(outfile);
    }


    return 0;
}

#else
int main()
{
    return climinal_main(stdin,stdout,climinalhandle_testtool(), NULL);  
}
#endif
