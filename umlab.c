/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction instruction = 0;
        instruction = (Um_instruction)Bitpack_newu(instruction, 4, 28, op);
        instruction = (Um_instruction)Bitpack_newu(instruction, 3, 6, ra);
        instruction = (Um_instruction)Bitpack_newu(instruction, 3, 3, rb);
        return (Um_instruction)Bitpack_newu(instruction, 3, 0, rc);
}


Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction instruction = 0;
        instruction = (Um_instruction)Bitpack_newu(instruction, 4, 28, LV);
        instruction = (Um_instruction)Bitpack_newu(instruction, 3, 25, ra);
        return (Um_instruction)Bitpack_newu(instruction, 15, 0, val);
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction multiplication(Um_register a, Um_register b,
 Um_register c) 
{
        return three_register(MUL, a, b, c);
}
static inline Um_instruction conditionalmove(Um_register a, Um_register b,
Um_register c) 
{
        return three_register(CMOV, a, b, c);
}

static inline Um_instruction division(Um_register a, Um_register b, 
Um_register c) 
{
        return three_register(DIV, a, b, c);
}

Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

Um_instruction input(Um_register c)
{
        return three_register(IN, 0, 0, c);
}

Um_instruction bitnand(Um_register a, Um_register b, Um_register c)
{
        return three_register(NAND, a, b, c);
}

Um_instruction map(Um_register b, Um_register c)
{
        return three_register(ACTIVATE, 0, b, c);
}

Um_instruction unmap(Um_register c)
{
        return three_register(INACTIVATE, 0, 0, c);
}
Um_instruction s_load(Um_register a, Um_register b, Um_register c)
{
        return three_register(SLOAD, a, b, c);
}

Um_instruction s_store(Um_register a, Um_register b, Um_register c)
{
        return three_register(SSTORE, a, b, c);
}



/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void addition_test(Seq_T stream)
{
        append(stream, loadval(r2, 65));
        append(stream, loadval(r3, 28));
        append(stream, add(r1, r2, r3));
        append(stream, output(r1));
        append(stream, halt());
}

void last_test(Seq_T stream)
{
        append(stream, input(r1));
        append(stream, output(r1));
        append(stream, halt()); 
}

void multiplication_test(Seq_T stream)
{
        append(stream, loadval(r2, 34));
        append(stream, loadval(r3, 2));
        append(stream, multiplication(r1, r2, r3));
        append(stream, output(r1));
        append(stream, halt());
}

void Division_test(Seq_T stream)
{
        append(stream, loadval(r2, 128));
        append(stream, loadval(r3, 2));
        append(stream, division(r1, r2, r3));
        append(stream, output(r1));
        append(stream, halt());
}

void bit_nand_test(Seq_T stream)
{
        append(stream, loadval(r2, 34));
        append(stream, loadval(r3, 108));
        append(stream, bitnand(r1, r2, r3));
        append(stream, halt());
}
void cond_move(Seq_T stream)
{
        append(stream, loadval(r2, 34));
        append(stream, loadval(r3, 108));
        append(stream, conditionalmove(r3, r2, r1));
        append(stream, output(r1));
        append(stream, output(r3));
        append(stream, halt());
}

void mapnunmapTest(Seq_T stream)
{
        append(stream, loadval(r5, 10));
        append(stream, loadval(r6, 45));
        append(stream, loadval(r7, 48));


        append(stream, map(r2, r5));

        append(stream, add(r2, r2, r7));

        append(stream, output(r2)); 
        append(stream, output(r6));


        append(stream, map(r3, r5));
        append(stream, add(r4, r3, r7));
        append(stream, output(r4));
        append(stream, output(r6));

        append(stream, unmap(r3));

        append(stream, map(r1, r5));
        append(stream, add(r1, r1, r7));
        append(stream, output(r1));
        append(stream, output(r6));

        append(stream, halt());
}

void segmented_load_unload(Seq_T stream)
{
        append(stream, loadval(r5, 10));
        append(stream, loadval(r6, 45));
        append(stream, loadval(r7, 48));

        append(stream, loadval(r1, 1));

        append(stream, map(r2, r5));
        
        append(stream, s_store(r2, r1, r6));

        append(stream, s_load(r4, r2, r1));

        append(stream, output(r4));

        append(stream, halt());
}


