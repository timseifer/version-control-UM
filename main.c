/**************************************************************
*
*                         main.c
*
*     Assignment: UM -> Homework Six
*     Authors:  Wade Perry (gperry04), Tim Seifert (tseife01)
*     Date:     04/19/21
*
*     summary
*
*     Main program handler for the universal machine
*
**************************************************************/ 

#include <stdlib.h>
#include <stdbool.h>
#include "um_instructions.h"
#include<stdio.h>
#include "um.h"
#include <math.h>

int main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(stderr, "UM takes one argument\n");
        exit(EXIT_FAILURE);
    }
    FILE* fp = fopen(argv[1], "r" );
    assert(fp != NULL);
    /* create an Array of the 32bit instuctions*/
    UArray_T instructions = Instructions_new(fp);
    fclose(fp);
    /* set up memory and add the instuctions at index 0 */
    Seq_T memory = Seq_new(100);
    Seq_addhi(memory, instructions);
    /* run the universal machine */
    execute(memory);
}