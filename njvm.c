#include "njvm.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>


unsigned int *programm_memory;
unsigned int *variable_memory;
int programm_size;
unsigned int instruction;
int stack[10000];
int stackpointer;
int framepointer;
int programm_counter;
bool halt;

int main(int argc, char *argv[]) {

    printf("Ninja Virtual Machine started\n");
    halt=true;
    
    if(strcmp(argv[1], "--version") == 0)
    {
        printf("Version: %d\n",VERSION);
    }
    else if(strcmp(argv[1], "--help") == 0)
    {
        printf("usage: ./njvm [option] [option] ...\n");
        printf("  --version        show version and exit\n");
        printf("  --help           show this help and exit\n");
    }
    else if(strstr(argv[1], "prog"))
    {
        char relative_programm_path[80] = "/home/student/Desktop/KonzepteSystemnaherProgrammierung/Tests/";
        strncat(relative_programm_path, argv[1], strlen(argv[1]));
        load_programm_from_File(relative_programm_path);
        halt=false;
        print_programm();
    }
    else if(argv[1] != NULL)
    {
        printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
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

void load_programm_from_File(char *programm_path)
{
    //Mode nochmal auschecken
    FILE *file_managment = fopen(programm_path,"r");
    if(file_managment == NULL)
    {
        printf("Error while opening File");
        exit(99);
    }
    //Verify the format of the File
    char format_verify[4];
    if(fread( &format_verify[0], sizeof(char), 4, file_managment) != 4) { exit(99); }

    if(!((format_verify[0] == 'N') & (format_verify[1] == 'J') & (format_verify[2] == 'B') & (format_verify[3] == 'F'))) 
    {
        printf("Unknown Format");
        exit(99);
    }
    //Read the version number
    int version_read;
    if(fread( &version_read, sizeof(int), 1, file_managment) != 1){ exit(99); }
    //Verify the version number
    if((version_read != VERSION)) { printf("Version missmatch"); exit(99); }

    //Read the number of instructions
    int instruction_count;
    if(fread( &instruction_count, sizeof(int), 1, file_managment) != 1) { printf("Error, noob"); exit(99); }
    programm_size = instruction_count;
    //Allocate memory for instructions
    unsigned int *temp_programm_memory= malloc(sizeof(int) * instruction_count);
    if(temp_programm_memory == NULL) { printf("Error while allocating instruction memory"); exit(99); } 
    programm_memory = temp_programm_memory;

    //Read number of Variables
    int variable_count;
    if(fread( &variable_count, sizeof(int), 1, file_managment) != 1) { printf("Error, noob"); exit(99); }
    //Allocate memory for variables
    unsigned int *temp_variable_memory= malloc(sizeof(int) * variable_count);
    if(temp_variable_memory == NULL) { printf("Error while allocating variable memory"); exit(99); } 
    variable_memory = temp_variable_memory;

    //Load instructions into programm_memory
    if(fread( &programm_memory[0], sizeof(int), instruction_count, file_managment) != instruction_count) { printf("Error, noob"); exit(99); }

    stackpointer=0;
    programm_counter=0;
    framepointer=0;

    fclose(file_managment);
}

void execute_instruction(unsigned int instruction)
{
    unsigned char opcode = instruction >> 24;
    int immediate = IMMEDIATE(instruction);
    immediate = SIGN_EXTEND(immediate);

    if((opcode == HALT))
    {
        halt=true;
    }
    else if((opcode == PUSHL))
    {
        stack[stackpointer] = stack[framepointer+immediate];
        stackpointer++;
    }
    else if((opcode == POPL))
    {
        stack[framepointer+immediate] = stack[stackpointer-1];
        stackpointer--;
    }
    else if((opcode == ASF))
    {
        stack[stackpointer] = framepointer;
        framepointer = stackpointer;
        stackpointer = stackpointer + immediate;
    }
    else if((opcode == RSF))
    {
        stackpointer = framepointer;
        framepointer = stack[stackpointer-1];
        stackpointer--;
    }
    else if((opcode == PUSHG))
    {
        stack[stackpointer] = variable_memory[immediate];
        stackpointer++;
    }
    else if((opcode == POPG))
    {
        variable_memory[immediate] = stack[stackpointer-1];
        stackpointer--;
    }
    else if((opcode == PUSHC))
    {
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
        int immediate = IMMEDIATE(instruction);
            immediate = SIGN_EXTEND(immediate);

        if((opcode == HALT))
        {
            printf("%d:\thalt\n", counter);
        }
        else if((opcode == ASF))
        {
            printf("%d:\tasf\t%d\n", counter,immediate);
        }
        else if((opcode == RSF))
        {
            printf("%d:\trsf\t\n", counter);
        }
        else if((opcode == PUSHL))
        {
            printf("%d:\tpushl\t%d\n", counter,immediate);
        }
        else if((opcode == POPL))
        {
            printf("%d:\tpopl\t%d\n", counter,immediate);
        }
        else if((opcode == PUSHC))
        {
            printf("%d:\tpushc\t%d\n", counter,immediate);
        }
        else if((opcode == PUSHG))
        {
            printf("%d:\tpushg\t%d\n", counter,immediate);
        }
        else if((opcode == POPG))
        {
            printf("%d:\tpopg\t%d\n", counter,immediate);
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