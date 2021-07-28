#include "njvm.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>


/*
* nur vorzeichenlose ganzzahlen rechtsschieben
*/




//wenn das vor dem Fragezeichen true(1) ist ist i = i | 0xFF000000,
//falls das vor dem Fragezeichen false(0) ist ist i = i
//#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))

//size = 100? 
unsigned int programm_memory[100];
unsigned int instruction;
int stack[100];
int stackpointer;
int programm_counter;
bool halt;

int main(int argc, char *argv[]) {

    printf("moce");
    halt=true;
    for(int i=1; i< argc; i++)

    {
        if(strcmp(argv[i], "--version") == 0)
        {
            printf("Version: %f\n",VERSION);
        }
        else if(strcmp(argv[i], "--help") == 0)
        {
            printf("usage: ./njvm [option] [option] ...\n");
            printf("  --version        show version and exit\n");
            printf("  --help           show this help and exit\n");
        }
        else if(strstr(argv[i], "programm"))
        {
            load_programm(argv[i]);
            halt=false;
            //print programm 1 NUR DEBUG
            for(int i=0;i<10;i++)
            {
                printf("%d", programm_memory[i]);
            }
        }
        else
        {
            printf("unknown command line argument '%s', try './njvm --help'\n", argv[i]);
        }
    }

    while(!halt)
    {
        instruction = programm_memory[programm_counter];
        programm_counter++;
        execute_instruction(instruction);
    }

    return 0;
}

void load_programm(char *programm_ident)
{
    if(strcmp(programm_ident,"programm1"))
    {
        unsigned int programm_instuctions[]= {
            (PUSHC << 24 | IMMEDIATE(3)),
            (PUSHC << 24 | IMMEDIATE(4)),
            (ADD << 24),
            (PUSHC << 24 | IMMEDIATE(10)),
            (PUSHC << 24 | IMMEDIATE(6)),
            (SUB << 24),
            (MUL << 24), 
            (PUSHC << 24 | IMMEDIATE(10)),
            (WRCHR << 24),
            (HALT << 24),
        };
        memcpy(programm_memory,programm_instuctions,sizeof(programm_instuctions)/sizeof(programm_instuctions[0]));
    }
    else if(strcmp(programm_ident,"programm2"))
    {
        unsigned int programm_instuctions[]= {
            (PUSHC << 24 | IMMEDIATE(-2)),
            (RDINT << 24),
            (MUL << 24), 
            (PUSHC << 24 | IMMEDIATE(3)),
            (ADD << 24),
            (WRINT << 24),
            (PUSHC << 24 | IMMEDIATE('\n')),
            (WRCHR << 24),
            (HALT << 24),
        };
        memcpy(programm_memory,programm_instuctions,sizeof(programm_instuctions)/sizeof(programm_instuctions[0]));
    }
    else if(strcmp(programm_ident,"programm3"))
    {
        unsigned int programm_instuctions[]= {
            (RDCHR << 24),
            (WRINT << 24),
            (PUSHC << 24 | IMMEDIATE('\n')),
            (WRCHR << 24),
            (HALT << 24),
        };
        memcpy(programm_memory,programm_instuctions,sizeof(programm_instuctions)/sizeof(programm_instuctions[0]));
    }

}

void execute_instruction(unsigned int instruction)
{
    unsigned char opcode = instruction >> 24;
    int immediate;
    immediate = instruction & 0x00FFFFFF;
    if((opcode = HALT))
    {
        halt=true;
    }
    else if((opcode = PUSHC))
    {
        stack[stackpointer] = immediate;
        stackpointer++;
    }
    else if((opcode = ADD))
    {
        stack[stackpointer-2] = stack[stackpointer-1] + stack[stackpointer-2];
        stackpointer--;
    }  
    else if((opcode = SUB))
    {
        stack[stackpointer-2] = stack[stackpointer-1] - stack[stackpointer-2];
        stackpointer--;
    } 
    else if((opcode = MUL))
    {
        stack[stackpointer-2] = stack[stackpointer-1] * stack[stackpointer-2];
        stackpointer--;
    } 
    else if((opcode = DIV))
    {
        if((stack[stackpointer-2] !=0))
        {
        stack[stackpointer-2] = stack[stackpointer-1] / stack[stackpointer-2];
        stackpointer--;
        }
        else
        {
            printf("Cant divide through 0");
            exit(1);
        }
    } 
    else if((opcode = MOD))
    {
        stack[stackpointer-2] = stack[stackpointer-1] % stack[stackpointer-2];
        stackpointer--;
    } 
    else if((opcode = RDINT))
    {
        int *scanned_num= NULL;
        printf("Enter Integer: ");
        scanf("%d",scanned_num);
        stack[stackpointer] = (uintptr_t)scanned_num;
        stackpointer++;
    } 
    else if((opcode = WRINT))
    {
        printf("%d",stack[stackpointer]);
        stackpointer--;
    }
    else if((opcode = RDCHR))
    {
        char *scanned_char= NULL;
        printf("Enter Integer: ");
        scanf("%c",scanned_char);
        stack[stackpointer] = (uintptr_t)scanned_char;
        stackpointer++;
    }
    else if((opcode = WRCHR))
    {
        char wr_char= stack[stackpointer]+ '0';
        printf("%c",wr_char);
        stackpointer--;
    } 
}

void print_programm()
{

}