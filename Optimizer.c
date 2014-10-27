/*
 *********************************************
 *  314 Principles of Programming Languages  *
 *  Fall 2014                                *
 *********************************************
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "InstrUtils.h"
#include "Utils.h"

#define EMPTY_FIELD 0xFFFFF

void delete_instruction(Instruction *instr)
{
	if (instr == NULL) return;

	/* set prev's next to instr's next */
	if (instr->prev != NULL) {
		instr->prev->next = instr->next;
	}

	/* set next's prev to instr's prev */
	if (instr->next != NULL) {
		instr->next->prev = instr->prev;
	}

	/* deallocate */
	free(instr);
}

/*
 * Given the last of three Instructions, folds them into one by deleting the
 * first two and changing the attributes of the third. Note that field1 is not
 * a parameter (because that register should never change).
 */
void fold_instruction(Instruction *instr, OpCode opcode, int field2, int field3)
{
	/* should not happen */
	if (instr == NULL) return;

	/* delete two previous instructions */
	delete_instruction(instr->prev);
	delete_instruction(instr->prev);

	/* change attributes */
	instr->opcode = opcode;
	instr->field2 = field2;
	instr->field3 = field3;
}

/*
 * If the pattern represents (<id> <op> <c>) or (<c> <op> <id>), returns 1 and
 * sets *id equal to <id>, *opcode equal to <op>, and *c equal to <c>. Also
 * sets *var_first equal to 1 if the pattern has <id> first, and 0 otherwise
 * (this is useful for analyzing patterns with a non-commutative operation).
 *
 * If the pattern represents (<id> <op> <id>), returns 2 and sets *id equal to
 * <id> and *opcode equal to <op>.
 *
 * If no pattern is detected, this function returns 0 and does not modify the
 * parameters.
 *
 * Does not consider patterns where <op> is not one of { ADD, SUB, MUL } .
 */
int scan_pattern(Instruction *instr1, Instruction *instr2, Instruction *instr3,
                 int *id, OpCode *opcode, int *c, int *var_first)
{
	if (instr3->opcode != ADD
		&& instr3->opcode != SUB
		&& instr3->opcode != MUL)
	{
		return 0;
	}

	/* LOAD, LOADI, op ? */
	if (instr1->opcode == LOAD
		&& instr2->opcode == LOADI
		&& instr1->field1 == instr3->field2
		&& instr2->field1 == instr3->field3)
	{
		*id = instr1->field2;
		*opcode = instr3->opcode;
		*c = instr2->field2;
		*var_first = 1;
		return 1;
	}

	/* LOADI, LOAD, op ? */
	if (instr1->opcode == LOADI
		&& instr2->opcode == LOAD
		&& instr1->field1 == instr3->field2
		&& instr2->field1 == instr3->field3)
	{
		*id = instr2->field2;
		*opcode = instr3->opcode;
		*c = instr1->field2;
		*var_first = 0;
		return 1;
	}

	/* LOAD <id>, LOAD <id>, op ? (where both ids are the same) */
	if (instr1->opcode == LOAD
		&& instr2->opcode == LOAD
		&& instr1->field2 == instr2->field2
		&& instr1->field1 == instr3->field2
		&& instr2->field1 == instr3->field3)
	{
		*id = instr1->field2;
		*opcode = instr3->opcode;
		return 2;
	}

	return 0;
}

int main()
{
	Instruction *head;
	Instruction *instr1, *instr2, *instr3;
	/* int opt_flag, opt_calc; */

	head = ReadInstructionList(stdin);

    /* ---  FIRST: ALGEBRAIC SIMPLIFICATION PASS --- */

	if (!head) {
		ERROR("No instructions\n");
		exit(EXIT_FAILURE);
	}

	instr1 = head;

	while (instr1 != NULL
		   && (instr2 = instr1->next) != NULL
		   && (instr3 = instr2->next) != NULL)
	{
		int id, c, var_first;   /* to store result of scan_pattern */
		OpCode opcode;

		int result = scan_pattern(instr1, instr2, instr3, &id, &opcode, &c, &var_first);

		if (!result) {
			instr1 = instr2;    /* advance window by one instruction */
			continue;
		}

		/* (<id> - <id>) ? */
		if (result == 2) {
			if (opcode == SUB){
				/* yes - replace with LOADI 0 */
				fold_instruction(instr3, LOADI, 0, EMPTY_FIELD);
				if (head == instr1) head = instr3;   /* move head if necessary */
				instr1 = instr3->next;               /* advance window past fold */
				continue;
			} else {
				/* no, and definitely not any other simplifiable pattern */
				instr1 = instr2;    /* advance window by one instruction */
				continue;
			}
		}

		/* (result is definitely 1 at this point) */

		/* other zero? i.e. (0 * a) or (a * 0) ? */
		if (opcode == MUL && c == 0) {
			/* replace with LOADI 0 */
			fold_instruction(instr3, LOADI, 0, EMPTY_FIELD);
			if (head == instr1) head = instr3;   /* move head if necessary */
			instr1 = instr3->next;               /* advance window past fold */
			continue;
		}

		/* identity? i.e. (<id> + 0) or (<id> * 1) ? */
		if ((opcode == ADD && c == 0)
			|| (opcode == MUL && c == 1)
			|| (opcode == SUB && c == 0 && var_first))
		{
			/* replace with LOAD <id> */
			fold_instruction(instr3, LOAD, id, EMPTY_FIELD);
			if (head == instr1) head = instr3;   /* move head if necessary */
			instr1 = instr3->next;               /* advance window past fold */
			continue;
		}

		/* no algebraically simplifiable patterns */
		instr1 = instr2;    /* advance window by one instruction */
	}


    /* --- SECOND: CONSTANT FOLDING PASS --- */

	if (!head) {
		ERROR("No instructions\n");
		exit(EXIT_FAILURE);
	}

	/* YOUR CODE GOES HERE */


	PrintInstructionList(stdout, head);
	DestroyInstructionList(head);
	return EXIT_SUCCESS;
}
