/**************************************************************
 *
 *                     um-instructions.h
 *
 *     Assignment: UM -> Homework Six
 *     Authors:  Wade Perry (gperry04), Tim Seifert (tseife01)
 *     Date:     04/19/21
 *
 *     summary
 *
 *     Bit-un-packing but for .um files implementation
 *
 **************************************************************/


#include "um_instructions.h"


UArray_T Instructions_new(FILE *file)
{
    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    int num_instructs = file_size / 4;
    /*create the UArray_T that hold instructions*/
    UArray_T instructions = UArray_new(num_instructs, sizeof(u_int32_t));
    int byte = fgetc(file);
    for(int i = 0; i < num_instructs; i++){
        u_int32_t instruction = 0;
        instruction = Bitpack_newu(instruction, 8, 24, byte);
        byte = fgetc(file);
        instruction = Bitpack_newu(instruction, 8, 16, byte);
        byte = fgetc(file);
        instruction = Bitpack_newu(instruction, 8, 8, byte);
        byte = fgetc(file);
        instruction = Bitpack_newu(instruction, 8, 0, byte);
        /*add the instruction to the instructions array*/
        *(u_int32_t *)UArray_at(instructions, i) = (u_int32_t)instruction;
        /*set up for next loop*/
        byte = fgetc(file);
    }
    return instructions;
}
