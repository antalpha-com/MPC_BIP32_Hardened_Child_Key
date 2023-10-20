#include "Common.h"

#ifdef DEBUG

void U32_prt(const U32 *n, U32 wordLen, const char *name)
{
	U32 i;
	if(name == NULL)
	{
		printf("{");
	}
	else
	{
		printf("%s:{", name);
	}
	if(wordLen == 0)
	{
		printf("0}\r\n");
		return;
	}
	if(wordLen > MAXWORDLEN)
	{
		wordLen = MAXWORDLEN;
	}
	for(i = wordLen - 1; i != 0; i--)
	{
		printf("%08X ", n[i]);
	}
	printf("%08X}\r\n", n[0]);
}

void U8_prt(const U8 *n, U32 byteLen, const char *name)
{
	U32 i;
	if(name == NULL)
	{
		printf("{");
	}
	else
	{
		printf("%s:{", name);
	}
	if(byteLen == 0)
	{
		printf("0}\r\n");
		return;
	}
	if(byteLen > MAXBYTELEN)
	{
		byteLen = MAXBYTELEN;
	}
	for(i = 0; i < byteLen; i++)
	{
		printf("%02X", n[i]);
	}
	printf("}\r\n");
}

void RET_prt(RET ret)
{
	switch(ret)
	{
		case 0:
			printf("SUCCESS\r\n");
			break;
		case 0x01:
			printf("FAIL\r\n");
			break;
		case 0x02:
			printf("YES\r\n");
			break;
		case 0x03:
			printf("NOT\r\n");
			break;

		case 0x10:
			printf("LENGTH_TOO_LONG\r\n");
			break;
		case 0x11:
			printf("DATA_TOO_BIG\r\n");
			break;
		case 0x12:
			printf("NULL_POINTER_ERROR\r\n");
			break;
		case 0x13:
			printf("ZERO_VALUE_ERROR\r\n");
			break;

		case 0x20:
			printf("EVEN_ERROR\r\n");
			break;
		case 0x21:
			printf("MODINV_ERROR\r\n");
			break;
		case 0x22:
			printf("BIGNUM_ERROR\r\n");
			break;

		case 0x30:
			printf("PRIKEY_ERROR\r\n");
			break;
		case 0x31:
			printf("PUBKEY_ERROR\r\n");
			break;
		case 0x32:
			printf("INF_POINT_ERROR\r\n");
			break;
		case 0x33:
			printf("POINT_NOT_ON_CRV_ERROR\r\n");
			break;
		case 0x34:
			printf("POINT_MUL_ERROR\r\n");
			break;
		case 0x35:
			printf("LOAD_BIGNUM_ERROR\r\n");
			break;
		case 0x36:
			printf("LOAD_POINT_ERROR\r\n");
			break;
		case 0x37:
			printf("CURVE_PARM_ERROR\r\n");
			break;
		case 0x38:
			printf("HASH_ERROR\r\n");
			break;
		case 0x39:
			printf("KDF_ERROR\r\n");
			break;
		case 0x3A:
			printf("HMAC_ERROR\r\n");
			break;
		case 0x3B:
			printf("ENC_ERROR\r\n");
			break;
		case 0x3C:
			printf("SM2_ERROR\r\n");
			break;
		case 0x3D:
			printf("ECC_ERROR\r\n");
			break;

		case 0x40:
			printf("SM9_ERROR\r\n");
			break;
		case 0x41:
			printf("SM9_GETRATE_ERROR\r\n");
			break;
		
		case 0x50:
			printf("PRIME_ERROR\r\n");
			break;
		case 0x51:
			printf("RSA_ERROR\r\n");
			break;

		default:
			printf("UNKNOW_ERROR\r\n");
			break;
	}
}

#endif

#define GET_U32(n,b,i)                       \
{                                            \
    (n) = ( (U32) (b)[(i)    ] << 24 )       \
        | ( (U32) (b)[(i) + 1] << 16 )       \
        | ( (U32) (b)[(i) + 2] <<  8 )       \
        | ( (U32) (b)[(i) + 3]       );      \
}

#define PUT_U32(n,b,i)                       \
{                                            \
    (b)[(i)    ] = (U8) ( (n) >> 24 );       \
    (b)[(i) + 1] = (U8) ( (n) >> 16 );       \
    (b)[(i) + 2] = (U8) ( (n) >>  8 );       \
    (b)[(i) + 3] = (U8) ( (n)       );       \
}

U32 strToU8(const char *src, U8 *dst)
{
	U32 i = 0, j = 0, k = 0;
	U8 tmp;
	while(src[i] != '\0')
	{
		if(src[i] == '0' && (src[i + 1] == 'x' || src[i + 1] == 'X'))
		{
			i += 2;
			continue;
		}
		else if(src[i] >= '0' && src[i] <= '9')
		{
			tmp = (U8)(src[i] - '0');
		}
		else if(src[i] >= 'A' && src[i] <= 'F')
		{
			tmp = (U8)(src[i] - 'A' + 10);
		}
		else if(src[i] >= 'a' && src[i] <= 'f')
		{
			tmp = (U8)(src[i] - 'a' + 10);
		}
		else
		{
			i++;
			continue;
		}

		if(k & 1)
		{
			dst[j] = (dst[j] << 4) | tmp;
			j++;
			if(j >= MAXBYTELEN)
			{
				break;
			}
		}
		else
		{
			dst[j] = tmp;
		}

		k++;
		i++;
	}

	if(k & 1)
	{
		return 0;
	}

	return j;
}

RET U32ToU8(const U32 *src, U8 *dst, U32 byteLen)
{
	S32 i, tmpLen = (S32)byteLen - 4;
	U32 j = 0;
	const U32 *psrc = src;
	U32 tmp[MAXWORDLEN];// = {0};

	if(byteLen > MAXBYTELEN)
	{
		return LENGTH_TOO_LONG;
	}
	if(src == (U32*)dst)
	{
		memcpy(tmp, src, byteLen);
		psrc = tmp;
	}
	for(i = 0; i <= tmpLen; i += 4)
	{
		PUT_U32(psrc[j], dst, tmpLen - i);
		j++;
	}
	tmpLen = (S32)(byteLen & 3) - 1;
	for(i = 0; i <= tmpLen; i++)
	{
		dst[i] = (U8)(psrc[j] >> ((U32)(tmpLen - i) << 3));
	}

	return SUCCESS;
}

RET U8ToU32(const U8 *src, U32 *dst, U32 byteLen)
{
	S32 i, tmpLen = (S32)(byteLen & 3) - 1;
	U32 j = 0;
	const U8 *psrc = src;
	U8 tmp[MAXBYTELEN];// = {0};

	if(byteLen > MAXBYTELEN)
	{
		return LENGTH_TOO_LONG;
	}
	if(src == (U8*)dst)
	{
		memcpy(tmp, src, byteLen);
		psrc = tmp;
	}
	for(i = (S32)byteLen - 4; i >= 0; i -= 4)
	{
		GET_U32(dst[j], psrc, i);
		j++;
	}
	if(tmpLen >= 0)
	{
		dst[j] = 0;
		for(i = tmpLen; i >= 0; i--)
		{
			dst[j] |= (U32)(psrc[i]) << ((U32)(tmpLen - i) << 3);
		}
	}

	return SUCCESS;
}

#undef GET_U32
#undef PUT_U32

static const U8 bits [256] = 
{
	0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
	8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
};

U32 U32_getBitLen(U32 n)
{
	if(n & 0xFFFF0000)
	{
		if(n & 0xFF000000)
		{
			return (bits[n >> 24] + 24);
		}
		else
		{
			return (bits[n >> 16] + 16);
		}
	}
	else
	{
		if(n & 0xFF00)
		{
			return (bits[n >> 8] + 8);
		}
		else
		{
			return bits[n];
		}
	}
}

