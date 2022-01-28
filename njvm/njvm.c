
#include "bigint.h"
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
ObjRef *variable_memory;
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
    else if(strstr(argv[1], "home"))
    {
        load_programm_from_File(argv[1]);
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
    ObjRef *temp_variable_memory =(ObjRef*) malloc((sizeof(ObjRef) * variable_count));
    if(temp_variable_memory == NULL) { printf("Error while allocating variable memory"); exit(99); } 
    variable_memory = temp_variable_memory;

    //initialise gobal_variable_memory with nil
    for(int i=0;i< variable_count;i++)
    {
        variable_memory[i] = NULL;
    }

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
        stack[i]= PushNil();
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
            bigFromInt(immediate);
            stack[stackpointer] = PushObjectRef(bip.res);
            stackpointer++;
            break;
        
        case ADD:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            bigAdd();
            stack[stackpointer-2] = PushObjectRef(bip.res);
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;
   
        case SUB:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            bigSub();
            stack[stackpointer-2] = PushObjectRef(bip.res);
            stack[stackpointer-1] =PushNil();
            stackpointer--;
            break;
    
        case MUL:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            bigMul();
            stack[stackpointer-2] = PushObjectRef(bip.res);
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;
    
        case DIV:
            bip.op1 =stack[stackpointer-1].u.objRef;
            int numbernotnull = bigToInt();
            //if((*(int *)(stack[stackpointer-1].u.objRef->data) !=0))
            if(numbernotnull != 0)
            {
                bip.op1 = stack[stackpointer-2].u.objRef;
                bip.op2 = stack[stackpointer-1].u.objRef;
                bigDiv();
                stack[stackpointer-2] = PushObjectRef(bip.res);
                stack[stackpointer-1] = PushNil();
                stackpointer--;
            }
            else
            {
                printf("Cant divide through 0");
                exit(1);
            }
            break;

        case MOD: ; 
            bip.op1 = stack[stackpointer-2].u.objRef;
            int mod1 = bigToInt();
            bip.op1 = stack[stackpointer-1].u.objRef;
            int mod2 = bigToInt();
            if((mod2 == 0))
            {
                printf("Cant use Module with 0");
                exit(99);
            }
            bigFromInt(mod1%mod2);
            stack[stackpointer-2] = PushObjectRef(bip.res);
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;
            

        case RDINT: ;
            printf("Enter Integer: ");
            bigRead(stdin);
            stack[stackpointer] = PushObjectRef(bip.res);
            stackpointer++;
            break;
    
        case WRINT:
            bip.op1 = PopObjectRef(stackpointer-1);
            bigPrint(stdout);
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;
    
        case RDCHR: ;
            printf("Enter Char: ");
            bigRead(stdin);
            stack[stackpointer] = PushObjectRef(bip.res);
            stackpointer++;
            break;
    
        case WRCHR:
            bip.op1 = PopObjectRef(stackpointer-1);
            printf("%c",bigToInt());
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;
        
        case PUSHG:
            stack[stackpointer] = PushObjectRef(variable_memory[immediate]);
            stackpointer++;
            break;
    
        case POPG:
            variable_memory[immediate] = PopObjectRef(stackpointer-1);
            stack[stackpointer-1] = PushNil();
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
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;
        
        case PUSHL:
            stack[stackpointer] = stack[framepointer+immediate];
            stackpointer++;
            break;
        
        case POPL:
            stack[framepointer+immediate] = stack[stackpointer-1];
            stackpointer--;
            break;

        case EQ:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;

            if( bigCmp() == 0 )
            {
                //stack[stackpointer-2] = PushValue(1);
                bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else
            {
                //stack[stackpointer-2] = PushValue(0);
                bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case NE:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            
            if( bigCmp() != 0 )
            {
                //stack[stackpointer-2] = PushValue(1);
                bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else
            {
                //stack[stackpointer-2] = PushValue(0);
                bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case LT:
            //-1 zurück wenn op2 größer op1 ist 
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            
            if( bigCmp() < 0 )
            {
                //stack[stackpointer-2] = PushValue(1);
                bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else
            {
               // stack[stackpointer-2] = PushValue(0);
               bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case LE:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            
            if( bigCmp() <=0 )
            {
                //stack[stackpointer-2] = PushValue(1);
                bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else
            {   
                //stack[stackpointer-2] = PushValue(0);
                bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case GT:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            
            if( bigCmp() > 0) 
            {
                //stack[stackpointer-2] = PushValue(1);
                bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else
            {
                //stack[stackpointer-2] = PushValue(0);
                bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case GE:
            bip.op1 = stack[stackpointer-2].u.objRef;
            bip.op2 = stack[stackpointer-1].u.objRef;
            
            if( bigCmp() >=0 )
            {
               // stack[stackpointer-2] = PushValue(1);
               bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else
            {
                //stack[stackpointer-2] = PushValue(0);
                bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case JMP:
            programm_counter = immediate;
            break;

        case BRF:
            bip.op1 = stack[stackpointer-1].u.objRef;
            int valuefalse = bigToInt();
            //printf("%d", value)
            //if(stack[stackpointer-1].u.number == 0) { programm_counter = immediate; }
            if(valuefalse == 0) { programm_counter = immediate; }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case BRT:
            bip.op1 = stack[stackpointer-1].u.objRef;
            int valuetrue = bigToInt();
            //if(stack[stackpointer-1].u.number == 1) { programm_counter = immediate; } 
            if(valuetrue == 1) { programm_counter = immediate; } 
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case CALL:
            stack[stackpointer] = PushValue(programm_counter);
            stackpointer++;
            programm_counter = immediate;
            break;
        
        case RET:
            programm_counter = PopValue(stackpointer-1);
            stack[stackpointer-1] = PushNil(); 
            stackpointer--;
            break;
        
        case DROP:
            for(int i=0;i < immediate;i++) 
            { 
                stack[stackpointer-1] = PushNil();
            }
            stackpointer= stackpointer - immediate;
            break;
        
        case PUSHR:
            stack[stackpointer] = PushObjectRef(return_value_register);
            stackpointer++;
            break;
        
        case POPR:
            return_value_register = PopObjectRef(stackpointer-1);
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;
        
        case DUP:
            stack[stackpointer] = stack[stackpointer-1];
            stackpointer++;
            break;

        case NEW:
            stack[stackpointer] = PushObjectRef(newCompoundObject(immediate));
            for(int i=0; i < GET_SIZE(PopObjectRef(stackpointer)); i++)
            {
                GET_REFS(PopObjectRef(stackpointer))[i] = NULL;
            } 
            stackpointer++;
            break;
        
        case GETF:
            if((!IS_PRIM(PopObjectRef(stackpointer-1))) && (immediate < GET_SIZE(PopObjectRef(stackpointer-1))))
            {
                ObjRef CmpObj = PopObjectRef(stackpointer-1);
                ObjRef GetFobjref = GET_REFS(CmpObj)[immediate];            
                stack[stackpointer-1] = PushObjectRef(GetFobjref);           
                 }
            else 
            {
                printf("Error while GETF, noob");
                exit(99);
            }
            break;

        case PUTF:
            if((!IS_PRIM(PopObjectRef(stackpointer-2))) && (immediate < GET_SIZE(PopObjectRef(stackpointer-2))))
            {
                GET_REFS(PopObjectRef(stackpointer-2))[immediate] = PopObjectRef(stackpointer-1);
                stack[stackpointer-2] = PushNil();
                stack[stackpointer-1] = PushNil();
                stackpointer=stackpointer-2;
            }
            else
            {
                printf("Error while PUTF, noob");
                exit(99);
            }
            break;

        case NEWA: ;
            //int array_size = *(int *)stack[stackpointer-1].u.objRef->data;
            bip.op1 = stack[stackpointer-1].u.objRef;
            int array_size = bigToInt();
            ObjRef objref = newCompoundObject(array_size);
            for( int i=0; i < GET_SIZE(objref);i++)
           {
               GET_REFS(objref)[i] = NULL;
            }
            stack[stackpointer-1] = PushObjectRef(objref);
            break;

        case GETFA:
            if((!IS_PRIM(PopObjectRef(stackpointer-2))) && (IS_PRIM(PopObjectRef(stackpointer-1))) )
            {
                //int index =*(int *) PopObjectRef(stackpointer-1)->data;
                bip.op1 = PopObjectRef(stackpointer-1);
                int index = bigToInt();

                ObjRef obj = GET_REFS(PopObjectRef(stackpointer-2))[index];
                stack[stackpointer-2] = PushObjectRef(obj);
                stack[stackpointer-1] = PushNil();
                stackpointer--;
            }
            break;

        case PUTFA: ;
            //int index =*(int *) PopObjectRef(stackpointer-2)->data;
            bip.op1 = PopObjectRef(stackpointer-2);
            int index = bigToInt();

            GET_REFS(PopObjectRef(stackpointer-3))[index] = PopObjectRef(stackpointer-1);
            stack[stackpointer-3] = PushNil();
            stack[stackpointer-2] = PushNil();
            stack[stackpointer-1] = PushNil();
            stackpointer = stackpointer-3;
            break;

        case GETSZ:
            if(!IS_PRIM(PopObjectRef(stackpointer-1)))
            {
                bigFromInt(GET_SIZE(PopObjectRef(stackpointer-1)));
                stack[stackpointer-1] = PushObjectRef(bip.res);
            }
            else 
            {
                printf("Warning: getsize got asked for primobject");
                stack[stackpointer-1] = PushValue(-1);
            }
            break;

        case PUSHN:
            stack[stackpointer] = PushNil();
            stackpointer++;
            break;

        case REFEQ:
            if(stack[stackpointer-1].u.objRef == stack[stackpointer-2].u.objRef)
            {
                //stack[stackpointer-2] = PushValue(1);
                bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else 
            {
                //stack[stackpointer-2] = PushValue(0);
                bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            stack[stackpointer-1] = PushNil();
            stackpointer--;
            break;

        case REFNE:
           if(stack[stackpointer-1].u.objRef == stack[stackpointer-2].u.objRef)
            {
                //stack[stackpointer-2] = PushValue(0);
                bigFromInt(0);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
            else 
            {
                //stack[stackpointer-2] = PushValue(1);
                bigFromInt(1);
                stack[stackpointer-2] = PushObjectRef(bip.res);
            }
                stack[stackpointer-1] = PushNil();
                stackpointer--;
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
            if((stack[i].isObjRef == 1) && (stack[i].u.objRef == NULL))
            {
                if(i == framepointer && i == stackpointer) 
                { printf("sp,fp-->"); printf("\t[%d]\txxx\n",i); }
                else if(i == framepointer) 
                { printf("fp-->"); printf("\t[%d]\tnil (obj)\n",i); }
                else if(i== stackpointer) { printf("sp-->\t[%d]\txxx\n",i); }
                else { printf("\t[%d]\tnil (obj)\n",i); }
            }
            else if(stack[i].isObjRef == 1)
            {
                if(i == framepointer && i == stackpointer) 
                { printf("sp,fp-->"); printf("\t[%d]\t%p (obj)\n",i,(void *)(stack[i].u.objRef)); }
                else if(i == framepointer) 
                { printf("fp-->"); printf("\t[%d]\t%p (obj)\n",i,(void *)(stack[i].u.objRef)); }
                else if(i== stackpointer) { printf("sp-->\t[%d]\txxx\n",i); }
                else { printf("\t[%d]\t%p (obj)\n",i,(void *)(stack[i].u.objRef)); }
            }
            else if(stack[i].isObjRef == 0)
            {
                if(i == framepointer && i == stackpointer) 
                { printf("sp,fp-->"); printf("\t[%d]\t%d (num)\n",i,stack[i].u.number); }
                else if(i == framepointer) 
                { printf("fp-->"); printf("\t[%d]\t%d (num)\n",i,stack[i].u.number); }
                else if(i== stackpointer) { printf("sp-->\t[%d]\txxx\n",i); }
                else { printf("\t[%d]\t%d (num)\n",i,stack[i].u.number); }
            }
            
        }
    }
    printf("--End of Stack--\n\n");
}

void print_static_data()
{
    printf("--Static data--\n");
    for(int i=0;(i < variable_memory_size);i++)
    {
        bip.op1 = variable_memory[i];
        if(bip.op1 == NULL) { printf("data[%d]:%d",i, 0); }
        else { printf("data[%d]:%d",i, bigToInt()); }
        printf("\n");
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
        printf("stack, data, object\n");
        char inspect_input_str[10];
        scanf("%s", inspect_input_str);
        if(strcmp(inspect_input_str,"stack") == 0) { print_stack(); }
        else if(strcmp(inspect_input_str,"data") == 0) { print_static_data(); }
        else if(strcmp(inspect_input_str,"object") == 0) 
        {
            printf("Enter arrayposition of object: ");
            int object_stack_adress = 0;
            scanf("%d",&object_stack_adress);
            if(stack[object_stack_adress].isObjRef == 1)
            {
            bip.op1 = stack[object_stack_adress].u.objRef;
                if(IS_PRIM(stack[object_stack_adress].u.objRef))
                {
                    printf("<Primitive Object>?\n");
                    printf("Value: %d\n",bigToInt());
                }
                else
                {
                    printf("<Compound Object>\n");
                    for(int i=0; i< GET_SIZE(stack[object_stack_adress].u.objRef); i++)
                    {
                        if(stack[object_stack_adress].u.objRef != NULL)
                        {
                            printf("field[%d]:\t%p\n", i, (void *)GET_REFS(stack[object_stack_adress].u.objRef)[i]);
                        }
                        else 
                        {
                            printf("field[%d]:\tnil\n",i);
                        }
                    }
                }
            }
            else { printf("Is not an object!\n"); }
        }
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
                printf("%d:\tdup\t\n", number);
                break; 

            case NEW:
                printf("%d:\tnew\t%d\n", number,immediate);
                break;
            
            case GETF:
                printf("%d:\tgetf\t%d\n", number,immediate);
                break;
            
            case PUTF:
                printf("%d:\tputf\t%d\n", number,immediate);
                break;
            
            case NEWA:
                printf("%d:\tnewa\t\n", number);
                break; 

            case GETFA:
                printf("%d:\tgetfa\t\n", number);
                break; 

            case PUTFA:
                printf("%d:\tputfa\t\n", number);
                break; 

            case GETSZ:
                printf("%d:\tgetsz\t\n", number);
                break; 

            case PUSHN:
                printf("%d:\tpushn\t\n", number);
                break; 

            case REFEQ:
                printf("%d:\trefeq\t\n", number);
                break; 

            case REFNE:
                printf("%d:\trefne\t\n", number);
                break; 

            default:
                printf("Unkown Opcode");
        }
}

Stackslot PushObjectRef(ObjRef objref)
{
    Stackslot stackobj;
    stackobj.isObjRef = 1;
    stackobj.u.objRef = objref;
    return stackobj;
}
Stackslot PushNil()
{
    Stackslot stackobj;
    stackobj.isObjRef =1;
    stackobj.u.objRef = NULL;
    return stackobj;
}

ObjRef PopObjectRef(int local_stackpointer)
{
    ObjRef refobj;

    if(stack[local_stackpointer].isObjRef == 1)
    {
        refobj = stack[local_stackpointer].u.objRef;
        return refobj;
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

ObjRef newPrimObject(int dataSize) 
{
  ObjRef objRef;
  objRef = malloc(sizeof(unsigned int) +
                  dataSize * sizeof(unsigned char));
  if (objRef == NULL) {
    fatalError("newPrimObject() got no memory");
  }
  objRef->size = dataSize;
  return objRef;
}

void fatalError(char *msg) {
  printf("Fatal error: %s\n", msg);
  exit(1);
}

ObjRef newCompoundObject(int numObjRefs)
{
    ObjRef objref;
    objref = malloc(sizeof(*objref)+ (numObjRefs * sizeof(void *)));
    if(objref == NULL)
    {
        fatalError("New CompoundObject() got no memory");
    }
    objref->size = numObjRefs;
    objref->size |= 1UL << (8 * sizeof(unsigned int) - 1);
    return objref;
}
