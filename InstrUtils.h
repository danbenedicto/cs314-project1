/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2014                                *
 *********************************************
 */

#include "Instr.h"
#include <stdio.h>

#ifndef INSTR_UTILS_H
#define INSTR_UTILS_H

void PrintInstruction(FILE * outfile, Instruction * instr);
void PrintInstructionList(FILE * outfile, Instruction * instr);
Instruction *ReadInstruction(FILE * infile);
Instruction *ReadInstructionList(FILE * infile);
Instruction *LastInstruction(Instruction * instr);
void DestroyInstructionList(Instruction * instr);

#endif
