// Copyright © 2023 Antalpha
//
// This file is part of Antalpha. The full Antalpha copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "Common.h"

static U32 seed = 0;

static U32 rand_U32()
{
	U32 i = 0x80000000, j = 32, r = 0;
	while(i >= 1 && (i & RAND_MAX) == 0)
	{
		i >>= 1;
		j--;
	}
	i = 0;
	while(i <= 32)
	{
		r += rand() << i;
		i += j;
	}
	seed ^= r;
	return r;
}

void setSeed(int s)
{
	seed = (U32)s;
}

RET U32_getRand(U32 *n, U32 len)
{
	U32 i;

	if(len > MAXWORDLEN)
	{
		return LENGTH_TOO_LONG;
	}

	srand(seed + (U32)time(NULL) + clock());

	for(i = 0; i < len; i++)
	{
		n[i] = rand_U32();
	}

	return SUCCESS;
}


