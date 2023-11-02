#include "BigNum.h"

#ifdef DEBUG

void BigNum_prt(const BigNum *n, const char *name)
{
	U32_prt(n->data, n->len, name);
}

#endif

// ��λ��: (carry, c) = a + b + carry, c �� carry ������ a �� b Ϊ��ͬ����
#define singleAdd(a, b, c, carry)		\
	(c) = (a) + (carry);				\
	if(c)								\
	{									\
		(carry) = 0;					\
	}									\
	(c) += (b);							\
	if((c) < (b))						\
	{									\
		(carry) = 1;					\
	}

//��λ��: (borrow, c) = a - b - borrow, c �� carry ������ a �� b Ϊ��ͬ����
#define singleSub(a, b, c, borrow)		\
	(c) = (b) + (borrow);				\
	if((c) >= (borrow))					\
	{									\
		(borrow) = (a) >= (c) ? 0 : 1;	\
		(c) = (a) - (c);				\
	}									\
	else								\
	{									\
		(c) = (a);						\
	}

// (H, L) = a * b
static void singleMul(U32 a, U32 b, U32 *H, U32 *L)
{
	U64 M = (U64)a * b;
	*H = (U32)(M >> 32);
	*L = (U32)M;
}

// (R2, H, L) = 2 * a * b
static void singleSqu(U32 a, U32 b, U32 *H, U32 *L, U32 *R2)
{
	U64 M = (U64)a * b;
	if(M >> 63)
	{
		(*R2)++;
	}
	*H = (U32)(M >> 31);
	*L = (U32)(M << 1);
}

RET BigNum_clr(BigNum *n)
{
	S32 i = (S32)n->len - 1;

	if(n == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	//�����޹ؿռ�
	memset(n->data + n->len, 0, (n->maxLen - n->len) << 2);

	//��ȡ��Ч����
	while(i >= 0 && n->data[i] == 0)
	{
		i--;
	}
	n->len = (U32)i + 1;

	return SUCCESS;
}

U32 BigNum_getBitVal(const BigNum *n, U32 i)
{
	U32 x = i >> 5;
	U32 y = 1 << (i & 31);

	if(n == NULL)
	{
		return (U32)NULL_POINTER_ERROR;
	}

	if(x >= n->len)
	{
		return 0;
	}

	return (n->data[x] & y) ? 1 : 0;
}

U32 BigNum_getBitLen(const BigNum *n)
{
	S32 i = n->len - 1;

	if(n == NULL)
	{
		return 0;
	}

	while(i >= 0 && n->data[i] == 0)
	{
		i--;
	}
	if(i < 0)
	{
		return 0;
	}

	return ((i << 5) + U32_getBitLen(n->data[i]));

#if 0
	S32 i = n->len - 1;
	U32 j = 0, tmpData;
	while(i >= 0 && n->data[i] == 0)
	{
		i--;
	}
	if(i < 0)
	{
		return 0;
	}
	tmpData = n->data[i];
	while(tmpData)
	{
		tmpData >>= 1;
		j++;
	}
	return ((i << 5) + j);
#endif
}

S32 BigNum_cmp(const BigNum *a, const BigNum *b)
{
	S32 i;

	if(a == NULL || b == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(a->len > b->len)
	{
		return 1;
	}
	if(b->len > a->len)
	{
		return -1;
	}
	for(i = a->len - 1; i >= 0; i--)
	{
		if(a->data[i] > b->data[i])
		{
			return 1;
		}
		if(a->data[i] < b->data[i])
		{
			return -1;
		}
	}
	return 0;
}

RET BigNum_cpy(const BigNum *src, BigNum *dst)
{
	if(src == NULL || dst == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	dst->len = MIN(src->len, dst->maxLen);
	memcpy(dst->data, src->data, dst->len << 2);
	BigNum_clr(dst);
	return SUCCESS;
}

RET BigNum_setZero(BigNum *n)
{
	if(n == NULL)
	{
		return NULL_POINTER_ERROR;
	}
	memset(n->data, 0, n->maxLen << 2);
	n->len = 0;
	return SUCCESS;
}

#if 1
RET BigNum_add(const BigNum *a, const BigNum *b, BigNum *c)
{
	U32 i;
	U32 carry = 0, tmp, tmpLen;
	const BigNum *swap;

	if(a == NULL || b == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(a->len < b->len)
	{
		swap = a;
		a = b;
		b = swap;
	}

	tmpLen = MIN(b->len, c->maxLen);
	
	for(i = 0; i < tmpLen; i++)
	{
		singleAdd(a->data[i], b->data[i], tmp, carry);
		c->data[i] = tmp;
	}

	tmpLen = MIN(a->len, c->maxLen);

	for(i = b->len; i < tmpLen; i++)
	{
		c->data[i] = a->data[i] + carry;
		if(carry && c->data[i])
		{
			carry = 0;
		}
	}

	if(carry && tmpLen < c->maxLen)
	{
		c->data[tmpLen++] = 1;
	}

	c->len = tmpLen;
	BigNum_clr(c);
	
	return SUCCESS;
}
#else
RET BigNum_add(const BigNum *a, const BigNum *b, BigNum *c)
{
	U32 i;
	U32 carry = 0;
	U32 tmp[MAXWORDLEN + 1];// = {0};
	U32 tmpLen = a->len <= b->len ? a->len : b->len;
	
	for(i = 0; i < tmpLen; i++)
	{
		singleAdd(a->data[i], b->data[i], tmp[i], carry);
	}

	for(i = b->len; i < a->len; i++)
	{
		tmp[i] = a->data[i] + carry;
		if(carry && tmp[i])
		{
			carry = 0;
		}
	}
	for(i = a->len; i < b->len; i++)
	{
		tmp[i] = b->data[i] + carry;
		if(carry && tmp[i])
		{
			carry = 0;
		}
	}

	if(carry)
	{
		tmp[i++] = 1;
	}

	//c->len = i <= c->maxLen ? i : c->maxLen;
	c->len = MIN(i, c->maxLen);
	memcpy(c->data, tmp, c->len << 2);
	BigNum_clr(c);
	
	return SUCCESS;
}
#endif

RET BigNum_signedAdd(const BigNum *a, U32 aSign, const BigNum *b, U32 bSign, BigNum *c, U32 *cSign)
{
	if(aSign == bSign)
	{
		*cSign = aSign;
		return BigNum_add(a, b, c);
	}
	else if(BigNum_cmp(a, b) >= 0)
	{
		*cSign = aSign;
		return BigNum_sub(a, b, c);
	}
	else
	{
		*cSign = bSign;
		return BigNum_sub(b, a, c);
	}
}

RET BigNum_addOne(const BigNum *a, BigNum *c)
{
	U32 i;

	if(a == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	c->len = MIN(a->len, c->maxLen);

	for(i = 0; i < c->len; i++)
	{
		c->data[i] = a->data[i] + 1;
		if(c->data[i])
		{
			break;
		}
	}
	
	if(i == c->len)
	{
		if(c->len < c->maxLen)
		{
			c->data[c->len++] = 1;
		}
	}
	else if(i < c->len)
	{
		i++;
		memcpy(c->data + i, a->data + i, (c->len - i) << 2);
	}
	else
	{
		return BIGNUM_ERROR;
	}

	BigNum_clr(c);

	return SUCCESS;
}

#if 1
RET BigNum_sub(const BigNum *a, const BigNum *b, BigNum *c)
{
	U32 i;
	U32 borrow = 0, tmp, tmpLen;
	S32 ret;

	if(a == NULL || b == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	ret = BigNum_cmp(a, b);
	if(ret <= 0)
	{
		BigNum_setZero(c);
		return (ret == 0 ? SUCCESS : BIGNUM_ERROR);
	}
	
	tmpLen = MIN(b->len, c->maxLen);

	for(i = 0; i < tmpLen; i++)
	{
		singleSub(a->data[i], b->data[i], tmp, borrow);
		c->data[i] = tmp;
	}

	tmpLen = MIN(a->len, c->maxLen);

	for(i = b->len; i < tmpLen; i++)
	{
		c->data[i] = a->data[i] - borrow;
		if(borrow && c->data[i] != (U32)-1)
		{
			borrow = 0;
		}
	}

	c->len = tmpLen;
	BigNum_clr(c);

	return SUCCESS;
}
#else
RET BigNum_sub(const BigNum *a, const BigNum *b, BigNum *c)
{
	U32 i;
	U32 borrow = 0;
	U32 tmp[MAXWORDLEN];// = {0};
	S32 ret;

	ret = BigNum_cmp(a, b);
	if(ret <= 0)
	{
		BigNum_setZero(c);
		return (ret == 0 ? SUCCESS : BIGNUM_ERROR);
	}
	
	for(i = 0; i < b->len; i++)
	{
		singleSub(a->data[i], b->data[i], tmp[i], borrow);
	}

	for(i = b->len; i < a->len; i++)
	{
		tmp[i] = a->data[i] - borrow;
		if(borrow && a->data[i])
		{
			borrow = 0;
		}
	}

	c->len = c->maxLen <= a->len ? c->maxLen : a->len;
	memcpy(c->data, tmp, c->len << 2);
	BigNum_clr(c);

	return SUCCESS;
}
#endif

RET BigNum_signedSub(const BigNum *a, U32 aSign, const BigNum *b, U32 bSign, BigNum *c, U32 *cSign)
{
	if(aSign == bSign)
	{
		if(BigNum_cmp(a, b) >= 0)
		{
			*cSign = aSign;
			return BigNum_sub(a, b, c);
		}
		else
		{
			*cSign = !aSign;
			return BigNum_sub(b, a, c);
		}
	}
	else
	{
		*cSign = aSign;
		return BigNum_add(a, b, c);
	}
}

RET BigNum_subOne(const BigNum *a, BigNum *c)
{
	U32 i = 0;
	U32 tmpLen;

	if(a == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(a->len == 0)
	{
		c->len = 0;
		BigNum_clr(c);
		return BIGNUM_ERROR;
	}

	tmpLen = MIN(a->len, c->maxLen);

	for(i = 0; i < tmpLen; i++)
	{
		c->data[i] = a->data[i] - 1;
		if(c->data[i] != (U32)-1)
		{
			break;
		}
	}

	if(i >= a->len)
	{
		c->len = 0;
		BigNum_clr(c);
		return BIGNUM_ERROR;
	}
	else if(i < c->len)
	{
		i++;
		memcpy(c->data + i, a->data + i, (c->len - i) << 2);
	}

	c->len = tmpLen;
	BigNum_clr(c);

	return SUCCESS;
}

RET BigNum_mul(const BigNum *a, const BigNum *b, BigNum *c)
{
	U32 k, i, j;
	U32 R0 = 0, R1 = 0, R2 = 0, U, V, carry;
	U32 tmp[MAXWORDLEN << 1];// = {0};
	U32 tmpLen = a->len + b->len - 1;

	if(a == NULL || b == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(a->len == 0 || b->len == 0)
	{
		BigNum_setZero(c);
		return SUCCESS;
	}

	//���������ҵ���ɨ�裨��λ����λ��
	for(k = 0; k < tmpLen; k++)
	{
		i = k < b->len ? 0 : (k - b->len + 1);
		j = k - i;
		while(i < a->len && j < b->len)
		{
			carry = 0;
			singleMul(a->data[i], b->data[j], &U, &V);
			singleAdd(R0, V, R0, carry);
			singleAdd(R1, U, R1, carry);
			R2 += carry;
			i++;
			j--;
		}
		tmp[k] = R0;
		R0 = R1;
		R1 = R2;
		R2 = 0;
	}

	tmp[tmpLen++] = R0;

	c->len = MIN(tmpLen, c->maxLen);//c->maxLen <= tmpLen ? c->maxLen : tmpLen;
	memcpy(c->data, tmp, c->len << 2);
	BigNum_clr(c);

	return SUCCESS;
}

RET BigNum_squ(const BigNum *a, BigNum *c)
{
	U32 k, i, j;
	U32 R0 = 0, R1 = 0, R2 = 0, U, V, carry;
	U32 tmp[MAXWORDLEN << 1];// = {0};
	U32 tmpLen = (a->len << 1) - 1;

	if(a == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(a->len == 0)
	{
		BigNum_setZero(c);
		return SUCCESS;
	}

	//���������ҵ���ɨ��
	for(k = 0; k < tmpLen; k++)
	{
		i = k < a->len ? 0 : (k - a->len + 1);
		j = k - i;
		while(i < a->len && j < a->len && i <= j)
		{
			carry = 0;
			if(i < j)
			{
				singleSqu(a->data[i], a->data[j], &U, &V, &R2);
			}
			else
			{
				singleMul(a->data[i], a->data[j], &U, &V);
			}
			singleAdd(R0, V, R0, carry);
			singleAdd(R1, U, R1, carry);
			R2 += carry;
			i++;
			j--;
		}
		tmp[k] = R0;
		R0 = R1;
		R1 = R2;
		R2 = 0;
	}

	tmp[tmpLen++] = R0;

	c->len = MIN(tmpLen, c->maxLen);//c->maxLen <= tmpLen ? c->maxLen : tmpLen;
	memcpy(c->data, tmp, c->len << 2);
	BigNum_clr(c);

	return SUCCESS;
}

#if 1
RET BigNum_shl(const BigNum *a, U32 len, BigNum *c)
{
	S32 i;
	U32 wordLen = len >> 5, bitLen = len & 31;

	if(a == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(bitLen)
	{
		if(c->maxLen > a->len + wordLen)
		{
			c->len = a->len + wordLen + 1;
			c->data[c->len - 1] = 0;
		}
		else
		{
			c->len = c->maxLen;
			c->data[c->len - 1] = a->data[c->len - wordLen - 1] << bitLen;
		}

		for(i = c->len - wordLen - 2; i >= 0; i--)
		{
			c->data[i + wordLen + 1] |= a->data[i] >> (32 - bitLen);
			c->data[i + wordLen] = a->data[i] << bitLen;
		}
	}
	else
	{
		c->len = MIN(a->len + wordLen, c->maxLen);
		for(i = c->len - wordLen - 1; i >= 0; i--)
		{
			c->data[i + wordLen] = a->data[i];
		}
	}

	if(c->maxLen > wordLen)
	{
		memset(c->data, 0, wordLen << 2);
	}

	BigNum_clr(c);

	return SUCCESS;
}
#else
RET BigNum_shl(const BigNum *a, U32 len, BigNum *c)
{
	S32 i;
	U32 wLen = len >> 5, bLen = len & 31;
	U32 tmp[MAXWORDLEN << 1];// = {0};

	if(bLen)
	{
		tmp[wLen + a->len] = 0;
		for(i = a->len - 1; i >= 0; i--)
		{
			tmp[wLen + i + 1] |= a->data[i] >> (32 - bLen);
			tmp[wLen + i] = a->data[i] << bLen;
		}
		c->len = c->maxLen <= (a->len + wLen + 1) ? c->maxLen : (a->len + wLen + 1);
	}
	else
	{
		for(i = a->len - 1; i >= 0; i--)
		{
			tmp[wLen + i] = a->data[i];
		}
		c->len = c->maxLen <= (a->len + wLen) ? c->maxLen : (a->len + wLen);
	}

	memset(c->data, 0, wLen << 2);
	memcpy(c->data + wLen, tmp + wLen, (c->len - wLen) << 2);
	BigNum_clr(c);

	return SUCCESS;
}
#endif

#if 1
RET BigNum_shr(const BigNum *a, U32 len, BigNum *c)
{
	U32 i;
	U32 wordLen = len >> 5, bitLen = len & 31, tmpLen;
	
	if(a == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(c->maxLen == 0)
	{
		return BIGNUM_ERROR;
	}

	if(len >= BigNum_getBitLen(a))
	{
		BigNum_setZero(c);
		return SUCCESS;
	}

	tmpLen = MIN(a->len - wordLen, c->maxLen);

	if(bitLen)
	{
		c->data[0] = a->data[wordLen] >> bitLen;
		for(i = 1; i < tmpLen; i++)
		{
			c->data[i - 1] |= a->data[i + wordLen] << (32 - bitLen);
			c->data[i] = a->data[i + wordLen] >> bitLen;
		}
		if(tmpLen < a->len - wordLen)
		{
			c->data[c->len - 1] |= a->data[c->len + wordLen] << (32 - bitLen);
		}
	}
	else
	{
		for(i = wordLen; i < tmpLen; i++)
		{
			c->data[i] = a->data[i + wordLen];
		}
	}
	
	c->len = tmpLen;
	BigNum_clr(c);

	return SUCCESS;
}
#else
RET BigNum_shr(const BigNum *a, U32 len, BigNum *c)
{
	U32 i;
	U32 wLen = len >> 5, bLen = len & 31;
	U32 tmp[MAXWORDLEN];// = {0};
	
	if(bLen)
	{
		tmp[0] = a->data[wLen] >> bLen;
		for(i = wLen + 1; i < a->len; i++)
		{
			tmp[i - wLen - 1] |= a->data[i] << (32 - bLen);
			tmp[i - wLen] = a->data[i] >> bLen;
		}
	}
	else
	{
		for(i = wLen; i < a->len; i++)
		{
			tmp[i - wLen] = a->data[i];
		}
	}

	c->len = c->maxLen <= (a->len - wLen) ? c->maxLen : (a->len - wLen);
	memcpy(c->data, tmp, c->len << 2);
	BigNum_clr(c);

	return SUCCESS;
}
#endif

static void BigNum_div_word(const BigNum *a, U32 b, U32 *q, U32 *r)
{
	S32 i;
    U64 dblA, tmpR = 0;
	for(i = a->len - 1; i >= 0; i--)
    {
        dblA = (tmpR << 32) | a->data[i];
        q[i] = (U32)(dblA / b);
        tmpR = dblA % b;
    }
    *r = (U32)tmpR;
}

RET BigNum_div(const BigNum *a, const BigNum *b, BigNum *q, BigNum *r)
{
	BigNum tr[1];
	U32 tqData[MAXWORDLEN], trData[MAXWORDLEN + 1];
	U32 tmpWordLen, qWordLen = a->len - b->len, ret;
	U32 x, y;
	S32 i;

	if(a == NULL || b == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(b->len == 0)
	{
		q->len = 0;
		BigNum_clr(q);
		r->len = 0;
		BigNum_clr(r);
		return BIGNUM_ERROR;
	}

	if(b->len == 1)
	{
		BigNum_div_word(a, b->data[0], tqData, trData);
		if(q != NULL)
		{
			q->len = q->maxLen <= a->len ? q->maxLen : a->len;
			memcpy(q->data, tqData, q->len << 2);
			BigNum_clr(q);
		}
		if(r != NULL && r->maxLen)
		{
			r->len = 1;
			r->data[0] = trData[0];
			BigNum_clr(r);
		}
		return SUCCESS;
	}

	ret = BigNum_cmp(a, b);
	if(ret == -1)
	{
		if(q != NULL)
		{
			q->len = 0;
			BigNum_clr(q);
		}
		if(r != NULL && a != r)
		{
			r->len = r->maxLen <= a->len ? r->maxLen : a->len;
			memcpy(r->data, a->data, r->len << 2);
			BigNum_clr(r);
		}
		return SUCCESS;
	}
	else if(ret == 0)
	{
		if(q != NULL && q->maxLen)
		{
			q->len = 1;
			q->data[0] = 1;
			BigNum_clr(q);
		}
		if(r != NULL)
		{
			r->len = 0;
			BigNum_clr(r);
		}
		return SUCCESS;
	}

	memset(tqData, 0, qWordLen << 2);

	tr->data = trData;
	tr->maxLen = b->len + 1;

	//��֤ tr ���س��Ȳ����� b
	if(U32_getBitLen(a->data[a->len - 1]) > U32_getBitLen(b->data[b->len - 1]))
	{
		tr->len = b->len - 1;
		tqData[qWordLen] = 0;
		qWordLen++;
	}
	else
	{
		tr->len = b->len;
	}
	tmpWordLen = qWordLen;
	memcpy(trData, a->data + tmpWordLen, tr->len << 2);

	//���� tr ���س��Ȳ����� b, ���� tr - b < b
	if(BigNum_cmp(tr, b) >= 0)
	{
		BigNum_sub(tr, b, tr);
		tqData[tmpWordLen] = 1;
		qWordLen++;
	}

	for(i = (S32)(tmpWordLen << 5) - 1; i >= 0 ; i--)
	{
		BigNum_shl(tr, 1, tr);
		x = i >> 5;
		y = 1 << (i & 31);
		if(a->data[x] & y)
		{
			tr->data[0] |= 1;
		}
		if(BigNum_cmp(tr, b) >= 0)
		{
			BigNum_sub(tr, b, tr);
			tqData[x] |= y;
		}
	}
	if(q != NULL)
	{
		q->len = q->maxLen <= qWordLen ? q->maxLen : qWordLen;
		memcpy(q->data, tqData, q->len << 2);
		BigNum_clr(q);
	}
	if(r != NULL)
	{
		r->len = r->maxLen <= tr->len ? r->maxLen : tr->len;
		memcpy(r->data, trData, r->len << 2);
		BigNum_clr(r);
	}
	return SUCCESS;
}


#undef singleAdd
#undef singleSub

