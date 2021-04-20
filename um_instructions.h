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
 *     Bit-un-packing but for .um files interface
 *
 **************************************************************/


#include <seq.h>
#include <bitpack.h>
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "uarray.h"

UArray_T Instructions_new(FILE *file);