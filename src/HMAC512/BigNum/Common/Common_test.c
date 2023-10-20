#include "Common.h"


int Common_test()
{
	U32		n[MAXWORDLEN] = {0};
	U8		m[MAXBYTELEN] = {0};
	char	str[] = "0x01, 0x02, 0xAB, 0xcd, 0x6F, 0x9b";
	U32		strLen;

	RET		ret;
	
	setSeed(12345678);

	ret = U32_getRand(n, MAXWORDLEN * 2);
	RET_prt(ret);

	ret = U32_getRand(n, 20);
	RET_prt(ret);

	U32_prt(n, MAXWORDLEN + 20, "n");

	ret = U32ToU8(n, m, MAXBYTELEN + 1);
	RET_prt(ret);

	ret = U32ToU8(n, m, 15);
	RET_prt(ret);

	U8_prt(m, MAXBYTELEN + 20, "m");

	strLen = strToU8(str, m);

	U8_prt(m, strLen, "str");

	ret = U8ToU32(m, n, MAXBYTELEN + 20);
	RET_prt(ret);

	ret = U8ToU32(m, n, strLen);
	RET_prt(ret);

	U32_prt(n, 20, "n");

	//printf("%d\r\n", U32_getBitLen(n[2]));

	return 0;
}


