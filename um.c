
/**************************************************************
*
*                     um.c
*
*     Assignment: UM -> Homework Six
*     Authors:  Wade Perry (gperry04), Tim Seifert (tseife01)
*     Date:     04/19/21
*
*     summary
*
*     A simulated universal machine which has its own programming
*     language... the language of ummmmmmm, wait I forgot. 
*
**************************************************************/


#include "um.h"

#define twoto32 4294967296

u_int32_t registers[8];

int program_counter, program_length;

void Conditional_move(instruction instruct);
void Segmented_load(instruction instruct, Seq_T memory);
void Segmented_Store(instruction instruct, Seq_T memory);
void Addition(instruction instruct);
void Multiplication(instruction instruct);
void Division(instruction instruct);
void Bitwise_NAND(instruction instruct);
void Halt(Seq_T memory, Seq_T unmappedSegs);
void Map_Segment(instruction instruct, Seq_T memory, Seq_T unmappedSegs);
void Unmap_Segment(instruction instruct, Seq_T unmappedSegs);
void Output(instruction instruct);
void Input(instruction instruct);
void Load_Program(instruction instruct, Seq_T memory);
void Load_Value(instruction instruct);


/* 
* execute()
* purpose: the main handler for the universal machine 
*          loops through instructions untill halt or end of input
* parameters: takes in memory as a Seq_T
* return type: void
* highlighting error conditions:
*/
void execute(Seq_T memory)
{
    /* Sequence to keep track of what segments are unmapped */
    Seq_T unmappedSegs = Seq_new(20);
    program_counter = 0;
    program_length = UArray_length((UArray_T)Seq_get(memory, 0));
    instruction instruct;
    while (program_counter < program_length) {
        /* get instruction */
        instruct = newInstuction(*(u_int32_t *)
            UArray_at((UArray_T)Seq_get(memory, 0), program_counter));
        program_counter++;
        /* switch statements */
        do_instruction(instruct, instruct.opCode, memory, unmappedSegs);
    }

}

/* 
* do_instruction()
* purpose: do instruction takes an insutction struct and performs the 
* corresponding function based on the opticode.
* parameters: instruction instruct, unsigned opticode, and seq_T memory, and
*  Seq_T unmappedSegs
* return type: void
* highlighting error conditions: if an opitcode case occurs out of the scope
*  of the switch statement
*/
void do_instruction(instruction instruct, unsigned opticode, 
                            Seq_T memory, Seq_T unmappedSegs)
{
    switch (opticode) {
        case 0  :
            Conditional_move(instruct);
            break; 
        case 1  :
            Segmented_load(instruct, memory);
            break;  
        case 2  :
            Segmented_Store(instruct, memory);
            break;
        case 3 :
            Addition(instruct);
            break;
        case 4 :
            Multiplication(instruct);
            break;
        case 5 :
            Division(instruct);
            break;
        case 6 :
            Bitwise_NAND(instruct);
            break;
        case 7 :
            Halt(memory, unmappedSegs);
            break;
        case 8 :
            Map_Segment(instruct, memory, unmappedSegs);
            break;
        case 9 :
            Unmap_Segment(instruct, unmappedSegs);
            break;
        case 10 :
            Output(instruct);
            break;
        case 11 :
            Input(instruct);
            break;
        case 12 :
            Load_Program(instruct, memory);
            break;
        case 13 :
            Load_Value(instruct);
            break;
        default :
            exit(EXIT_FAILURE);
    }
}

/* 
* newInsturction()
* purpose: do instruction takes an insutction struct and performs the 
* corresponding function based on the opticode.
* parameters: instruction instruct, unsigned opticode, and seq_T memory, and
*  Seq_T unmappedSegs
* return type: void
* highlighting error conditions: if an opitcode case occurs out of the scope
*  of the switch statement
*/
instruction newInstuction(u_int32_t word)
{
    /* unpack the word */
    unsigned opticode = Bitpack_getu(word, 4, 28);
    unsigned a = 0;
    unsigned b = 0;
    unsigned c = 0;
    /*special case*/
    if (opticode == 13) {
        a = Bitpack_getu(word, 3, 25);
        b = Bitpack_getu(word, 25, 0);
    }
    else { /*normal way*/
        c = Bitpack_getu(word, 3, 0);
        b = Bitpack_getu(word, 3, 3);
        a = Bitpack_getu(word, 3, 6);
    }
    instruction instuct = {opticode, a, b, c};
    return instuct;
}

/* 
* Conditional_move()
* purpose: based on the value in regisister c, if this vlaue is not zero,
* move register b's value into register's a..
* parameters: instruction instruct holds the current instructions data 
* (opticode, a, b, and c)
* return type: void
* highlighting error conditions: 
*/
void Conditional_move(instruction instruct){
    if (registers[instruct.c] != 0) {
        registers[instruct.a] = registers[instruct.b];
    }
}
/* 
* Conditional_move()
* purpose: Segmented load takes the 32 bit word at memory segment 
*           register B at index register C and stores it in register A
*           i.e ( $r[A] := $m[$r[B]][$r[C]] )
* parameters: instruction instruct holds the current instructions data 
*           (opticode, a, b, and c) and Seq_T memory, which simulates 
*           main memory
* return type: void
* highlighting error conditions: 
*/
void Segmented_load(instruction instruct, Seq_T memory)
{
    assert(registers[instruct.b] < (unsigned)Seq_length(memory));
    /* get the segment at $m[$r[B]] */
    UArray_T segment = 
        (UArray_T)Seq_get(memory, (unsigned)registers[instruct.b]);
    assert(registers[instruct.c] < (unsigned)UArray_length(segment));
    /* get the word at $m[$r[B]][$r[C]] */
    uint32_t word = 
        *(uint32_t *)UArray_at(segment, (unsigned)registers[instruct.c]);
    registers[instruct.a] = word;
}

void Segmented_Store(instruction instruct, Seq_T memory){
    UArray_T segment = 
        (UArray_T)Seq_get(memory, (unsigned)registers[instruct.a]);
    *(uint32_t *)UArray_at(segment, (unsigned)registers[instruct.b]) = 
        (unsigned)registers[instruct.c];
}

/* 
* Addition()
* purpose: Add the values stored at register B and C together 
*           and store them in register A (mod 2^32)
*           ( $r[A] := ($r[B] + $r[C]) mod 2^32 )
* parameters: instruction instruct holds the current instructions data 
*           (opticode, a, b, and c)
* return type: void
* highlighting error conditions: 
*/
void Addition(instruction instruct){
    registers[instruct.a] =
        (registers[instruct.b] + registers[instruct.c]) % twoto32;
}

/* 
* Multiplication()
* purpose: Multiplys the values stored at register B and C together 
*           and store them in register A (mod 2^32)
*           ( $r[A] := ($r[B] * $r[C]) mod 2^32 )
* parameters: instruction instruct holds the current instructions data
*           (opticode, a, b, and c)
* return type: void
* highlighting error conditions: 
*/
void Multiplication(instruction instruct){
    registers[instruct.a] = 
        (registers[instruct.b] * registers[instruct.c]) % twoto32;
}

/* 
* Division()
* purpose:  Divides the values stored at register B and C together 
*           and store them in register A
*           ( $r[A] := ($r[B] / $r[C]) )
* parameters:   instruction instruct holds the current instructions data 
*               (opticode, a, b, and c)
* return type: void
* highlighting error conditions: 
*/
void Division(instruction instruct){
    assert(registers[instruct.c] != 0);
    registers[instruct.a] = 
        (registers[instruct.b] / registers[instruct.c]);
}

/* 
* Bitwise_NAND()
* purpose: Bitwise_NAND -> Nots the bitwise and of the values stored in 
register B and register C.
*          $r[A] := ¬($r[B] ∧ $r[C])
* parameters: instruction instruct holds the current instructions data 
            (opticode, a, b, and c) and Seq_T memory
* return type: void
* highlighting error conditions: 
*/
void Bitwise_NAND(instruction instruct){
    registers[instruct.a] = 
        ~((unsigned)registers[instruct.b] & (unsigned)registers[instruct.c]);
}
/* 
* Halt()
* purpose: Computation stops.
*          Stops the execution of any further lines of um instructions
* parameters: Takes both Seq_T memory and Seq_T unmappedSegs to free memory 
associated with both our simulated main memory and the seperate reused 
unmappedSegs
* return type: void
* highlighting error conditions: 
*/
void Halt(Seq_T memory, Seq_T unmappedSegs)
{
    int length = Seq_length(memory);
    for(int i = 0; i < length; i++){
        UArray_T segment = (UArray_T)Seq_remlo(memory);
        if (segment != NULL) {
            UArray_free(& segment);
        }
    }
    length = Seq_length(unmappedSegs);
    for(int i = 0; i < length; i++){
        unsigned *index = (unsigned *)Seq_remlo(unmappedSegs);
        free(index);
    }
    Seq_free(&memory);
    Seq_free(&unmappedSegs);
    exit(EXIT_SUCCESS);
}

/* 
* Map_Segment() 
* purpose:
Modified from: https://www.cs.tufts.edu/comp/40/assignments/hw06/um.pdf
A new segment is created with a number of words equal to the value in register
$r[C]. Each word in the new segment is initialized to 0. A new segment is
mapped as $m[$r[B]].
* parameters: Takes both Seq_T memory and Seq_T unmappedSegs in order to add 
the new mapped segment as well the instruction itself instruct.
* return type: void
* highlighting error conditions: 
*/
void Map_Segment(instruction instruct, Seq_T memory, Seq_T unmappedSegs){
    /*create new UArray_T of size $r[C]*/
    UArray_T segment = UArray_new(registers[instruct.c], sizeof(uint32_t));
    for(int i = 0; i < (int)registers[instruct.c]; i++){
        *(uint32_t *)UArray_at(segment, i) = (uint32_t)0;
    }
    /*check to see if there are any currently unmapped segments*/
    if (Seq_length(unmappedSegs) > 0) {
        unsigned *index = (unsigned *)Seq_remlo(unmappedSegs);
        UArray_T oldseg = Seq_put(memory, *index, segment);
        UArray_free(&oldseg);
        registers[instruct.b] = *index;
        free(index);
    }
    else { /*if no unmapped segments add a new index in memory*/
        Seq_addhi(memory, segment);
        registers[instruct.b] = Seq_length(memory) - 1;
    }
}

/* 
* Unmap_Segment()
* purpose:
Modified from: https://www.cs.tufts.edu/comp/40/assignments/hw06/um.pdf
The segment $m[$r[C]] is unmapped. Future Map Segment instructions may reuse the
identifier $r[C].
* parameters: Takes both Seq_T memory and Seq_T unmappedSegs in order to add 
the new mapped segment as well the instruction itself instruct.
* return type: void
* highlighting error conditions: 
*/
void Unmap_Segment(instruction instruct, Seq_T unmappedSegs)
{
    /*add the index to the unmappedSegs Seq so it can be overwritten later*/
    unsigned *index = malloc(sizeof(unsigned));
    *index = registers[instruct.c];
    assert(*index != 0);
    Seq_addhi(unmappedSegs, index);
}
/* 
* Output()
* purpose:
Modified from: https://www.cs.tufts.edu/comp/40/assignments/hw06/um.pdf
The value in register $r[C] is written to the I/O device immediately. 
* parameters: Takes an instruction, instruct.
* return type: void
* highlighting error conditions: Only values from 0 to 255 are allowed.
*/
void Output(instruction instruct)
{
    assert(registers[instruct.c] < 256);
    printf("%c", registers[instruct.c]);
}
/* 
* Input()
* purpose:
Modified from: https://www.cs.tufts.edu/comp/40/assignments/hw06/um.pdf
The universal machine waits for input on the
I/O device. When input arrives, $r[C] is loaded with the input.
* parameters: Takes an instruction, instruct.
* return type: void
* highlighting error conditions: Only values from 0 to 255 are allowed. If the 
* end of input has been signaled register c is loaded with all and all 1 
* 32-bit word.
*/
void Input(instruction instruct)
{
    int input = getchar();
    if ( input < 256 && input > -1) {
        registers[instruct.c] = input;
    }
    if ( input == EOF) {
        registers[instruct.c] = (u_int32_t) ~0;
    }
}
/* 
* Load_program()
* purpose:
Modified from: https://www.cs.tufts.edu/comp/40/assignments/hw06/um.pdf
The universal machine waits for input on the
I/O device. When input arrives, $r[C] is loaded with the input.
* parameters Takes an instruction, instruct and simulated main memory Seq_T 
* memory. 
* return type: void
* highlighting error conditions: Only values from 0 to 255 are allowed. If the 
* end of input has been signaled register c is loaded with all and all 1 
* 32-bit word.
*/
void Load_Program(instruction instruct, Seq_T memory)
{
    if ((unsigned)registers[instruct.b] != 0) {
        assert((unsigned)registers[instruct.b] < (unsigned)Seq_length(memory));
        UArray_T newInstruct = 
            (UArray_T)Seq_get(memory, (unsigned)registers[instruct.b]);
        UArray_T oldInstruct = (UArray_T)Seq_remlo(memory);
        UArray_free(&oldInstruct);
        Seq_addlo(memory, newInstruct);
        program_length = UArray_length(newInstruct);
    }
    program_counter = (unsigned)registers[instruct.c];
}
/* 
* Load_Value()
* purpose:
Modified from: https://www.cs.tufts.edu/comp/40/assignments/hw06/um.pdf
A special function that takes the instruction in and replaces register a with
the value stored at the remaining 25 bits of the word.
* parameters: Takes an instruction, instruct: an unpacked 32 bit word.
* return type: void
* highlighting error conditions: 
*/
void Load_Value(instruction instruct)
{
    registers[instruct.a] = instruct.b;
}
