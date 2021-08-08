#ifndef NJVM_H
#define NJVM_H

#include <stdio.h>

#define VERSION 3

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
#define WRCHR 10 // a
#define PUSHG 11 // b
#define POPG 12 // c
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define EQ 17
#define NE 18 // 12
#define LT 19
#define LE 20
#define GT 21 // 15
#define GE 22
#define JMP 23 // 17
#define BRF 24 // 18
#define BRT 25

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))

void execute_instruction(unsigned int instruction);

void print_programm();

void print_stack();

void load_programm_from_File(char *programm_path);

void print_instruction(int counter,unsigned int instruction);

_Bool debug_menu();

void print_static_data();

#endif