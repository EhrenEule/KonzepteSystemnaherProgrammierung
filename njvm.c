#include "njvm.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

//size = 100? 
unsigned int programm_memory[100];
int programm_size;
unsigned int instruction;
int stack[100];
int stackpointer;
int programm_counter;
bool halt;

int main(int argc, char *argv[]) {

    printf("Ninja Virtual Machine started\n");
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
        else if(strstr(argv[i], "prog"))
        {
            load_programm(argv[i]);
            halt=false;
            print_programm();
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
        //print_stack();
    }
    
    printf("Ninja Virtual Machine stopped");
    return 0;
}

void load_programm(char *programm_ident)
{
    if(strcmp(programm_ident,"prog1") == 0)
    {
        unsigned int programm_instuctions[]= {
            (PUSHC << 24 | IMMEDIATE(3)),
            (PUSHC << 24 | IMMEDIATE(4)),
            (ADD << 24),
            (PUSHC << 24 | IMMEDIATE(10)),
            (PUSHC << 24 | IMMEDIATE(6)),
            (SUB << 24),
            (MUL << 24), 
            (WRINT << 24),
            (PUSHC << 24 | IMMEDIATE(10)),
            (WRCHR << 24),
            (HALT << 24),
        };
        memcpy(programm_memory,programm_instuctions,sizeof(programm_instuctions));
        programm_size=11;
    }
    else if(strcmp(programm_ident,"prog2") == 0)
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
        memcpy(programm_memory,programm_instuctions,sizeof(programm_instuctions));
        programm_size=9;
    }

    else if(strcmp(programm_ident,"prog3") == 0)
    {
        unsigned int programm_instuctions[]= {
            (RDCHR << 24),
            (WRINT << 24),
            (PUSHC << 24 | IMMEDIATE('\n')),
            (WRCHR << 24),
            (HALT << 24),
        };
        memcpy(programm_memory,programm_instuctions,sizeof(programm_instuctions));
        programm_size=5;
    }
    else 
    {
        printf("Unknown Program.\nExisting now");
        exit(1);
    }

}

void load_programm_from_File(char *programm_path)
{
    //so you only need to give the relative path
    programm_path = "/Tests/Aufgabe2_Tests/%c", programm_path;
    //Mode nochmal auschecken
    FILE *file_managment = fopen(programm_path,'r');

    if(file_managment == NULL)
    {
        printf("Error while loading File");
        exit(1);
    }


    fclose(file_managment);
}

void execute_instruction(unsigned int instruction)
{
    unsigned char opcode = instruction >> 24;

    if((opcode == HALT))
    {
        halt=true;
    }
    else if((opcode == PUSHC))
    {
        int immediate = IMMEDIATE(instruction);
        immediate = SIGN_EXTEND(immediate);
        stack[stackpointer] = immediate;
        stackpointer++;
    }
    else if((opcode == ADD))
    {
        stack[stackpointer-2] = stack[stackpointer-2] + stack[stackpointer-1];
        stack[stackpointer-1] = 0;
        stackpointer--;
    }  
    else if((opcode == SUB))
    {
        stack[stackpointer-2] = stack[stackpointer-2] - stack[stackpointer-1];
        stack[stackpointer-1] = 0;
        stackpointer--;
    } 
    else if((opcode == MUL))
    {
        stack[stackpointer-2] = stack[stackpointer-2] * stack[stackpointer-1];
        stack[stackpointer-1] = 0;
        stackpointer--;
    } 
    else if((opcode == DIV))
    {
        if((stack[stackpointer-1] !=0))
        {
        stack[stackpointer-2] = stack[stackpointer-2] / stack[stackpointer-1];
        stack[stackpointer-1] = 0;
        stackpointer--;
        }
        else
        {
            printf("Cant divide through 0");
            exit(1);
        }
    } 
    else if((opcode == MOD))
    {
        stack[stackpointer-2] = stack[stackpointer-2] % stack[stackpointer-1];
        stack[stackpointer-1] = 0;
        stackpointer--;
    } 
    else if((opcode == RDINT))
    {
        int scanned_num;
        printf("Enter Integer: ");
        scanf("%d",&scanned_num);
        stack[stackpointer] = scanned_num;
        stackpointer++;
    } 
    else if((opcode == WRINT))
    {
        printf("%d",stack[stackpointer-1]);
        stack[stackpointer-1] = 0;
        stackpointer--;
    }
    else if((opcode == RDCHR))
    {
        char scanned_char;
        printf("Enter Char: ");
        scanf(" %c",&scanned_char);
        stack[stackpointer] = scanned_char;
        stackpointer++;
    }
    else if((opcode == WRCHR))
    {
        char wr_char= stack[stackpointer-1];
        printf("%c",wr_char);
        stack[stackpointer-1] = 0;
        stackpointer--;
    } 

}

void print_programm()
{
    int counter=0;
    for(int i=0;i < programm_size;i++ )
    {
        unsigned int instruction = programm_memory[i];
        unsigned char opcode = instruction >> 24;
        if((opcode == HALT))
        {
            printf("%d:\thalt\n", counter);
        }
        else if((opcode == PUSHC))
        {
            int immediate = IMMEDIATE(instruction);
            immediate = SIGN_EXTEND(immediate);
            printf("%d:\tpushc\t%d\n", counter,immediate);
        }
        else if((opcode == ADD))
        {
            printf("%d:\tadd\n", counter);
        }
        else if((opcode == SUB))
        {
            printf("%d:\tsub\n", counter);
        }
        else if((opcode == MUL))
        {
            printf("%d:\tmul\n", counter);
        }
        else if((opcode == DIV))
        {
            printf("%d:\tdiv\n", counter);
        }
        else if((opcode == MOD))
        {
            printf("%d:\tmod\n", counter);
        }
        else if((opcode == RDINT))
        {
            printf("%d:\trdint\n", counter);
        }
        else if((opcode == RDCHR))
        {
            printf("%d:\trdchar\n", counter);
        }
        else if((opcode == WRCHR))
        {
            printf("%d:\twrchr\n", counter);
        }
        else if((opcode == WRINT))
        {
            printf("%d:\twrint\n", counter);
        }
        counter++;
    }
}

void print_stack()

{
    for(int i=0; i < sizeof(stack);i++)
    {
        if(stack[i] != 0)
        {
        printf("Stackpointer:%d\t%d\n",stackpointer,stack[i]);
        }
    }
    printf("\n");
}