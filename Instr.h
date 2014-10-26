/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2014                                *
 *********************************************
 */

#ifndef INSTR_H
#define INSTR_H

typedef enum { LOAD, LOADI, STORE, ADD, SUB, MUL, READ, WRITE } OpCode;

typedef struct InstructionInfo Instruction;

struct InstructionInfo {
	OpCode opcode;
	int field1;
	int field2;
	int field3;
	Instruction *prev;	/* previous instruction */
	Instruction *next;	/* next instruction */
};

#endif
