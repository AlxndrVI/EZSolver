#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "GlobalVars.h"
#include "EZSolvers.h"

//int static offset2row[mxmymxmy];
//int static offset2column[mxmymxmy];
//int static offset2block[mxmymxmy];
//int static cons[mxmymxmy][consblocksize];

void showboard(int x, int y, int m[mxmymxmy])
{
	for (int row = 0; row < mxmy; ++row)
	{
		if (row % my == 0) std::cout << std::endl;
		for (int line = 0; line < mx; ++line)
		{
			for (int block = 0; block < my; ++block)
			{
				std::cout << " ";
				for (int digit = block * mx; digit < block * mx + mx; ++digit)
				{
					if ((x == digit) & (y == row)) std::cout << " >";
					else std::cout << " |";
					for (int bit = line * my; bit < line * my + my; ++bit)
					{
						if (m[row*mxmy + digit] & (1 << bit))
							std::cout << (bit + 1);
						else
							std::cout << " ";
					}
					if ((x == digit) & (y == row)) std::cout << "< ";
					else std::cout << "| ";
				}
				std::cout << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}


//int CheckSingles(int board[mxmymxmy], int o)
//{
//	int solved = 0;
//	int comparemask = 0, singlemask = 0, activemask = 0;
//
//	for (int i = 0; i < mxmy; ++i)
//	{
//		int offset = (o / mxmy)*mxmy + i;
//		activemask = activemask | board[offset] & comparemask;
//		singlemask = (board[offset] ^ comparemask) &~activemask;
//		comparemask = comparemask | board[offset]; //сохраняем все биты
//	}
//
//	for (int i = 0; i < mxmy; ++i)
//	{
//		int offset = (o / mxmy)*mxmy + i;
//		int clearmask = board[offset] & singlemask;
//		if ((clearmask) && (clearmask != board[offset]))
//		{
//			board[offset] = clearmask;
//			solved += 1 + EraseMark(board, offset);
//		}
//	}
//
//	comparemask = 0, singlemask = 0, activemask = 0;
//
//	for (int i = 0; i < mxmy; ++i)
//	{
//		int offset = i*mxmy + o%mxmy;
//		activemask = activemask | board[offset] & comparemask;
//		singlemask = (board[offset] ^ comparemask) &~activemask;
//		comparemask = comparemask | board[offset]; //сохраняем все биты
//	}
//
//	for (int i = 0; i < mxmy; ++i)
//	{
//		int offset = i*mxmy + o%mxmy;
//		int clearmask = board[offset] & singlemask;
//		if ((clearmask) && (clearmask != board[offset]))
//		{
//			board[offset] = clearmask;
//			solved += 1 + EraseMark(board, offset);
//		}
//	}
//
//	comparemask = 0, singlemask = 0, activemask = 0;
//
//	for (int i = 0; i < mxmy; ++i)
//	{
//		int offset = ((o / mxmy) / my) * my * mxmy + ((o % mxmy) / mx) * mx + i % mx + (i / mx) * mxmy;
//		activemask = activemask | board[offset] & comparemask;
//		singlemask = (board[offset] ^ comparemask) &~activemask;
//		comparemask = comparemask | board[offset]; //сохраняем все биты
//	}
//
//	for (int i = 0; i < mxmy; ++i)
//	{
//		int offset = ((o / mxmy) / my) * my * mxmy + ((o % mxmy) / mx) * mx + i % mx + (i / mx) * mxmy;
//		int clearmask = board[offset] & singlemask;
//		if ((clearmask) && (clearmask != board[offset]))
//		{
//			board[offset] = clearmask;
//			solved += 1 + EraseMark(board, offset);
//		}
//	}
//	return solved;
//}


int MarkHiddenSingles(int board[mxmymxmy])
{
	int solved = 0;
	for (int j = 0; j < mxmy; ++j)
	{
		int comparemask = 0, singlemask = 0;

		for (int i = 0; i < mxmy; ++i)
		{
			int offset = j*mxmy + i;
			int val = board[offset];
			singlemask |= val & comparemask;
			comparemask |= val; //сохраняем все биты
		}


		//singlemask = mflags &~singlemask;
		singlemask ^= mflags;

		for (int i = 0; i < mxmy; ++i)
		{
			int offset = j*mxmy + i;
			int clearmask = board[offset] & singlemask;
			if (clearmask && clearmask != board[offset])
			{
				board[offset] = clearmask;
				solved += 1 + EraseMark(board, offset, clearmask);
			}
		}
	}


	for (int j = 0; j < mxmy; ++j)
	{
		int comparemask = 0, singlemask = 0;

		for (int i = 0; i < mxmy; ++i)
		{
			int offset = i*mxmy + j;
			int val = board[offset];
			singlemask |= val & comparemask;
			comparemask |= val; //сохраняем все биты
		}

		//singlemask = mflags &~singlemask;
		singlemask ^= mflags;

		for (int i = 0; i < mxmy; ++i)
		{
			int offset = i*mxmy + j;
			int clearmask = board[offset] & singlemask;
			if (clearmask && clearmask != board[offset])
			{
				board[offset] = clearmask;
				solved += 1 + EraseMark(board, offset, clearmask);
			}
		}
	}


	for (int j = 0; j < mxmy; ++j)
	{
		int comparemask = 0, singlemask = 0;

		for (int i = 0; i < mxmy; ++i)
		{
			//int offset = my * mxmy * (j / my) + mx * (j % my) + i % mx + (i / mx) * mxmy;
			int offset = block2offset[j + 2 * mxmy][i];
			int val = board[offset];
			singlemask |= val & comparemask;
			comparemask |= val; //сохраняем все биты
		}

		//singlemask = mflags &~singlemask;
		singlemask ^= mflags;

		for (int i = 0; i < mxmy; ++i)
		{
			//int offset = my * mxmy * (j / my) + mx * (j % my) + i % mx + (i / mx) * mxmy;
			int offset = block2offset[j + 2 * mxmy][i];
			int clearmask = board[offset] & singlemask;
			if (clearmask && clearmask != board[offset])
			{
				board[offset] = clearmask;
				solved += 1 + EraseMark(board, offset, clearmask);
			}
		}
	}
	return solved;
}

int EraseMark(int board[mxmymxmy], int offset, int val)
{
	int solved = 0;
	//int val = board[offset];
	//val = ~val&mflags;
	for (int i = 0; i < consblocksize; ++i)
	{
		//int found = (board[cons[offset][i]] & (~val))&mflags;
		int found = board[cons[offset][i]] & (val ^ mflags); //xor во время цикла оказывается быстрее.....
		//int found = board[cons[offset][i]] & val;
		if (found != board[cons[offset][i]]) // renew variants
		{
			board[cons[offset][i]] = found;
			if (bitcountarray[found] == 1) // open single
			{
				//CheckSingles(board, offset);
				solved += 1 + EraseMark(board, cons[offset][i], found);
			}
		}
	}
	return solved;
}

int EZSolver(int m[mxmy][mxmy], int solution[mxmy][mxmy])
{
	int blockmasks[mxmy * 3];

	int nm[mxmymxmy];

	int(*src) = (int(*))m;
	int(*sol) = (int(*))solution;
	int solved = 0;


	for (int j = 0; j < mxmy; ++j)
	{
		blockmasks[j] = 0;
		for (int i = 0; i < mxmy; ++i)
		{
			blockmasks[j] |= ((1 << m[j][i]) >> 1); //Bit Magic FTW
		}

		blockmasks[j + mxmy] = 0;
		for (int i = 0; i < mxmy; ++i)
		{
			blockmasks[j + mxmy] |= ((1 << m[i][j]) >> 1); //Bit Magic FTW
		}

		blockmasks[j + 2 * mxmy] = 0;

		for (int i = 0; i < mxmy; ++i)
		{
			blockmasks[j + 2 * mxmy] |= ((1 << src[block2offset[j + 2 * mxmy][i]]) >> 1); //Bit Magic FTW
			//int offset = my * mxmy * (j / my) + mx * (j % my) + i % mx + (i / mx) * mxmy;
			//blockmasks[j + 2 * mxmy] |= ((1 << src[offset]) >> 1); //Bit Magic FTW
		}
	}

	for (int o = 0; o < mxmymxmy; ++o)
	{
		nm[o] = (src[o]) ? (1 << src[o]) >> 1 : ~(blockmasks[offset2row[o]] | blockmasks[offset2column[o]] | blockmasks[offset2block[o]])&mflags; //'~&' FASTER THAN '^' %)
		solved += (bitcountarray[nm[o]] == 1) ? 1 : 0;
	}


#ifdef DEBUG
	showboard(0, 0, nm);
	std::cout << "Solved: " << solved << std::endl << std::endl;
#endif // DEBUG

	int z;
	while ((solved != mxmymxmy) && (z = MarkHiddenSingles(nm)))
	{
		solved += z;
	};

#ifdef DEBUG
	showboard(0, 0, nm);
	std::cout << "Solved: " << solved << std::endl << std::endl;
#endif // DEBUG

	for (int *t = &solution[0][0], *s = &nm[0];
		t < &solution[0][0] + mxmymxmy;
		++t, ++s)
	{
		*t = mask2digit[*s];
	}

	//for (int *t = &solution[0][0], *s = &nm[0];
	//	t < &solution[0][0] + mxmymxmy;
	//	++t, ++s)
	//{
	//	int val = *s;
	//	int f = 1;
	//	while (val >>= 1) //из маски получаем цифру
	//	{
	//		f++;
	//	}
	//	*t = f;
	//}

	return solved / mxmymxmy;
}

