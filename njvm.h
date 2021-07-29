#ifndef NJVM_H
#define NJVM_H

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
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))

void load_programm(char *programm_ident);

void execute_instruction(unsigned int instruction);

void print_programm();

void print_stack();

#endif