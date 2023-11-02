// Copyright © 2023 Antalpha
//
// This file is part of Antalpha. The full Antalpha copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#ifndef HEADER_BIGNUM_H
#define HEADER_BIGNUM_H

#include <string.h>
#include "../Common/Common.h"

#define BIGNUM_MAXLEN MAXWORDLEN

typedef struct
{
	U32 *data;	//unsigned int
	U32	maxLen;
	U32 len;
}BigNum;

/*****************************************************************************************************************/

#ifdef DEBUG

#include <stdio.h>

/*
 * ��ӡ���� n ����Ļ��, name ��Ϊ NULL
 */
void BigNum_prt(const BigNum *n, const char *name);

#endif

/*
 * �������ÿռ�, ���� n �ĳ���Ϊ��Ч����, �����޹ؿռ�
 */
RET BigNum_clr(BigNum *n);

/*
 * ��ȡ�� i �����ص�ֵ���� 0 ��ʼ���㣩
 */
U32 BigNum_getBitVal(const BigNum *n, U32 i);

/*
 * ��ȡ���س��ȣ��� 1 ��ʼ���㣩
 */
U32 BigNum_getBitLen(const BigNum *n);

/*
 * �Ƚ���������С:  a > b ���� 1,a < b ���� -1,a = b ���� 0
 * Ҫ�� a->len �� b->len ��Ϊ����Ч����
 */
S32 BigNum_cmp(const BigNum *a, const BigNum *b);

/*
 * ��������, ��� src �ĳ��ȱ� dst ��󳤶ȳ�, ��ֻ������Ч����
 */
//RET BigNum_cpy(BigNum *dst, const BigNum *src);
RET BigNum_cpy(const BigNum *src, BigNum *dst);

/*
 * ��������Ϊ 0
 */
RET BigNum_setZero(BigNum *n);

/*****************************************************************************************************************/

/*
 * ������: c = a + b
 */
RET BigNum_add(const BigNum *a, const BigNum *b, BigNum *c);

/*
 * ���������ż�: c = a + b, 0��1�ֱ����������
 */
RET BigNum_signedAdd(const BigNum *a, U32 aSign, const BigNum *b, U32 bSign, BigNum *c, U32 *cSign);

/*
 * ������һ: c = a + 1
 */
RET BigNum_addOne(const BigNum *a, BigNum *c);

/*
 * ������:  c = a - b,Ҫ�� a >= b
 */
RET BigNum_sub(const BigNum *a, const BigNum *b, BigNum *c);

/*
 * ���������ż�: c = a + b, 0��1�ֱ����������
 */
RET BigNum_signedSub(const BigNum *a, U32 aSign, const BigNum *b, U32 bSign, BigNum *c, U32 *cSign);

/*
 * ������һ: c = a - 1,Ҫ�� a >= 1
 */
RET BigNum_subOne(const BigNum *a, BigNum *c);

/*
 * ������:  c = a * b
 */
RET BigNum_mul(const BigNum *a, const BigNum *b, BigNum *c);

/*
 * ����ƽ��:  c = a^2
 */
RET BigNum_squ(const BigNum *a, BigNum *c);

/*
 * �������� len ����
 */
RET BigNum_shl(const BigNum *a, U32 len, BigNum *c);

/*
 * �������� len ����
 */
RET BigNum_shr(const BigNum *a, U32 len, BigNum *c);

/*
 * ��������:  a �� b = q ���� r
 */
RET BigNum_div(const BigNum *a, const BigNum *b, BigNum *q, BigNum *r);

/*****************************************************************************************************************/

/*
 * c = a mod n
 */
#define BigNum_mod(a, n, c)		BigNum_div(a, n, NULL, c)

/*
 * c = (a + b) mod n
 */
RET BigNum_modAdd(const BigNum *a,  const BigNum *b,  const BigNum *n,  BigNum *c);

/*
 * c = (a - b) mod n, ���� 0 <= a < n, 0 <= b < n
 */
RET BigNum_modSub(const BigNum *a,  const BigNum *b,  const BigNum *n,  BigNum *c);

/*
 * c = a * b mod n
 */
RET BigNum_modMul(const BigNum *a,  const BigNum *b,  const BigNum *n,  BigNum *c);

/*
 * c = a^2 mod n
 */
RET BigNum_modSqu(const BigNum *a,  const BigNum *n,  BigNum *c);

/*
 * c = 2a mod n, ���� 0 <= a < n
 */
RET BigNum_modMul2(const BigNum *a,  const BigNum *n,  BigNum *c);

/*
 * c = a/2 mod n, ���� 0 <= a < n, Ҫ�� n Ϊ����
 */
RET BigNum_modDiv2(const BigNum *a,  const BigNum *n,  BigNum *c);

/*
 * 1 = a * c mod n
 */
RET BigNum_modInv(const BigNum *a,  const BigNum *n,  BigNum *c);


/*****************************************************************************************************************/

typedef struct
{
	const BigNum	*n;
	U32				ni0;
	U32				rr[MAXWORDLEN];
}BigNum_mont_ctx;

/*
 * ���� Montgomery ģ�˵�������,�� Montgomery ģ���ڼ�,n ���뱣�ֲ���
 */
RET BigNum_mont_set(BigNum_mont_ctx *ctx, const BigNum *n);

/*
 * ������ a ת���� Montgomery ��ʽ ma
 */
RET BigNum_mont_start(const BigNum_mont_ctx *ctx, const BigNum *a, BigNum *ma);

/*
 * Montgomery ģ��, mc = Mont(ma, mb)
 */
RET BigNum_mont_mul(const BigNum_mont_ctx *ctx, const BigNum *ma, const BigNum *mb, BigNum *mc);

/*
 * Montgomery ģ��, mc = Mont^n (ma)
 */
RET BigNum_mont_exp(const BigNum_mont_ctx *ctx, const BigNum *ma, const BigNum *e, BigNum *mc);

/*
 * �� Montgomery ��ʽ���� ma ת����һ����ʽ a
 */
RET BigNum_mont_finish(const BigNum_mont_ctx *ctx, const BigNum *ma, BigNum *a);

/*
 * ģ��, c = a^e mod n
 */
RET BigNum_modExp(const BigNum *a, const BigNum *e, const BigNum *n, BigNum *c);

/*****************************************************************************************************************/

#endif

