#include <stdio.h>
#include <string.h>
#define VERSION 0.1

int main(int argc, char *argv[]) {

    for(int i=0; i< argc; i++)
    {
        if(strcmp(argv[i], "--version"))
        {
            printf("Version: %c",VERSION);
        }
        else if(strcmp(argv[i], "--help"))
        {
            printf("usage: ./njvm [option] [option] ...\n");
            printf("  --version        show version and exit\n");
            printf("  --help           show this help and exit\n");
        }
        else
        {
            printf("unknown command line argument '%c', try './njvm --help'\n", argv[i]);
        }
    }
    return 0;
}
