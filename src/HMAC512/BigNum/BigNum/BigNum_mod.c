#include "BigNum.h"

/*
RET BigNum_mod(const BigNum *a, const BigNum *n, BigNum *c)
{
	U32 aBitLen, nBitLen, tmpLen;
	BigNum n1[1], a1[1];
	U32 n1Data[MAXWORDLEN] = {0};
	U32 a1Data[MAXWORDLEN] = {0};

	n1->data = n1Data;
	n1->maxLen = MAXWORDLEN;
	memcpy(n1Data, n->data, n->len << 2);
	n1->len = n->len;
	BigNum_clr(n1);

	if(c->maxLen < n1->len)
	{
		return BIGNUM_ERROR;
	}

	a1->data = a1Data;
	a1->maxLen = MAXWORDLEN;
	memcpy(a1Data, a->data, a->len << 2);
	a1->len = a->len;
	BigNum_clr(a1);

	aBitLen = BigNum_getBitLen(a1);
	nBitLen = BigNum_getBitLen(n1);

	if(aBitLen > nBitLen)
	{
		BigNum_shl(n1, aBitLen - nBitLen, n1);
		tmpLen = aBitLen - nBitLen;
		if(BigNum_cmp(a1, n1) >= 0)
		{
			BigNum_sub(a1, n1, a1);
		}
		else
		{
			BigNum_shr(n1, 1, n1);
			tmpLen--;
			BigNum_sub(a1, n1, a1);
		}
		aBitLen = BigNum_getBitLen(a1);
	}

	while(aBitLen > nBitLen)
	{
		BigNum_shr(n1, tmpLen - (aBitLen - nBitLen), n1);
		tmpLen = aBitLen - nBitLen;
		if(BigNum_cmp(a1, n1) >= 0)
		{
			BigNum_sub(a1, n1, a1);
		}
		else
		{
			BigNum_shr(n1, 1, n1);
			tmpLen--;
			BigNum_sub(a1, n1, a1);
		}
		aBitLen = BigNum_getBitLen(a1);
	}
	if(BigNum_cmp(a1, n) >= 0)
	{
		BigNum_sub(a1, n, c);
	}
	else
	{
		c->len = a1->len;
		memcpy(c->data, a1->data, c->len << 2);
		BigNum_clr(c);
	}

	return SUCCESS;
}
*/

RET BigNum_modAdd(const BigNum *a, const BigNum *b, const BigNum *n, BigNum *c)
{
	BigNum tmp[1];
	U32 tmpData[MAXWORDLEN + 1] = {0};

	tmp->data = tmpData;
	tmp->maxLen = MAX(a->len, b->len) + 1;

	BigNum_add(a, b, tmp);
	return BigNum_mod(tmp, n, c);
}

RET BigNum_modSub(const BigNum *a, const BigNum *b, const BigNum *n, BigNum *c)
{
	BigNum tmp[1];
	U32 tmpData[MAXWORDLEN + 1];// = {0};

	tmp->data = tmpData;
	tmp->maxLen = MAX(a->len, n->len) + 1;

	if(BigNum_cmp(a, b) >= 0)
	{
		BigNum_sub(a, b, c);
	}
	else
	{
		BigNum_add(a, n, tmp);
		BigNum_sub(tmp, b, c);
	}
	return SUCCESS;
}

RET BigNum_modMul(const BigNum *a, const BigNum *b, const BigNum *n, BigNum *c)
{
	BigNum tmp[1];
	U32 tmpData[MAXWORDLEN << 1];// = {0};

	tmp->data = tmpData;
	tmp->maxLen = a->len + b->len;
	tmp->len = 0;
	
	BigNum_mul(a, b, tmp);
	return BigNum_mod(tmp, n, c);
}

RET BigNum_modSqu(const BigNum *a, const BigNum *n, BigNum *c)
{
	BigNum tmp[1];
	U32 tmpData[MAXWORDLEN << 1];// = {0};

	tmp->data = tmpData;
	tmp->maxLen = a->len << 1;
	tmp->len = 0;
	
	BigNum_squ(a, tmp);
	return BigNum_mod(tmp, n, c);
}

RET BigNum_modMul2(const BigNum *a, const BigNum *n, BigNum *c)
{
	BigNum tmp[1];
	U32 tmpData[MAXWORDLEN + 1];// = {0};

	tmp->data = tmpData;
	tmp->maxLen = a->len + 1;
	tmp->len = 0;

	BigNum_shl(a, 1, tmp);
	if(BigNum_cmp(tmp, n) >= 0)
	{
		return BigNum_sub(tmp, n, c);
	}
	else
	{
		//c->len = c->maxLen <= tmp->len ? c->maxLen : tmp->len;
		/*c->len = MIN(tmp->len, c->maxLen);
		memcpy(c->data, tmpData, c->len << 2);
		BigNum_clr(c);*/
		//BigNum_cpy(c, tmp);
		BigNum_cpy(tmp, c);
		return SUCCESS;
	}
}

RET BigNum_modDiv2(const BigNum *a, const BigNum *n, BigNum *c)
{
	BigNum tmp[1];
	U32 tmpData[MAXWORDLEN + 1];// = {0};

	tmp->data = tmpData;
	tmp->maxLen = n->len + 1;
	tmp->len = 0;

	if(a->data[0] & 1)
	{
		if((n->data[0] & 1) == 0)
		{
			return EVEN_ERROR;
		}
		BigNum_add(a, n, tmp);
		BigNum_shr(tmp, 1, c);
	}
	else
	{
		BigNum_shr(a, 1, c);
	}
	return SUCCESS;
}

static RET BigNum_modInv_even(const BigNum *a, const BigNum *n, BigNum *c)
{
	U32 aData[MAXWORDLEN], uData[MAXWORDLEN], vData[MAXWORDLEN], AData[MAXWORDLEN + 1], BData[MAXWORDLEN + 1], CData[MAXWORDLEN + 1], DData[MAXWORDLEN + 1];
	U32 aSign = 0, bSign = 0, cSign = 0, dSign = 0;
	BigNum a1[1], u[1], v[1], A[1], B[1], C[1], D[1];
	S32 ret;

	a1->data = aData;
	a1->maxLen = n->len;
	BigNum_mod(a, n, a1);

	u->data = uData;
	u->maxLen = n->len;
	//BigNum_cpy(u, a1);
	BigNum_cpy(a1, u);


	v->data = vData;
	v->maxLen = n->len;
	//BigNum_cpy(v, n);
	BigNum_cpy(n, v);

	A->data = AData;
	A->maxLen = n->len + 1;
	A->data[0] = 1;
	A->len = 1;
	BigNum_clr(A);

	B->data = BData;
	B->maxLen = n->len + 1;
	BigNum_setZero(B);

	C->data = CData;
	C->maxLen = n->len + 1;
	BigNum_setZero(C);

	D->data = DData;
	D->maxLen = n->len + 1;
	D->data[0] = 1;
	D->len = 1;
	BigNum_clr(D);

	while((u->data[0] != 1 || u->len != 1) &&(v->data[0] != 1 || v->len != 1))
	{
		while((u->data[0] & 1) == 0)
		{
			BigNum_shr(u, 1, u);
			if((A->data[0] & 1) || (B->data[0] & 1))
			{
				BigNum_signedAdd(A, aSign,  n, 0, A, &aSign);
				BigNum_signedSub(B, bSign, a1, 0, B, &bSign);
			}
			BigNum_shr(A, 1, A);
			BigNum_shr(B, 1, B);
		}
		while((v->data[0] & 1) == 0)
		{
			BigNum_shr(v, 1, v);
			if((C->data[0] & 1) || (D->data[0] & 1))
			{
				BigNum_signedAdd(C, cSign,  n, 0, C, &cSign);
				BigNum_signedSub(D, dSign, a1, 0, D, &dSign);
			}
			BigNum_shr(C, 1, C);
			BigNum_shr(D, 1, D);
		}

		ret = BigNum_cmp(u, v);
		if(ret > 0)
		{
			BigNum_sub(u, v, u);
			BigNum_signedSub(A, aSign, C, cSign, A, &aSign);
			BigNum_signedSub(B, bSign, D, dSign, B, &bSign);
		}
		else if(ret < 0)
		{
			BigNum_sub(v, u, v);
			BigNum_signedSub(C, cSign, A, aSign, C, &cSign);
			BigNum_signedSub(D, dSign, B, bSign, D, &dSign);
		}
		else
		{
			break;
		}
	}

	if(u->data[0] == 1 && u->len == 1)
	{
		if(aSign)
		{
			BigNum_mod(A, n, A);
			return BigNum_sub(n, A, c);
		}
		else
		{
			return BigNum_mod(A, n, c);
		}
		return SUCCESS;
	}
	else if(v->data[0] == 1 && v->len == 1)
	{
		if(cSign)
		{
			BigNum_mod(C, n, C);
			return BigNum_sub(n, C, c);
		}
		else
		{
			return BigNum_mod(C, n, c);
		}
		return SUCCESS;
	}

	return MODINV_ERROR;
}

static RET BigNum_modInv_odd(const BigNum *a, const BigNum *n, BigNum *c)
{
	U32 uData[MAXWORDLEN], vData[MAXWORDLEN], x1Data[MAXWORDLEN], x2Data[MAXWORDLEN];
	BigNum u[1], v[1], x1[1], x2[1];
	S32 ret;

	u->data = uData;
	u->maxLen = n->len;
	BigNum_mod(a, n, u);

	v->data = vData;
	v->maxLen = n->len;
	//BigNum_cpy(v, n);
	BigNum_cpy(n, v);

	x1->data = x1Data;
	x1->maxLen = n->len;
	x1->data[0] = 1;
	x1->len = 1;
	BigNum_clr(x1);

	x2->data = x2Data;
	x2->maxLen = n->len;
	BigNum_setZero(x2);


	while((u->data[0] != 1 || u->len != 1) &&(v->data[0] != 1 || v->len != 1))
	{
		while((u->data[0] & 1) == 0)
		{
			BigNum_shr(u, 1, u);
			BigNum_modDiv2(x1, n, x1);
		}
		while((v->data[0] & 1) == 0)
		{
			BigNum_shr(v, 1, v);
			BigNum_modDiv2(x2, n, x2);
		}

		ret = BigNum_cmp(u, v);
		if(ret > 0)
		{
			BigNum_sub(u, v, u);
			BigNum_modSub(x1, x2, n, x1);
		}
		else if(ret < 0)
		{
			BigNum_sub(v, u, v);
			BigNum_modSub(x2, x1, n, x2);
		}
		else
		{
			break;
		}
	}

	if(u->data[0] == 1 && u->len == 1)
	{
		BigNum_mod(x1, n, c);
		return SUCCESS;
	}
	else if(v->data[0] == 1 && v->len == 1)
	{
		BigNum_mod(x2, n, c);
		return SUCCESS;
	}
	return MODINV_ERROR;
}

RET BigNum_modInv(const BigNum *a, const BigNum *n, BigNum *c)
{
	if(a == NULL || n == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(a->len == 0 || n->len == 0)
	{
		return ZERO_VALUE_ERROR;
	}

	if(n->data[0] & 1)
	{
		return BigNum_modInv_odd(a, n, c);
	}
	else
	{
		return BigNum_modInv_even(a, n, c);
	}
}

