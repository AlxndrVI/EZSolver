#include <iostream>
#include <chrono>
#include <ppl.h>
#include <stdio.h>
#include <stdlib.h>

#include "GlobalVars.h"
#include "EZSolvers.h"
#include "ZhouSolver.h"
#include "JSolve.h"
#include "JCZSolve.h"

#pragma warning(disable:1079)

typedef struct cellnode
{
	struct cellnode * nextcell;
	int* value;
};

typedef struct blocknode
{
	struct blocknode * nextblock;
	struct cellnode cell;
	int blocktype; //row, column or block(square if mx==my);
};

int solution[mxmy][mxmy];

//int bitcountarray[1 << mxmy];

//int offset2row[mxmymxmy];
//int offset2column[mxmymxmy];
//int offset2block[mxmymxmy];

void show(int m[mxmy][mxmy])
{
	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			std::cout << m[j][i] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void show2(int m[mxmy][mxmy])
{
	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			int val = m[j][i];
			if (bitcount(val) == 1)
			{
				int f = 1;
				while (val >>= 1) //из маски получаем цифру
				{
					f++;
				}
				std::cout << f << "\t";
			}
			else
				std::cout << 0 << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void show3(int x, int y, int m[mxmy][mxmy])
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
						if (m[row][digit] & (1 << bit))
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

void printflags(int flags)
{
	for (int i = 0; i <= mxmy; ++i)
	{
		if (flags & (1 << i)) std::cout << i << " ";
	}
}

unsigned __int64 solvertest(int x, int y, int m[mxmy][mxmy], unsigned __int64 Solutions)
{
	int xx, yy, n; //val «начение в €чейке, n количество бит в €чейке
	xx = x; yy = y;

	show3(x, y, m);

	if (y == mxmy) //дошли до конца
	{
		if (Solutions == 0) memcpy(solution, m, mxmymxmy*sizeof(int)); //нас интересует только первое решение
		return ++Solutions; // добавим в копилочку
	}

	n = bitcount(m[y][x]);

	if (n == 0) return Solutions;

	if (x < mxmy - 1) ++x;
	else
	{
		x = 0; ++y;
	}

	if (n > 0)
	{
		for (int f = 0; f < mxmy; ++f)
		{
			int val = (1 << f);
			if (m[yy][xx] & val)
			{
				int nm[mxmy][mxmy];
				int error = 0;
				memcpy(nm, m, sizeof(int)*mxmymxmy);

				for (int i = 0; i < mxmy; ++i)
				{
					//if (nm[yy][i] == val)
					//{
					//	error = 1;
					//	i = mxmy;
					//}
					//else
					nm[yy][i] &= ~val;
				}

				if (error == 0)
					for (int j = 0; j < mxmy; ++j)
					{
						//if (nm[j][xx] == val)
						//{
						//	error = 1;
						//	j = mxmy;
						//}
						//else
						nm[j][xx] &= ~val;
					}

				if (error == 0)
					for (int j = my * (yy / my); j < my * (yy / my) + my; ++j)
					{
						for (int i = mx * (xx / mx); i < mx * (xx / mx) + mx; ++i)
						{
							//if (nm[j][i] == val)
							//{
							//	error = 1;
							//	j = mxmy;
							//	i = mxmy;
							//}
							//else
							//							nm[j][i] &= ~val;
							nm[j][i] = -1;
						}
					}
				show3(x, y, nm);
				if (error == 1)
				{
					nm[yy][xx] = val;
					Solutions = solvertest(x, y, nm, Solutions);
				}
			}
		}
	}
	//else Solutions = solvertest(x, y, m); //jmp

	return Solutions;
}

inline int clearmark(int *Value, blocknode blocknodes[mxmy * 3], int board[mxmy][mxmy])
{
	int count = 0;
	int offset = Value - *board;
	cellnode* currentcell, *prevcell;
	int value = *Value;
	int flag = ~value&mflags;

	prevcell = currentcell = &blocknodes[offset2row[offset]].cell;

	while (currentcell = currentcell->nextcell)
	{
		if (*currentcell->value == value)
		{
			prevcell->nextcell = currentcell->nextcell;
			++count;
		}
		else if ((*currentcell->value & value) && (bitcount(*currentcell->value) == 2))
		{
			*currentcell->value &= flag;
			prevcell = currentcell;
			count += clearmark(currentcell->value, blocknodes, board);
		}
		else
		{
			*currentcell->value &= flag;
			prevcell = currentcell;
		}
	}

	prevcell = currentcell = &blocknodes[offset2column[offset]].cell;

	while (currentcell = currentcell->nextcell)
	{
		if (*currentcell->value == value)
		{
			prevcell->nextcell = currentcell->nextcell;
			++count;
		}
		else if ((*currentcell->value & value) && (bitcount(*currentcell->value) == 2))
		{
			*currentcell->value &= flag;
			prevcell = currentcell;
			count += clearmark(currentcell->value, blocknodes, board);
		}
		else
		{
			*currentcell->value &= flag;
			prevcell = currentcell;
		}
	}

	prevcell = currentcell = &blocknodes[offset2block[offset]].cell;

	while (currentcell = currentcell->nextcell)
	{
		if (*currentcell->value == value)
		{
			prevcell->nextcell = currentcell->nextcell;
			++count;
		}
		else if ((*currentcell->value & value) && (bitcount(*currentcell->value) == 2))
		{
			*currentcell->value &= flag;
			prevcell = currentcell;
			count += clearmark(currentcell->value, blocknodes, board);
		}
		else
		{
			*currentcell->value &= flag;
			prevcell = currentcell;
		}
	}

	return count;
}

int checkhiddensingles(blocknode blocknodes[mxmy * 3])
{
	blocknode *currentblock;
	cellnode *currentcell;
	int count = 0;

	currentblock = &blocknodes[0];
	while (currentblock)
	{
		currentcell = &currentblock->cell;
		int m = 0, v = 0, x = 0;
		while (currentcell = currentcell->nextcell)
		{
			x = x | *currentcell->value & m;
			v = (m ^ *currentcell->value) & ~x;
			m = m | *currentcell->value;
		}
		currentcell = &currentblock->cell;
		while (currentcell = currentcell->nextcell)
		{
			if (*currentcell->value & v)
			{
				++count;
				*currentcell->value = (*currentcell->value)&v;
			}
		}

		currentblock = currentblock->nextblock;
	}

	currentblock = &blocknodes[9];
	while (currentblock)
	{
		currentcell = &currentblock->cell;
		int m = 0, v = 0, x = 0;
		while (currentcell = currentcell->nextcell)
		{
			x = x | *currentcell->value & m;
			v = (m ^ *currentcell->value) & ~x;
			m = m | *currentcell->value;
		}
		currentcell = &currentblock->cell;
		while (currentcell = currentcell->nextcell)
		{
			if (*currentcell->value & v)
			{
				++count;
				*currentcell->value = (*currentcell->value)&v;
			}
		}

		currentblock = currentblock->nextblock;
	}

	currentblock = &blocknodes[18];
	while (currentblock)
	{
		currentcell = &currentblock->cell;
		int m = 0, v = 0, x = 0;
		while (currentcell = currentcell->nextcell)
		{
			x = x | *currentcell->value & m;
			v = (m ^ *currentcell->value) & ~x;
			m = m | *currentcell->value;
		}
		currentcell = &currentblock->cell;
		while (currentcell = currentcell->nextcell)
		{
			if (*currentcell->value & v)
			{
				++count;
				*currentcell->value = (*currentcell->value)&v;
			}
		}

		currentblock = currentblock->nextblock;
	}

	return count;
}

unsigned __int64 newsolver()
{


	int nm[mxmy][mxmy];

	struct cellnode cellnodes[mxmymxmy * 3];
	struct blocknode blocknodes[mxmy * 3];



	//for (int *t = *nm, size = mxmymxmy;
	//	0 < size;
	//	++t, --size)
	//{
	//	*t = mflags;
	//}

	for (int *t = &nm[0][0];
		t < &nm[0][0] + mxmymxmy;
		++t)
	{
		*t = mflags;
	}

	//«аполн€ем nm
	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			if (m[j][i] != 0)
				nm[j][i] = (1 << ((m[j][i]) - 1));
		}
	}

	//cellnodes grouping
	for (int k = 0; k < 3; ++k)
	{
		for (int j = 0; j < mxmy; ++j)
		{

			blocknodes[k * mxmy + j].blocktype = k;
			blocknodes[k * mxmy + j].nextblock = &blocknodes[k * mxmy + j + 1];
			blocknodes[k * mxmy + j].cell.nextcell = &cellnodes[k * mxmy * mxmy + j*mxmy];


			for (int i = 0; i < mxmy; ++i)
			{
				cellnodes[k * mxmy * mxmy + j * mxmy + i].nextcell = &cellnodes[k * mxmy * mxmy + j * mxmy + i + 1];
				if (k == 0) //row
				{
					cellnodes[k * mxmy * mxmy + j * mxmy + i].value = &nm[j][i]; // &nm[0][0] + (j*mxmy + i);
					//offset2row[j*mxmy + i] = k * mxmy + j; //link to blocknode
				}
				else if (k == 1) //column
				{
					cellnodes[k * mxmy * mxmy + j * mxmy + i].value = &nm[i][j]; // &nm[0][0] + (i*mxmy + j);
					//offset2column[i * mxmy + j] = k * mxmy + j;
				}
				else //block
				{
					cellnodes[k * mxmy * mxmy + j * mxmy + i].value = &nm[0][0] + (my * mxmy * (j / my) + mx * (j % my) + i % mx + (i / mx) * mxmy);
					//offset2block[my * mxmy * (j / my) + mx * (j % my) + i % mx + (i / mx) * mxmy] = k * mxmy + j;
				}
			}
			cellnodes[k * mxmy * mxmy + (j + 1) * mxmy - 1].nextcell = 0;
		}
		blocknodes[(k + 1) * mxmy - 1].nextblock = 0;
	}

	blocknode* currentblock;
	cellnode* currentcell;
	//prevcell = &currentblock->cell;
	//currentcell = prevcell->nextcell;

	//for (int y = 0; y < mxmy * 3; ++y)
	//{
	//	currentblock = &blocknodes[y];
	//	prevcell = &currentblock->cell;
	//	currentcell = prevcell->nextcell;

	//	std::cout << "Block # " << y << " . Type " << currentblock->blocktype << std::endl;
	//	while (currentcell)
	//	{
	//		int offset = currentcell->value - &nm[0][0];

	//		switch (currentblock->blocktype)
	//		{

	//		case 0:
	//			std::cout << "\t" << offset << " Val2Row    " << offset2row[offset] << std::endl;
	//			break;

	//		case 1:
	//			std::cout << "\t" << offset << " Val2Column " << offset2column[offset] << std::endl;
	//			break;

	//		case 2:
	//			std::cout << "\t" << offset << " Val2block  " << offset2block[offset] << std::endl;
	//			break;

	//		}
	//		currentcell = currentcell->nextcell;
	//	}

	//}

	//return 1;
	//std::cout << "\nRow\n";
	//for (int i = 0; i < mxmy * mxmy; ++i)
	//{
	//	std::cout << i << "\t" << offset2row[i] << std::endl;
	//}

	//std::cout << "\nColumn\n";
	//for (int i = 0; i < mxmy * mxmy; ++i)
	//{
	//	std::cout << i << "\t" << offset2column[i] << std::endl;
	//}

	//std::cout << "\nBlock\n";
	//for (int i = 0; i < mxmy * mxmy; ++i)
	//{
	//	std::cout << i << "\t" << offset2block[i] << std::endl;
	//}
	//return 1;

	//for (int *t = &nm[0][0];
	//	t < &nm[0][0] + mxmymxmy;
	//	++t)
	//{
	//	*t = t - &nm[0][0];
	//}
	int oldcount;
	int count = 0;
	do
	{
		oldcount = count;
		currentblock = &blocknodes[0]; //строки
		while (currentblock)
		{
			currentcell = &currentblock->cell;

			while (currentcell = currentcell->nextcell)
			{
				if (bitcount(*currentcell->value) == 1)
				{
					count += clearmark(currentcell->value, blocknodes, nm);
				}
			}
			currentblock = currentblock->nextblock;
		}

		currentblock = &blocknodes[mxmy]; //колонки
		while (currentblock)
		{
			currentcell = &currentblock->cell;

			while (currentcell = currentcell->nextcell)
			{
				if (bitcount(*currentcell->value) == 1)
				{
					count += clearmark(currentcell->value, blocknodes, nm);
				}
			}
			currentblock = currentblock->nextblock;
		}

		currentblock = &blocknodes[2 * mxmy]; //блоки
		while (currentblock)
		{
			currentcell = &currentblock->cell;

			while (currentcell = currentcell->nextcell)
			{
				if (bitcount(*currentcell->value) == 1)
				{
					count += clearmark(currentcell->value, blocknodes, nm);
				}
			}
			currentblock = currentblock->nextblock;
		}


		//currentblock = &blocknodes[0]; //строки
		//while (currentblock)
		//{
		//	currentcell = &currentblock->cell;

		//	while (currentcell = currentcell->nextcell)
		//	{
		//		if (bitcount(*currentcell->value) != 1)
		//		{
		//			count += clearmark(currentcell->value, blocknodes, nm);
		//		}
		//	}
		//	currentblock = currentblock->nextblock;
		//}



		//show3(0, 0, nm);
		//std::cout << " Count :\t" << count << std::endl;
		count += checkhiddensingles(blocknodes);
	} while (count != oldcount);

	for (int *t = &solution[0][0], *s = &nm[0][0];
		t < &solution[0][0] + mxmymxmy;
		++t, ++s)
	{
		int val = *s;
		int f = 1;
		while (val >>= 1) //из маски получаем цифру
		{
			f++;
		}
		*t = f;
	}


	return 1;
}
unsigned __int64 solver()
{
	//создаем массив с которого начнем считать
	int nm[mxmy][mxmy];

	for (int *t = &nm[0][0], size = mxmymxmy;
		0 < size;
		++t, --size)
	{
		*t = mflags;
	}

	//show(m);

	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			if (m[j][i] != 0)
			{

				int val = (1 << ((m[j][i]) - 1));
				//Eliminates current number from markups of current box, column and row;
				for (int x = 0; x < mxmy; ++x)
				{
					nm[j][x] &= ~val;
				}

				for (int y = 0; y < mxmy; ++y)
				{
					nm[y][i] &= ~val;
				}

				for (int y = my * (j / my); y < my * (j / my) + my; ++y)
				{
					for (int x = mx * (i / mx); x < mx * (i / mx) + mx; ++x)
					{
						nm[y][x] &= ~val;
					}
				}
				nm[j][i] = val;
			}
		}
	}

	//show3(0, 0, nm);
	int count = 0, oldcount = 0;

loop:

	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			int val;
			if (bitcount(nm[j][i]) != 1)
			{
				//собираем все варианты в строке кроме текущей €чейки
				val = 0;
				for (int x = 0; x < mxmy; ++x)
				{
					if (i != x)
						val |= nm[j][x];
				}

				val = ~val & mflags;
				//если после инвертировани€ флага осталс€ один вариант то тут об€зательно скрыта€ цифра
				if (bitcount(val) == 1)
				{
					//по горизонтали уже найдена цифра, осталось обновить колонку и блок
					for (int y = 0; y < mxmy; ++y)
					{
						nm[y][i] &= ~val;
					}

					for (int y = my * (j / my); y < my * (j / my) + my; ++y)
					{
						for (int x = mx * (i / mx); x < mx * (i / mx) + mx; ++x)
						{
							nm[y][x] &= ~val;
						}
					}
					//что бы не обходить €чейку после удалени€ флага восстанавливаем значение исходной €чейки
					nm[j][i] = val;
					//можем переходить к следующей €чейке, нам не требуетс€ провер€ть заного.
					continue;
					//goto exit;
				}

				val = 0;
				for (int y = 0; y < mxmy; ++y)
				{
					if (j != y)
						val |= nm[y][i];
				}

				val = ~val & mflags;
				if (bitcount(val) == 1)
				{
					for (int x = 0; x < mxmy; ++x)
					{
						nm[j][x] &= ~val;
					}

					for (int y = my * (j / my); y < my * (j / my) + my; ++y)
					{
						for (int x = mx * (i / mx); x < mx * (i / mx) + mx; ++x)
						{
							nm[y][x] &= ~val;
						}
					}
					nm[j][i] = val;

					continue;
					//goto exit;
				}

				val = 0;
				for (int y = my * (j / my); y < my * (j / my) + my; ++y)
				{
					for (int x = mx * (i / mx); x < mx * (i / mx) + mx; ++x)
					{
						if ((j != y) && (i != x))
							val != nm[y][x];
					}
				}
				val = ~val & mflags;
				if (bitcount(val) == 1)
				{
					for (int y = 0; y < mxmy; ++y)
					{
						nm[y][i] &= ~val;
					}

					for (int x = 0; x < mxmy; ++x)
					{
						nm[j][x] &= ~val;
					}

					nm[j][i] = val;
				}
				//exit:
			}
		}

		oldcount = count;
		//show3(0, 0, nm);
	}



	count = mxmymxmy;
	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			if (bitcount(nm[j][i]) == 1)
			{
				--count;
				int val = nm[j][i];

				//Eliminates current number from markups of current box, column and row;
				for (int x = 0; x < mxmy; ++x)
				{
					nm[j][x] &= ~val;
				}

				for (int y = 0; y < mxmy; ++y)
				{
					nm[y][i] &= ~val;
				}

				for (int y = my * (j / my); y < my * (j / my) + my; ++y)
				{
					for (int x = mx * (i / mx); x < mx * (i / mx) + mx; ++x)
					{
						nm[y][x] &= ~val;
					}
				}
				nm[j][i] = val;
			}
		}
	}

	for (int *t = &solution[0][0], *s = &nm[0][0];
		t < &solution[0][0] + mxmymxmy;
		++t, ++s)
	{
		int val = *s;
		int f = 1;
		while (val >>= 1) //из маски получаем цифру
		{
			f++;
		}
		*t = f;
	}

	if (count == 0) return 1;
	if (count == oldcount) {
		return 0;
	}
	//show3(0, 0, nm);

	goto loop;
}

unsigned __int64 BacktrackPreflagsSeq(int x, int y, int rowFlags[mxmy], int blockFlags[mx][my], int colFlags[mxmy], int m[mxmy][mxmy], unsigned __int64 Solutions)
{
	int xx, yy, flags;
	xx = x; yy = y; //xx, yy - old coordinates, x, y - will be new
	flags = 0;

	if (y == mxmy) //дошли до конца
	{
		if (Solutions == 0) memcpy(solution, m, mxmymxmy*sizeof(int)); //нас интересует только первое решение
		return ++Solutions; // добавим в копилочку
	}

	//если есть кандидаты то ведем поиск глубже

	flags = rowFlags[yy] | blockFlags[yy / my][xx / mx] | colFlags[xx];

	if (x < mxmy - 1)
	{
		++x;
	}
	else
	{
		x = 0; ++y;
	}

	if (m[yy][xx] == 0)
	{
		int nm[mxmy][mxmy];
		memcpy(nm, m, sizeof(int)*mxmymxmy);

		for (int i = 1; i < mxmy + 1; ++i)
		{
			if ((flags & (1 << i)) == 0)
			{
				int rowFlags2[mxmy], blockFlags2[mx][my], colFlags2[mxmy];
				memcpy(rowFlags2, rowFlags, sizeof(int)*mxmy);
				memcpy(colFlags2, colFlags, sizeof(int)*mxmy);
				memcpy(blockFlags2, blockFlags, sizeof(int)*mxmy);

				nm[yy][xx] = i;

				rowFlags2[yy] = rowFlags[yy] | (1 << i);
				colFlags2[xx] = colFlags[xx] | (1 << i);
				blockFlags2[yy / my][xx / mx] = blockFlags[yy / my][xx / mx] | (1 << i);

				Solutions = BacktrackPreflagsSeq(x, y, rowFlags2, blockFlags2, colFlags2, nm, Solutions);
			}
		}
	}
	else Solutions = BacktrackPreflagsSeq(x, y, rowFlags, blockFlags, colFlags, m, Solutions); //jmp
	return Solutions;
}

unsigned __int64 BacktrackPreflagsSeq_1st(int x, int y, int rowFlags[mxmy], int blockFlags[mx][my], int colFlags[mxmy], int m[mxmy][mxmy], unsigned __int64 Solutions)
{
	int xx, yy, flags;
	xx = x; yy = y; //xx, yy - old coordinates, x, y - will be new
	flags = 0;

	if (y == mxmy) //дошли до конца
	{
		if (Solutions == 0) memcpy(solution, m, mxmymxmy*sizeof(int)); //нас интересует только первое решение
		return ++Solutions; // добавим в копилочку
	}

	//если есть кандидаты то ведем поиск глубже

	flags = rowFlags[yy] | blockFlags[yy / my][xx / mx] | colFlags[xx];

	if (x < mxmy - 1)
	{
		++x;
	}
	else
	{
		x = 0; ++y;
	}

	if (m[yy][xx] == 0)
	{
		int nm[mxmy][mxmy];
		memcpy(nm, m, sizeof(int)*mxmymxmy);

		for (int i = 1; i < mxmy + 1; ++i)
		{
			if ((flags & (1 << i)) == 0)
			{
				int rowFlags2[mxmy], blockFlags2[mx][my], colFlags2[mxmy];
				memcpy(rowFlags2, rowFlags, sizeof(int)*mxmy);
				memcpy(colFlags2, colFlags, sizeof(int)*mxmy);
				memcpy(blockFlags2, blockFlags, sizeof(int)*mxmy);

				nm[yy][xx] = i;

				rowFlags2[yy] = rowFlags[yy] | (1 << i);
				colFlags2[xx] = colFlags[xx] | (1 << i);
				blockFlags2[yy / my][xx / mx] = blockFlags[yy / my][xx / mx] | (1 << i);

				Solutions = BacktrackPreflagsSeq_1st(x, y, rowFlags2, blockFlags2, colFlags2, nm, Solutions);
				if (Solutions) return Solutions;
			}
		}
	}
	else Solutions = BacktrackPreflagsSeq_1st(x, y, rowFlags, blockFlags, colFlags, m, Solutions); //jmp
	return Solutions;
}


unsigned __int64 BacktrackSeq(int x, int y, int m[mxmy][mxmy], unsigned __int64 Solutions)
{
	int xx, yy;
	int flags;
	xx = x; yy = y; flags = 0;

	if (y == mxmy) //дошли до конца
	{
		if (Solutions == 0) memcpy(solution, m, mxmymxmy*sizeof(int)); //нас интересует только первое решение
		return ++Solutions;
	}

	// —охран€ем имеющиес€ цифры в строке, столбце и блоке.
	for (int i = 0; i < mxmy; ++i)
	{
		flags |= (1 << m[y][i]);
	}

	for (int j = 0; j < mxmy; ++j)
	{
		flags |= (1 << m[j][x]);
	}

	for (int j = my * (y / my); j < my * (y / my) + my; ++j)
	{
		for (int i = mx * (x / mx); i < mx * (x / mx) + mx; ++i)
		{
			flags |= (1 << m[j][i]);
		}
	}

	if (x < mxmy - 1) ++x;
	else
	{
		x = 0; ++y;
	}

	if (m[yy][xx] == 0)
	{

		for (int i = 1; i < mxmy + 1; ++i)
		{
			if ((flags & (1 << i)) != (1 << i))
			{
				int nm[mxmy][mxmy];
				memcpy(nm, m, mxmymxmy*sizeof(int));
				nm[yy][xx] = i;
				Solutions = BacktrackSeq(x, y, nm, Solutions);
			}
		}
	}
	else
		Solutions = BacktrackSeq(x, y, m, Solutions); //jmp
	return Solutions;
}


unsigned __int64 Backtracking1()
{
	//создаем массив с которого начнем считать
	int nm[mxmy][mxmy];

	memcpy(nm, m, mxmymxmy*sizeof(int));


	//запускаем рекурсию поиска вариантов.

	return BacktrackSeq(0, 0, nm, 0);
}

unsigned __int64 Backtracking2()
{
	//создаем массив с которого начнем считать
	int nm[mxmy][mxmy];

	memcpy(nm, m, mxmymxmy*sizeof(int));

	//заполн€ем маски дл€ каждой строки, колонки и блоков.
	int blockFlags[mx][my];
	int colFlags[mxmy];
	int rowFlags[mxmy];
	memset(blockFlags, 0, sizeof(int)*mxmy);
	memset(colFlags, 0, sizeof(int)*mxmy);
	memset(rowFlags, 0, sizeof(int)*mxmy);


	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			rowFlags[j] |= (1 << nm[j][i]);
			colFlags[i] |= (1 << nm[j][i]);
			blockFlags[j / my][i / mx] |= (1 << nm[j][i]);
		}
	}

	//запускаем рекурсию поиска вариантов.

	return BacktrackPreflagsSeq(0, 0, rowFlags, blockFlags, colFlags, nm, 0);
}

unsigned __int64 Backtracking2_1st()
{
	//создаем массив с которого начнем считать
	int nm[mxmy][mxmy];

	memcpy(nm, m, mxmymxmy*sizeof(int));

	//заполн€ем маски дл€ каждой строки, колонки и блоков.
	int blockFlags[mx][my];
	int colFlags[mxmy];
	int rowFlags[mxmy];
	memset(blockFlags, 0, sizeof(int)*mxmy);
	memset(colFlags, 0, sizeof(int)*mxmy);
	memset(rowFlags, 0, sizeof(int)*mxmy);


	for (int j = 0; j < mxmy; ++j)
	{
		for (int i = 0; i < mxmy; ++i)
		{
			rowFlags[j] |= (1 << nm[j][i]);
			colFlags[i] |= (1 << nm[j][i]);
			blockFlags[j / my][i / mx] |= (1 << nm[j][i]);
		}
	}

	//запускаем рекурсию поиска вариантов.

	return BacktrackPreflagsSeq_1st(0, 0, rowFlags, blockFlags, colFlags, nm, 0);
}

void start(unsigned __int64(*start)())
{
	double elapsed;
	using namespace std::chrono;
	auto chrono_t1 = high_resolution_clock::now();
	int solutionCount = 0;
	for (int i = 0; i < ITERATIONS; ++i)
		solutionCount += start();
	elapsed = duration_cast<duration<double>>(high_resolution_clock::now() - chrono_t1).count();
	switch (solutionCount)
	{
	case 0:
		std::cout << "Solution not found." << std::endl;
		break;

	case 1:

		show(solution);
		std::cout << "Found one solution." << std::endl;
		break;

	default:

		std::cout << "Found " << solutionCount << " solutions." << std::endl;
	}

	std::cout << "Elapsed time: \t"
		<< elapsed
		<< " seconds." << std::endl;
}

void start3(int(*start)(int m[mxmy][mxmy], int s[mxmy][mxmy]))
{
	double elapsed;
	using namespace std::chrono;
	auto chrono_t1 = high_resolution_clock::now();
	int solutionCount = 0;
	for (int i = 0; i < ITERATIONS; ++i)
		solutionCount += start(m, solution);
	elapsed = duration_cast<duration<double>>(high_resolution_clock::now() - chrono_t1).count();
	switch (solutionCount)
	{
	case 0:
		std::cout << "Solution not found." << std::endl;
		break;

	case 1:

		show(solution);
		std::cout << "Found one solution." << std::endl;
		break;

	default:

		std::cout << "Found " << solutionCount << " solutions." << std::endl;
	}

	std::cout << "Elapsed time: \t"
		<< elapsed
		<< " seconds." << std::endl;
}

void start2(int(*start)(const char *, char *, int))
{
	double elapsed;
	using namespace std::chrono;

	char stringBuffer[mxmymxmy * 4];
	char outputStrings[mxmymxmy * 4] = "";

	for (int i = 0; i < mxmymxmy; ++i)
	{

		_snprintf(stringBuffer, 20, "%d", m[i / mxmy][i%mxmy]);
		strcat(outputStrings, strdup(stringBuffer));
	}
	//printf("%s\n", outputStrings);
	auto chrono_t1 = high_resolution_clock::now();
	int solutionCount = 0;
	char solut[mxmymxmy + 1];
	for (int i = 0; i < ITERATIONS; ++i)
		solutionCount += start(outputStrings, solut, 1);
	elapsed = duration_cast<duration<double>>(high_resolution_clock::now() - chrono_t1).count();
	switch (solutionCount)
	{
	case 0:
		std::cout << "Solution not found." << std::endl;
		break;

	case 1:

		//show(solution);
		std::cout << solut << std::endl;
		std::cout << "Found one solution." << std::endl;
		break;

	default:

		std::cout << "Found " << solutionCount << " solutions." << std::endl;
	}

	std::cout << "Elapsed time: \t"
		<< elapsed
		<< " seconds." << std::endl;
}

void printPredefinedVars()
{
	std::cout << "static const int bitcountarray[" << (1 << mxmy) << "] =" << std::endl
		<< "{ " << std::endl;

	for (int i = 0; i < 1 << mxmy; ++i)
	{
		int count = 0; int n = i;
		while (n)
		{
			count += n & 1;
			n >>= 1;
		}
		if ((i % 16) == 0)
			std::cout << "\t";
		if ((i == ((1 << mxmy) - 1)))
			std::cout << count << std::endl;
		else if ((i % 16) == 15)
			std::cout << count << "," << std::endl;
		else
			std::cout << count << ", ";
	}
	std::cout << "};" << std::endl << std::endl;

	std::cout << "static const int mask2digit[" << (1 << mxmy) << "] =" << std::endl
		<< "{ " << std::endl;
	int mask2digit[1 << mxmy];

	memset(mask2digit, 0, sizeof(int)*(1 << mxmy));

	for (int i = 1; i < mxmy + 1; ++i)
	{
		mask2digit[((1 << i) >> 1)] = i;
	}

	for (int i = 0; i < 1 << mxmy; ++i)
	{
		if ((i % 16) == 0)
			std::cout << "\t";
		if ((i == ((1 << mxmy) - 1)))
			std::cout << mask2digit[i] << std::endl;
		else if ((i % 16) == 15)
			std::cout << mask2digit[i] << "," << std::endl;
		else
			std::cout << mask2digit[i] << ", ";
	}
	std::cout << "};" << std::endl << std::endl;

	std::cout << "static const int offset2row[" << mxmymxmy << "] =" << std::endl
		<< "{ " << std::endl;

	for (int i = 0; i < mxmymxmy; ++i)
	{
		//		int n = i / mxmy;
		int n = offset2row[i];
		if ((i % mxmy) == 0)
			std::cout << "\t";
		if ((i == (mxmymxmy - 1)))
			std::cout << n << std::endl;
		else if ((i % mxmy) == (mxmy - 1))
			std::cout << n << "," << std::endl;
		else
			std::cout << n << ", ";
	}
	std::cout << "};" << std::endl << std::endl;

	std::cout << "static const int offset2column[" << mxmymxmy << "] =" << std::endl
		<< "{ " << std::endl;

	for (int i = 0; i < mxmymxmy; ++i)
	{
		//		int n = i % mxmy + mxmy;
		int n = offset2column[i];
		if ((i % mxmy) == 0)
			std::cout << "\t";
		if ((i == (mxmymxmy - 1)))
			std::cout << n << std::endl;
		else if ((i % mxmy) == (mxmy - 1))
			std::cout << n << "," << std::endl;
		else
			std::cout << n << ", ";
	}
	std::cout << "};" << std::endl << std::endl;

	std::cout << "static const int offset2block[" << mxmymxmy << "] =" << std::endl
		<< "{ " << std::endl;

	for (int i = 0; i < mxmymxmy; ++i)
	{
		//		int n = my * mxmy * ((i / mxmy) / my) + mx * ((i / mxmy) % my) + (i%mxmy) % mx + ((i%mxmy) / mx) * mxmy + 2 * mxmy;
		int n = offset2block[i];
		if ((i % mxmy) == 0)
			std::cout << "\t";
		if ((i == (mxmymxmy - 1)))
			std::cout << n << std::endl;
		else if ((i % mxmy) == (mxmy - 1))
			std::cout << n << "," << std::endl;
		else
			std::cout << n << ", ";
	}
	std::cout << "};" << std::endl << std::endl;



	int static cons[mxmymxmy][consblocksize];
	for (int y = 0; y < mxmy; ++y)
	{
		for (int x = 0; x < mxmy; ++x)
		{
			int n = 0;
			int offset = y*mxmy + x;

			for (int j = 0; j < my*(y / my); ++j)
			{
				cons[offset][n++] = j*mxmy + x;
			}

			for (int j = my*(y / my); j < y; ++j)
			{
				for (int i = mx * (x / mx); i < mx * (1 + x / mx); ++i)
				{
					cons[offset][n++] = j*mxmy + i;
				}
			}

			for (int i = 0; i < x; ++i)
			{
				cons[offset][n++] = y*mxmy + i;
			}

			for (int i = x + 1; i < mxmy; ++i)
			{
				cons[offset][n++] = y*mxmy + i;
			}

			for (int j = y + 1; j < my * (1 + y / my); ++j)
			{
				for (int i = mx * (x / mx); i < mx * (1 + x / mx); ++i)
				{
					cons[offset][n++] = j*mxmy + i;
				}
			}

			for (int j = my * (1 + y / my); j < mxmy; ++j)
			{
				cons[offset][n++] = j*mxmy + x;
			}
		}
	}
	std::cout << "static const int cons[" << mxmymxmy << "][" << consblocksize << "] =" << std::endl
		<< "{ " << std::endl;

	for (int o = 0; o < mxmymxmy; ++o)
	{
		std::cout << "\t{ ";
		for (int p = 0; p < consblocksize; ++p)
		{
			int n = cons[o][p];
			if ((p == (consblocksize - 1)))
				std::cout << n;
			else
				std::cout << n << ", ";
		}
		if (o == mxmymxmy - 1)
			std::cout << "}" << std::endl;
		else
			std::cout << "}," << std::endl;
	}
	std::cout << "};" << std::endl << std::endl;



	std::cout << "static const int block2offset[" << mxmy * 3 << "][" << mxmy << "] =" << std::endl
		<< "{ " << std::endl;

	for (int j = 0; j < mxmy; ++j)
	{
		std::cout << "\t{ ";
		for (int i = 0; i < mxmy; ++i)
		{
			int n = j*mxmy + i;
			if ((i == (mxmy - 1)))
				std::cout << n;
			else
				std::cout << n << ", ";
		}
		std::cout << "}," << std::endl;
	}

	for (int j = 0; j < mxmy; ++j)
	{
		std::cout << "\t{ ";
		for (int i = 0; i < mxmy; ++i)
		{
			int n = i*mxmy + j;
			if ((i == (mxmy - 1)))
				std::cout << n;
			else
				std::cout << n << ", ";
		}
		std::cout << "}," << std::endl;
	}

	for (int j = 0; j < mxmy; ++j)
	{
		std::cout << "\t{ ";
		for (int i = 0; i < mxmy; ++i)
		{
			int n = my * mxmy * (j / my) + mx * (j % my) + i % mx + (i / mx) * mxmy;
			if ((i == (mxmy - 1)))
				std::cout << n;
			else
				std::cout << n << ", ";
		}
		if (j == mxmy - 1)
			std::cout << "}" << std::endl;
		else
			std::cout << "}," << std::endl;
	}
	std::cout << "};" << std::endl << std::endl;
}

void main()
{


	//for (int i = 0; i < 1 << mxmy; ++i)
	//{
	//	int count = 0; int n = i;
	//	while (n)
	//	{
	//		count += n & 1;
	//		n >>= 1;
	//	}
	//	bitcountarray[i] = count;
	//}

	//for (int j = 0; j < mxmy; ++j)
	//{
	//	for (int i = 0; i < mxmy; ++i)
	//	{
	//		offset2row[j*mxmy + i] = j;
	//		offset2column[i * mxmy + j] = mxmy + j;
	//		offset2block[my * mxmy * (j / my) + mx * (j % my) + i % mx + (i / mx) * mxmy] = 2 * mxmy + j;
	//	}
	//}

	//printPredefinedVars();
	//return;

	std::cout << "We need to find a solution to this sudoku puzzle:" << std::endl;
	show(m);

	//std::cout << "..using Backtracking1 algorithm.." << std::endl;
	//start(Backtracking1);//	Elapsed time: 	1170.85 seconds.
	//0.0058593; 3319sol: 0.205078

	//std::cout << "..using Backtracking2 algorithm.." << std::endl;
	//start(Backtracking2);//	Elapsed time: 	891.875 seconds.
	//0.0039062; 3319sol: 0.151367

	//std::cout << "..using Backtracking2 algorithm..(1st solution)" << std::endl;
	//start(Backtracking2_1st);//	Elapsed time: 	174.785 seconds.
	//0.0039062; 3319sol: 0.151367

	//std::cout << "..using Weird algorithm.." << std::endl;
	//start(solver); //	Elapsed time: 	22.6666 seconds.

	//std::cout << "..using New Weird algorithm.." << std::endl;
	//start(newsolver); // Elapsed time: 	12.5098 seconds.

	std::cout << "..EZSolver.." << std::endl;
	start3(EZSolver); //	Elapsed time: 	2.94824 seconds.

	std::cout << "..EZSolver2.." << std::endl;
	start3(EZSolver2); //	Elapsed time: 	3.51758 seconds.

	std::cout << "..EZSolver3.." << std::endl;
	start3(EZSolver3); //	Elapsed time: 	3.58594 seconds.

	std::cout << "..EZSolver4.." << std::endl;
	start3(EZSolver4); //	Elapsed time: 	3.51563 seconds.



	std::cout << "..JSolve.." << std::endl;
	start2(JSolve);//	Elapsed time: 	4.08203 seconds.

	std::cout << "..ZhouSolver.." << std::endl;
	start2(ZhouSolver);//	Elapsed time: 	2.87305 seconds.

	std::cout << "..JCZSolver.." << std::endl;
	start2(JCZSolver); //	Elapsed time: 	2.36719 seconds.
}