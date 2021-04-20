
/**************************************************************
*
*                     um.h
*
*     Assignment: UM -> Homework Six
*     Authors:  Wade Perry (gperry04), Tim Seifert (tseife01)
*     Date:     04/19/21
*
*     summary
*
*     The interface for the um.c file, implementation largely hidden
*     from client.
*
**************************************************************/



#include "uarray.h"
#include "seq.h"
#include <bitpack.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

/*
The structure for an instruction, derived from the 32 bit word from a .um file.
Includes an opticode that helps to call um functions. A, B, C are the registers
to the program. In the case of opticode 13, b is utilized as the value for the
32-bit word.
*/
typedef struct {
    unsigned opCode;
    unsigned a;
    unsigned b;
    unsigned c;
} instruction;

/*
execute passes in sequence that simulates main memory in order to run the .um 
file.
*/
void execute(Seq_T memory);
/*
do_instruction takes an instruction struct and an opticode as well as 
umappedSegs in order to reutilize memory and perform the proper um command.
do_instruction can result in a checked runtime error if the auxillary functions
fail.
*/
void do_instruction(instruction instruct, unsigned opticode, Seq_T m, Seq_T
unmappedSegs);
/*
newInstruction takes in a word and creates a struct instruction, returning the
value to the function call.
*/
instruction newInstuction(u_int32_t word);
