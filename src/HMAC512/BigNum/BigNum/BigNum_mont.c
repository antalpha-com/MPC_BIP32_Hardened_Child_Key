#include "BigNum.h"

RET BigNum_mont_set(BigNum_mont_ctx *ctx, const BigNum *n)
{
	BigNum r[1], n0[1], rr1[1];
	U32 rData[MAXWORDLEN + 1] = {0};
	U32 i;

	if(ctx == NULL || n == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	i = U32_getBitLen(n->data[n->len - 1]);

	r->data = rData;
	r->maxLen = MAXWORDLEN + 1;
	if(i == 32)
	{
		rData[n->len] = 1;
		r->len = n->len + 1;
	}
	else
	{
		rData[n->len - 1] = 1 << i;
		r->len = n->len;
	}

	rr1->data = ctx->rr;
	rr1->maxLen = MAXWORDLEN;
	rr1->len = 0;

	BigNum_sub(r, n, rr1);

	i = (n->len << 5) + 32 - i;

	while(i)
	{
		BigNum_modMul2(rr1, n, rr1);
		i--;
	}
	//BigNum_prt(rr1, "rrmodn");

	rData[0] = 0;
	rData[1] = 1;
	r->len = 2;

	n0->data = n->data;
	n0->maxLen = 1;
	n0->len = 1;

	BigNum_modInv(r, n0, r);
	BigNum_shl(r, 32, r);
	BigNum_subOne(r, r);
	BigNum_div(r, n0, r, NULL);
	//BigNum_prt(r, "ni");

	ctx->n = (BigNum*)n;
	ctx->ni0 = rData[0];

	return SUCCESS;
}

RET BigNum_mont_mul(const BigNum_mont_ctx *ctx, const BigNum *ma, const BigNum *mb, BigNum *mc)
{
	BigNum t[1];
	U32 tData[(MAXWORDLEN << 1) + 1] = {0};
	U64 CS;
	U32 m, carry;
	U32 i, j;

	if(ctx == NULL || ma == NULL || mb == NULL || mc == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	t->data = tData;
	t->maxLen = (MAXWORDLEN << 1) + 1;
	t->len = 0;

	if(ma != mb)
	{
		BigNum_mul(ma, mb, t);
	}
	else
	{
		BigNum_squ(ma, t);
	}

	for(i = 0; i < ctx->n->len; i++)
	{
		CS = 0;
		m = tData[i] * ctx->ni0;
		for(j = 0; j < ctx->n->len; j++)
		{
			CS = (U64)tData[i + j] + (U64)m * (U64)ctx->n->data[j] + (CS >> 32);
			tData[i + j] = (U32)CS;
		}

		carry = CS >> 32;
		tData[i + ctx->n->len] += carry;
		if(tData[i + ctx->n->len] < carry)
		{
			carry = 1;
			for(j = i + ctx->n->len + 1; j < t->maxLen; j++)
			{
				tData[j] += carry;
				if(tData[j])
				{
					break;
				}
			}
		}
	}
	for(i = 0; i <= ctx->n->len; i++)
	{
		tData[i] = tData[i + ctx->n->len];
	}
	t->len = ctx->n->len + 1;
	BigNum_clr(t);

	if(BigNum_cmp(t, ctx->n) >= 0)
	{
		return BigNum_sub(t, ctx->n, mc);
	}
	else
	{
		mc->len = mc->maxLen <= t->len ? mc->maxLen : t->len;
		memcpy(mc->data, tData, mc->len << 2);
		BigNum_clr(mc);
		return SUCCESS;
	}
}

RET BigNum_mont_exp(const BigNum_mont_ctx *ctx, const BigNum *ma, const BigNum *e, BigNum *mc)
{
	U32 i;
	U32 t0Data[MAXWORDLEN], t1Data[MAXWORDLEN];
	BigNum t0[1], t1[1];
	
	if(ctx == NULL || ma == NULL || e == NULL || mc == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(e->len == 0)
	{
		mc->data[0] = 1;
		mc->len = 1;
		BigNum_clr(mc);
		return SUCCESS;
	}

	memcpy(t0Data, ma->data, ma->len << 2);
	t0->data = t0Data;
	t0->maxLen = ctx->n->maxLen;
	t0->len = ma->len;

	memcpy(t1Data, ma->data, ma->len << 2);
	t1->data = t1Data;
	t1->maxLen = ctx->n->maxLen;
	t1->len = ma->len;

	i = BigNum_getBitLen(e) - 1;

	while(i)
	{
		i--;
		BigNum_mont_mul(ctx, t1, t1, t1);
		if(BigNum_getBitVal(e, i))
		{
			BigNum_mont_mul(ctx, t1, t0, t1);
		}
	}

	mc->len = mc->maxLen <= t1->len ? mc->maxLen : t1->len;
	memcpy(mc->data, t1Data, mc->len << 2);
	BigNum_clr(mc);

	return SUCCESS;
}


RET BigNum_mont_start(const BigNum_mont_ctx *ctx, const BigNum *a, BigNum *ma)
{
	BigNum rr1[1];

	if(ctx == NULL || a == NULL || ma == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	rr1->data = (U32*)ctx->rr;
	rr1->maxLen = MAXWORDLEN;
	rr1->len = ctx->n->len;

	return BigNum_mont_mul(ctx, a, rr1, ma);
}

RET BigNum_mont_finish(const BigNum_mont_ctx *ctx, const BigNum *ma, BigNum *a)
{
	BigNum one[1];
	U32 oneData[1] = {1};

	if(ctx == NULL || ma == NULL || a == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	one->data = oneData;
	one->maxLen = 1;
	one->len = 1;

	return BigNum_mont_mul(ctx, one, ma, a);
}

RET BigNum_modExp(const BigNum *a, const BigNum *e, const BigNum *n, BigNum *c)
{
	BigNum_mont_ctx ctx[1];
	U32 tmpData[MAXWORDLEN];
	BigNum tmp[1];

	if(a == NULL || e == NULL || n == NULL || c == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	tmp->data = tmpData;
	tmp->maxLen = n->len;

	BigNum_mod(a, n, tmp);

	BigNum_mont_set(ctx, n);

	BigNum_mont_start(ctx, tmp, tmp);
	BigNum_mont_exp(ctx, tmp, e, tmp);
	BigNum_mont_finish(ctx, tmp, c);
	
	return SUCCESS;
}


/*****************************************************************************************************************/

//ADD(t[1], C)
#define ADD											\
do													\
{													\
	U32 C = (U32)(CS >> 32);						\
	if((t[1] = t[1] + C) < C)						\
	{												\
		t[2]++;										\
	}												\
}while(0)

static RET FIPS(const U32 *a, const U32 *b, const U32 *n, U32 nWordLen, U32 ni0, U32 *t, U32 *m)
{
	U64 CS;
	U32 i, j;

	for(i = 0; i < nWordLen; i++)
	{
		for(j = 0; j < i; j++)
		{
			CS = t[0] + (U64)a[j] * b[i - j];
			ADD;
			CS = (U32)CS + (U64)m[j] * n[i - j];
			t[0] = (U32)CS;
			ADD;
		}
		CS = t[0] + (U64)a[i] * b[0];
		ADD;
		m[i] = (U32)CS * ni0;
		CS = (U32)CS + (U64)m[i] * n[0];
		ADD;
		t[0] = t[1];
		t[1] = t[2];
		t[2] = 0;
	}

	for(i = nWordLen; i < 2 * nWordLen; i++)
	{
		for(j = i - nWordLen + 1; j < nWordLen; j++)
		{
			CS = t[0] + (U64)a[j] * b[i - j];
			ADD;
			CS = (U32)CS + (U64)m[j] * n[i - j];
			t[0] = (U32)CS;
			ADD;
		}
		m[i - nWordLen] = t[0];
		t[0] = t[1];
		t[1] = t[2];
		t[2] = 0;
	}

	return SUCCESS;
}

#undef ADD

RET BigNum_mont_FIPS_mul(const BigNum_mont_ctx *ctx, const BigNum *ma, const BigNum *mb, BigNum *mc)
{
	U32 tData[MAXWORDLEN + 3] = {0};
	BigNum t[1];

	if(ctx == NULL || ma == NULL || mb == NULL || mc == NULL)
	{
		return NULL_POINTER_ERROR;
	}

	if(ma->maxLen < ctx->n->len || mb->maxLen < ctx->n->len)
	{
		return BIGNUM_ERROR;
	}

	FIPS(ma->data, mb->data, ctx->n->data, ctx->n->len, ctx->ni0, tData + ctx->n->len, tData);

	t->data = tData;
	t->maxLen =ctx->n->len + 1;
	t->len =ctx->n->len + 1;
	BigNum_clr(t);

	if(BigNum_cmp(t, ctx->n) >= 0)
	{
		return BigNum_sub(t, ctx->n, mc);
	}
	else
	{
		return BigNum_cpy(t, mc);
	}
}




