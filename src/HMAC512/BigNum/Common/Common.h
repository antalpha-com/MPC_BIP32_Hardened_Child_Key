#ifndef	HEADER_COMMON_H
#define HEADER_COMMON_H

#include <string.h>
#include <stdlib.h>
#include <time.h>


#define DEBUG

#define MAXWORDLEN			128
#define MAXBYTELEN			(MAXWORDLEN << 2)

typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int		U32;
typedef unsigned long long	U64;
typedef signed char			S8;
typedef short				S16;
typedef long				S32;
typedef long long			S64;

typedef enum
{
	SUCCESS					= 0,
	FAIL					= 0x01,
	YES						= 0x02,
	NOT						= 0x03,

	LENGTH_TOO_LONG			= 0x10,
	DATA_TOO_BIG			= 0x11,
	NULL_POINTER_ERROR		= 0x12,
	ZERO_VALUE_ERROR		= 0x13,

	EVEN_ERROR				= 0x20,
	MODINV_ERROR			= 0x21,
	BIGNUM_ERROR			= 0x22,

	PRIKEY_ERROR			= 0x30,
	PUBKEY_ERROR			= 0x31,
	INF_POINT_ERROR			= 0x32,
	POINT_NOT_ON_CRV_ERROR	= 0x33,
	POINT_MUL_ERROR			= 0x34,
	LOAD_BIGNUM_ERROR		= 0x35,
	LOAD_POINT_ERROR		= 0x36,
	CURVE_PARM_ERROR		= 0x37,
	HASH_ERROR				= 0x38,
	KDF_ERROR				= 0x39,
	HMAC_ERROR				= 0x3A,
	ENC_ERROR				= 0x3B,
	SM2_ERROR				= 0x3C,
	ECC_ERROR				= 0x3D,

	SM9_ERROR				= 0x40,
	SM9_GETRATE_ERROR		= 0x41,

	PRIME_ERROR				= 0x50,
	RSA_ERROR				= 0x51,

}RET;

#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#define MAX(a, b) ((a) >= (b) ? (a) : (b))

#ifdef DEBUG

#include <stdio.h>

/*
 * 打印 U32 数组到屏幕上
 */
void U32_prt(const U32 *n, U32 wordLen, const char *name);

/*
 * 打印 U8 数组到屏幕上
 */
void U8_prt(const U8 *n, U32 byteLen, const char *name);

/*
 * 打印错误信息
 */
void RET_prt(RET ret);

#endif

/*
 * 字符串转换成 U8 数组, 返回长度
 */
U32 strToU8(const char *src, U8 *dst);

/*
 * U32 数组（字间小端）转换成 U8 数组（字节大端）
 */
RET U32ToU8(const U32 *src, U8 *dst, U32 byteLen);

/*
 * U8 数组（字节大端）转换成 U32 数组（字间小端）
 */
RET U8ToU32(const U8 *src, U32 *dst, U32 byteLen);

/*
 * 获取单个word的比特长度（从 1 开始计算）
 */
U32 U32_getBitLen(U32 n);

/*
 * 设置随机数种子
 */
void setSeed(int s);

/*
 * 获取 U32 型随机数
 */
RET U32_getRand(U32 *n, U32 len);

#endif

