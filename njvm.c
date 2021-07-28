#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*
* nur vorzeichenlose ganzzahlen rechtsschieben
*/

#define VERSION 0.1

#define HALT 0
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
//wenn das vor dem Fragezeichen true(1) ist ist i = i | 0xFF000000,
//falls das vor dem Fragezeichen false(0) ist ist i = i
//#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))

unsigned int programm_memory[];
unsigned int instruction;
int stack[];
int stackpointer=0;
int programm_counter=0;
bool halt;

int main(int argc, char *argv[]) {

    for(int i=0; i< argc; i++)

    {
        if(strcmp(argv[i], "--version"))
        {
            printf("Version: %f",VERSION);
        }
        else if(strcmp(argv[i], "--help"))
        {
            printf("usage: ./njvm [option] [option] ...\n");
            printf("  --version        show version and exit\n");
            printf("  --help           show this help and exit\n");
        }
        else if(strstr(argv[i], "programm"))
        {
            load_programm(argv[i]);
        }
        else
        {
            printf("unknown command line argument '%s', try './njvm --help'\n", argv[i]);
        }
    }

    while(!halt)
    {
        instruction = programm_memory[programm_counter];
        programm_counter=programm_counter++;
        execute_instruction(instruction);
    }

    return 0;
}

load_programm(char programm_ident)
{
    if(strcmp(programm_ident,"programm 1"))
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
    else if(strcmp(programm_ident,"programm 2"))
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
    else if(strcmp(programm_ident,"programm 3"))
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

execute_instruction(unsigned int instruction)
{
    unsigned char opcode = instruction >> 24;
    unsigned int immediate = instruction & 0x00FFFFFF;
    if(opcode = HALT)
    {
        halt=true;
    }
    else if(opcode = PUSHC)
    {
        stack[stackpointer] = immediate;
        stackpointer=stackpointer++;
    }
    else if(opcode = ADD)
    {
        stack[stackpointer-2] = stack[stackpointer-1] + stack[stackpointer-2];
        stackpointer=stackpointer--;
    }  
    else if(opcode = SUB)
    {
        stack[stackpointer-2] = stack[stackpointer-1] - stack[stackpointer-2];
        stackpointer=stackpointer--;
    } 
    else if(opcode = MUL)
    {
        stack[stackpointer-2] = stack[stackpointer-1] * stack[stackpointer-2];
        stackpointer=stackpointer--;
    } 
    else if(opcode = DIV)
    {
        if(stack[stackpointer-2] !=0)
        stack[stackpointer-2] = stack[stackpointer-1] / stack[stackpointer-2];
        stackpointer=stackpointer--;
        else
        {
            printf("Cant divide through 0");
            exit(1);
        }
    } 
    else if(opcode = MOD)
    {
        stack[stackpointer-2] = stack[stackpointer-1] % stack[stackpointer-2];
        stackpointer=stackpointer--;
    } 
    else if(opcode = ADD)
    {
        stack[stackpointer-2] = stack[stackpointer-2] + stack[stackpointer-1];
        stackpointer=stackpointer--;
    } 
}
