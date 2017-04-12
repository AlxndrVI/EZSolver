#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "GlobalVars.h"
#include "EZSolvers.h"

int EZSolver2(int m[mxmy][mxmy], int solution[mxmy][mxmy])
{
	int nm[mxmymxmy];

	int(*src) = (int(*))m;
	int(*sol) = (int(*))solution;
	int solved = 0;

	int queuelenght = 0;
	int queue[mxmymxmy];

	int blockSolved[mxmy * 3];
	int blockOneOrMore[mxmy * 3];
	int blockOnlyOne[2][mxmy * 3];

	for (int b = 0; b < mxmy * 3; ++b)
	{
		blockSolved[b] = 0;
		blockOnlyOne[0][b] = mflags;
		blockOnlyOne[1][b] = mflags;
		blockOneOrMore[b] = mflags;
	}

	for (int o = 0; o < mxmymxmy; ++o)
	{
		int c = src[o];

		if (c)
		{
			c = ((1 << c) >> 1);
			nm[o] = c;

			blockSolved[offset2row[o]] |= c;
			blockSolved[offset2column[o]] |= c;
			blockSolved[offset2block[o]] |= c;
		}
		else
		{
#ifdef DEBUG
			nm[o] = c; //0
#endif // DEBUG

			queue[queuelenght] = o;
			++queuelenght;
		}
	}

	int ql = 0;
	int curr = 0;
	int next = 1;

	while (queuelenght && (ql != queuelenght))
	{

#ifdef DEBUG
		showboard(0, 0, nm);
		std::cout << "qld " << ql << " " << queuelenght << std::endl;
#endif // DEBUG

		ql = queuelenght;
		queuelenght = 0;

		for (int b = 0; b < mxmy * 3; ++b)
		{
			blockOnlyOne[next][b] = mflags;
			blockOneOrMore[b] = mflags;
		}

		for (int q = 0; q < ql; ++q)
		{
			int ic = blockSolved[offset2row[queue[q]]] | blockSolved[offset2column[queue[q]]] | blockSolved[offset2block[queue[q]]];
			int c = ic^mflags;
			int d = c & (blockOnlyOne[curr][offset2row[queue[q]]] | blockOnlyOne[curr][offset2column[queue[q]]] | blockOnlyOne[curr][offset2block[queue[q]]]);

			if (bitcountarray[d] == 1)
			{
				nm[queue[q]] = d;
				blockSolved[offset2row[queue[q]]] |= d;
				blockSolved[offset2column[queue[q]]] |= d;
				blockSolved[offset2block[queue[q]]] |= d;
			}
			else if (bitcountarray[c] == 1) //если 1 вариант
			{
				nm[queue[q]] = c;

				blockSolved[offset2row[queue[q]]] |= c;
				blockSolved[offset2column[queue[q]]] |= c;
				blockSolved[offset2block[queue[q]]] |= c;
			}
			else //если несколько вариантов
			{
#ifdef DEBUG
				nm[queue[q]] = c;
#endif // DEBUG

				blockOnlyOne[next][offset2row[queue[q]]] &= ic | blockOneOrMore[offset2row[queue[q]]];
				blockOnlyOne[next][offset2column[queue[q]]] &= ic | blockOneOrMore[offset2column[queue[q]]];
				blockOnlyOne[next][offset2block[queue[q]]] &= ic | blockOneOrMore[offset2block[queue[q]]];

				blockOneOrMore[offset2row[queue[q]]] &= ic;
				blockOneOrMore[offset2column[queue[q]]] &= ic;
				blockOneOrMore[offset2block[queue[q]]] &= ic;

				queue[queuelenght] = queue[q];
				++queuelenght;
			}
		}

		// swap curr vs next
		int temp = curr;
		curr = next;
		next = temp;
	}

#ifdef DEBUG
	showboard(0, 0, nm);
	std::cout << "qld " << ql << " " << queuelenght << std::endl;

	for (int b = 0; b < mxmy * 3; ++b)
	{
		std::cout << b << "\tSolved\t" << std::bitset<9>(blockSolved[b]) << "\tOnlyOne_0\t" << std::bitset<9>(blockOnlyOne[0][b]) << "\tOnlyOne_1\t" << std::bitset<9>(blockOnlyOne[1][b]) << std::endl;
	}
#endif // DEBUG

	for (int *t = &solution[0][0], *s = &nm[0];
		t < &solution[0][0] + mxmymxmy;
		++t, ++s)
	{
		*t = mask2digit[*s];
	}

	return !queuelenght;
}

