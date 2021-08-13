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


unsigned int* programm_memory;
ObjRef variable_memory;
int programm_memory_size;
int variable_memory_size;
unsigned int instruction;
Stackslot stack[10000];
bool halt=true;
bool debug=false;
int breakpoint=-10;
//Registers
int stackpointer;
int framepointer;
int programm_counter;
ObjRef return_value_register;



int main(int argc, char *argv[]) {

    printf("Ninja Virtual Machine started\n");
    
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
    }
    else if(argv[1] != NULL)
    {
        printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
    }
    //To activate debug mode
    if((argc == 3) && (strcmp(argv[2], "--debug") == 0)){ debug=true; }

    //This is where the magic happens
    while(!halt)
    {
        instruction = programm_memory[programm_counter];
        if(breakpoint == programm_counter) { debug=true; }
        while(debug)
        {
            print_instruction(programm_counter,instruction);
            bool should_step=debug_menu();
            if(should_step) { break; };
        }
        programm_counter++;
        execute_instruction(instruction);
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
    programm_memory_size = instruction_count;
    //Allocate memory for instructions
    unsigned int *temp_programm_memory= malloc(sizeof(int) * instruction_count);
    if(temp_programm_memory == NULL) { printf("Error while allocating instruction memory"); exit(99); } 
    programm_memory = temp_programm_memory;

    //Read number of Variables
    int variable_count;
    if(fread( &variable_count, sizeof(int), 1, file_managment) != 1) { printf("Error, noob"); exit(99); }
    variable_memory_size = variable_count;
    //Allocate memory for variables
    ObjRef temp_variable_memory= malloc((sizeof(int)+sizeof(unsigned int)) * variable_count);
    if(temp_variable_memory == NULL) { printf("Error while allocating variable memory"); exit(99); } 
    variable_memory = temp_variable_memory;

    //Load instructions into programm_memory
    if(fread( &programm_memory[0], sizeof(int), instruction_count, file_managment) != instruction_count) { printf("Error, noob"); exit(99); }

    stackpointer=0;
    programm_counter=0;
    framepointer=0;

    //malloc for return_value_register
    return_value_register = malloc(sizeof(unsigned int)+ sizeof(int));
    return_value_register->size = sizeof(int);

    //Initialise Stack with -99 Objects
    for(int i=0; i< (sizeof(stack)/ sizeof(stack[0])); i++)
    {
        stack[i]= PushObject(-99);
    }

    fclose(file_managment);
}

void execute_instruction(unsigned int instruction)
{
    unsigned char opcode = instruction >> 24;
    int immediate = IMMEDIATE(instruction);
    immediate = SIGN_EXTEND(immediate);

    switch(opcode)
    {
        case HALT:
            halt=true;
            break;

        case PUSHC:
            stack[stackpointer] = PushObject(immediate);
            stackpointer++;
            break;
        
        case ADD:
            stack[stackpointer-2] = PushObject(PopObject(stackpointer-2) + PopObject(stackpointer-1));
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
   
        case SUB:
            stack[stackpointer-2] = PushObject(PopObject(stackpointer-2) - PopObject(stackpointer-1));
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
    
        case MUL:
            stack[stackpointer-2] = PushObject(PopObject(stackpointer-2) * PopObject(stackpointer-1));
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
    
        case DIV:
            if((*(int *)(stack[stackpointer-1].u.objRef->data) !=0))
            {
                stack[stackpointer-2] = PushObject(PopObject(stackpointer-2) / PopObject(stackpointer-1));
                stack[stackpointer-1] = PushObject(-99);
                stackpointer--;
            }
            else
            {
                printf("Cant divide through 0");
                exit(1);
            }
            break;
    
        case MOD: ;
            stack[stackpointer-2] = PushObject(PopObject(stackpointer-2) * PopObject(stackpointer-1));
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case RDINT: ;
            int scanned_num;
            printf("Enter Integer: ");
            scanf("%d",&scanned_num);
            stack[stackpointer] = PushObject(scanned_num);
            stackpointer++;
            break;
    
        case WRINT:

            printf("%d",PopObject(stackpointer-1));
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
    
        case RDCHR: ;
            char scanned_char;
            printf("Enter Char: ");
            scanf(" %c",&scanned_char);
            stack[stackpointer] = PushObject(scanned_char);
            stackpointer++;
            break;
    
        case WRCHR: ;
            printf("%c",PopObject(stackpointer-1));
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
        
        case PUSHG:
            stack[stackpointer] = PushObject(*(int *)variable_memory[immediate].data);
            stackpointer++;
            break;
    
        case POPG: ;
            ObjRef objref = malloc(sizeof(unsigned int)+ sizeof(int));
            objref->size = sizeof(int);
            *(int *)objref->data = PopObject(stackpointer-1);
            variable_memory[immediate] = *objref;
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
        
        case ASF:
            stack[stackpointer] = PushValue(framepointer);
            stackpointer++;
            framepointer = stackpointer;
            stackpointer = stackpointer + immediate;
            break;
    
        case RSF:
            stackpointer = framepointer;
            framepointer = PopValue(stackpointer-1);
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
        
        case PUSHL:
            stack[stackpointer] = PushObject(PopObject(framepointer+immediate));
            stackpointer++;
            break;
        
        case POPL:
            stack[framepointer+immediate] = PushObject(PopObject(stackpointer-1));
            stackpointer--;
            break;

        case EQ:
            if( (*(int *)stack[stackpointer-2].u.objRef->data) == (*(int *)stack[stackpointer-1].u.objRef->data) )
            {
                stack[stackpointer-2] = PushValue(1);
            }
            else
            {
                stack[stackpointer-2] = PushValue(0);
            }
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case NE:
            if( (*(int *)stack[stackpointer-2].u.objRef->data) != (*(int *)stack[stackpointer-1].u.objRef->data))
            {
                stack[stackpointer-2] = PushValue(1);
            }
            else
            {
                stack[stackpointer-2] = PushValue(0);
            }
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case LT:
            if((*(int *)stack[stackpointer-2].u.objRef->data) < (*(int *)stack[stackpointer-1].u.objRef->data))
            {
                stack[stackpointer-2] = PushValue(1);
            }
            else
            {
                stack[stackpointer-2] = PushValue(0);
            }
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case LE:
            if((*(int *)stack[stackpointer-2].u.objRef->data) <= (*(int *)stack[stackpointer-1].u.objRef->data))
            {
                stack[stackpointer-2] = PushValue(1);
            }
            else
            {   
                stack[stackpointer-2] = PushValue(0);
            }
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case GT:
            if( (*(int *)stack[stackpointer-2].u.objRef->data) > (*(int *)stack[stackpointer-1].u.objRef->data))
            {
                stack[stackpointer-2] = PushValue(1);
            }
            else
            {
                stack[stackpointer-2] = PushValue(0);
            }
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case GE:
            if( (*(int *)stack[stackpointer-2].u.objRef->data) >= (*(int *)stack[stackpointer-1].u.objRef->data))
            {
                stack[stackpointer-2] = PushValue(1);
            }
            else
            {
                stack[stackpointer-2] = PushValue(0);
            }
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case JMP:
            programm_counter = immediate;
            break;

        case BRF:
            if(stack[stackpointer-1].u.number == 0) { programm_counter = immediate; }
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case BRT:
            if(stack[stackpointer-1].u.number == 1) { programm_counter = immediate; } 
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;

        case CALL:
            stack[stackpointer] = PushValue(programm_counter);
            stackpointer++;
            programm_counter = immediate;
            break;
        
        case RET:
            programm_counter = PopValue(stackpointer-1);
            stack[stackpointer-1] = PushObject(-99); 
            stackpointer--;
            break;
        
        case DROP:
            for(int i=0;i < immediate;i++) 
            { 
                stack[stackpointer-1] = PushObject(-99);
            }
            stackpointer= stackpointer - immediate;
            break;
        
        case PUSHR:
            stack[stackpointer] = PushObject(*(int *)return_value_register->data);
            stackpointer++;
            break;
        
        case POPR:
            *(int *)return_value_register->data = PopObject(stackpointer-1);
            stack[stackpointer-1] = PushObject(-99);
            stackpointer--;
            break;
        
        case DUP:
            stack[stackpointer] = stack[stackpointer-1];
            stackpointer++;
            break;
        
        default:
            printf("Unkown Opcode:%d\tImmediate:%d", opcode,immediate);
            exit(99);
    }
}

void print_programm()
{
    for(int i=0;i < programm_memory_size;i++ )
    {
        unsigned int instruction = programm_memory[i];
        print_instruction(i,instruction);
    }
    printf("\n");
}

void print_stack()
{
    printf("--Stack--\n");
    for(int i=(sizeof(stack)/sizeof(stack[0])-1);i >= 0 ; i--)
    {
        if(i <= stackpointer)
        {
            if(stack[i].isObjRef == 1)
            {
                if(i == framepointer && i == stackpointer) 
                { printf("sp,fp-->"); printf("\t%d\n",(*(int *)stack[i].u.objRef->data)); }
                else if(i == framepointer) 
                { printf("fp-->"); printf("\t%d\n",*(int *)stack[i].u.objRef->data); }
                else if(i== stackpointer) { printf("sp-->\txxx\n"); }
                else { printf("\t%d\n",*(int *)stack[i].u.objRef->data); }
            }
            else if(stack[i].isObjRef == 0)
            {
                if(i == framepointer && i == stackpointer) 
                { printf("sp,fp-->"); printf("\t%d\n",stack[i].u.number); }
                else if(i == framepointer) 
                { printf("fp-->"); printf("\t%d\n",stack[i].u.number); }
                else if(i== stackpointer) { printf("sp-->\txxx\n"); }
                else { printf("\t%d\n",stack[i].u.number); }
            }
            
        }
    }
    printf("--End of Stack--\n\n");
}

void print_static_data()
{
    printf("--Static data--\n");
    for(int i=0;i < variable_memory_size;i++)
    {
        printf("data[%d]:%d\n",i,*(int *) variable_memory[i].data);
    }
    printf("--End of static data--\n\n");
}

bool debug_menu()
{
    
    printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
    char input_str[10];
    scanf("%s", input_str);
    if(strcmp(input_str, "inspect") == 0)
    {
        printf("stack, data\n");
        char inspect_input_str[10];
        scanf("%s", inspect_input_str);
        if(strcmp(inspect_input_str,"stack") == 0) { print_stack(); }
        else if(strcmp(inspect_input_str,"data") == 0) { print_static_data(); }
    }
    else if(strcmp(input_str, "list") == 0)
    {
        print_programm();
    }
    else if(strcmp(input_str, "breakpoint") == 0)
    {
        printf("Enter breakpoint:");
        int scanned_breakpoint;
        scanf("%d", &scanned_breakpoint);
        if(scanned_breakpoint > programm_memory_size-1) { printf("Irregular Breakpoint"); exit(99); }
        breakpoint=scanned_breakpoint;
        debug=false;
    }
    else if(strcmp(input_str, "step") == 0)
    {
        return true;
    }
    else if(strcmp(input_str, "run") == 0) { debug=false; }
    else if(strcmp(input_str, "quit") == 0) { exit(0); }
    return false;
}

void print_instruction(int number,unsigned int instruction)
{
        unsigned char opcode = instruction >> 24;
        int immediate = IMMEDIATE(instruction);
        immediate = SIGN_EXTEND(immediate);
        switch(opcode)
        {    
            case HALT:
                printf("%d:\thalt\n",number);
                break;
        
            case PUSHC:
                printf("%d:\tpushc\t%d\n",number,immediate);
                break;

            case ADD:
                printf("%d:\tadd\n", number);
                break;

            case SUB:
                printf("%d:\tsub\n", number);
                break;

            case MUL:
                printf("%d:\tmul\n", number);
                break;

            case DIV:
                printf("%d:\tdiv\n", number);
                break;

            case MOD:
                printf("%d:\tmod\n", number);
                break;

            case RDINT:
                printf("%d:\trdint\n", number);
                break;

            case WRINT:
                printf("%d:\twrint\n", number);
                break;

            case RDCHR:
                printf("%d:\trdchar\n", number);
                break;

            case WRCHR:
                printf("%d:\twrchr\n", number);
                break;
            
            case PUSHG:
                printf("%d:\tpushg\t%d\n", number,immediate);
                break;

            case POPG:
                printf("%d:\tpopg\t%d\n", number,immediate);
                break;
            
            case PUSHL:
                printf("%d:\tpushl\t%d\n", number,immediate);
                break;

            case POPL:
                printf("%d:\tpopl\t%d\n", number,immediate);
                break;

            case ASF:
                printf("%d:\tasf\t%d\n", number,immediate);
                break;
        
            case RSF:
                printf("%d:\trsf\t\n", number);
                break;

            case EQ:
                printf("%d:\teq\t\n", number);
                break;
      
            case NE:
                printf("%d:\tne\t\n", number);
                break;

            case LT:
                printf("%d:\tllt\t\n", number);
                break;

            case LE:
                printf("%d:\tle\t\n", number);
                break;

            case GT:
                printf("%d:\tgt\t\n", number);
                break;

            case GE:
                printf("%d:\tge\t\n", number);
                break;

            case JMP:
                printf("%d:\tjmp\t%d\n", number,immediate);
                break;

            case BRF:
                printf("%d:\tbrf\t%d\n", number,immediate);
                break;

            case BRT:
                printf("%d:\tbrt\t%d\n", number,immediate);
                break;
            
            case CALL:
                printf("%d:\tcall\t%d\n", number,immediate);
                break;

            case RET:
                printf("%d:\tret\t\n", number);
                break;

            case DROP:
                printf("%d:\tdrop\t%d\n", number,immediate);
                break;

            case PUSHR:
                printf("%d:\tpushr\t\n", number);
                break;

            case POPR:
                printf("%d:\tpopr\t\n", number);
                break;

            case DUP:
                printf("%d:\tge\t\n", number);
                break; 

            default:
                printf("Unkown Opcode");
        }
}

Stackslot PushObject(int value)
{
    ObjRef refobj;
    refobj = malloc(sizeof(unsigned int)+ sizeof(int));
    refobj->size = sizeof(int);
    *(int *)refobj->data = value;

    Stackslot stackobj;
    stackobj.isObjRef = 1;
    stackobj.u.objRef = refobj;
    return stackobj;
}

int PopObject(int local_stackpointer)
{
    if(stack[local_stackpointer].isObjRef == 1)
    {
        ObjRef refobj = stack[local_stackpointer].u.objRef;
        return *(int *)refobj->data;
    }
    else
    {
        printf("error while PopObject due to Type missmatch");
        exit(99);
    }
}

Stackslot PushValue(int value)
{
    Stackslot stackobj;
    stackobj.isObjRef = 0;
    stackobj.u.number = value;
    return stackobj;
}

int PopValue(int local_stackpointer)
{
    if(stack[local_stackpointer].isObjRef == 0)
    {
        return stack[local_stackpointer].u.number;
    }
    else 
    {
        printf("error while PopValue due to Type missmatch");
        exit(99);
    }
}