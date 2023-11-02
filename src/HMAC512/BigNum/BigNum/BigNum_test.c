// Copyright © 2023 Antalpha
//
// This file is part of Antalpha. The full Antalpha copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "BigNum.h"

int BigNum_test()
{
#if 1
extern RET BigNum_mont_FIPS_mul(const BigNum_mont_ctx *ctx, const BigNum *ma, const BigNum *mb, BigNum *mc);
	U32 SM2_p_data[8] = {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE};
	U32 SM2_a_data[8] = {0xFFFFFFFC, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE};
	U32 SM2_b_data[8] = {0x4D940E93, 0xDDBCBD41, 0x15AB8F92, 0xF39789F5, 0xCF6509A7, 0x4D5A9E4B, 0x9D9F5E34, 0x28E9FA9E};
	U32 SM2_n_data[8] = {0x39D54123, 0x53BBF409, 0x21C6052B, 0x7203DF6B, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE};
	U32 c_data[8], d_data[8];

	BigNum p[1] = {SM2_p_data, 8, 8}, a[1] = {SM2_a_data, 8, 8}, b[1] = {SM2_b_data, 8, 8}, n[1] = {SM2_n_data, 8, 8};
	BigNum c[1] = {c_data, 8, 0}, d[1] = {d_data, 8, 0};
	BigNum_mont_ctx ctx[1];

	BigNum_mont_set(ctx, n);

	BigNum_mont_start(ctx, a, a);
	BigNum_mont_start(ctx, b, b);

	BigNum_mont_mul(ctx, a, b, c);
	BigNum_mont_FIPS_mul(ctx, a, b, d);

	BigNum_mont_finish(ctx, c, c);
	BigNum_mont_finish(ctx, d, d);

	BigNum_prt(c, "c");
	BigNum_prt(d, "d");



#endif


#if 0
	U32 data1[8] = {0, 0, 0, 0, 0, 0, 0, 0xFFF00000};
	U32 data2[8] = {0x00000001, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
	U32 data3[9];
	BigNum a[1] = {data1, 8, 8};
	BigNum b[1] = {data2, 8, 7};
	BigNum c[1] = {data3, 8, 0};

	BigNum_shr(a, 32, c);
	BigNum_prt(c, 0);
	BigNum_shr(a, 32, a);
	BigNum_prt(a, 0);
	
#endif

#if 0
	BigNum a[1], b[1], c[1], d[1];
	U32 ad[8] = {0, 0, 2, 0, 0, 0, 0, 0};
	U32 bd[8] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE};
	U32 cd[9] = {0};
	U32 dd[8] = {0};
	a->maxLen = 8;
	a->len = 3;
	b->maxLen = 8;
	b->len = 8;
	c->maxLen = 9;
	c->len = 0;
	a->data = ad;
	b->data = bd;
	c->data = cd;
	d->data = dd;
	d->maxLen = 8;

	BigNum_div(b, a, c);
	BigNum_prt(c, "c");
	BigNum_mod(b, a, d);
	BigNum_prt(d, "d");
	BigNum_div2(b, a, c, d);
	BigNum_prt(c, "c");
	BigNum_prt(d, "d");
	//BigNum_sub1(a, c);


#endif
	
#if 0
	U32 i;
	BigNum k[1];
	U32 kD[8] = {0};
	k->data = kD;
	k->maxLen = 8;
	k->len = 8;

	for(i = 0; i < 100; i++)
	{
		U32_getRand(k->data, 8);
		BigNum_prt(k, "k");
	}
#endif

	
#if 0
	U32 ad[32] = {0xDAFFBCCB,0x2117075D,0xABCF79DF,0xA9247FAC,0x17AB96C9,0x8DA784AB,0x4A185FD2,0x01CBF832,0xF6D5B4B7,0x91F184E9,0x217F7143,0x86C1EBA5,0xCF9CFD43,0x758021DF,0xAB7462B2,0xB7A7E9D8,0x44526A8D,0xAACA6606,0x4E3D16F5,0x4BF4D995,0x4E876938,0x8E02BF88,0xA3449C2D,0xA787B8FB,0x84E6B194,0x7DF1A79F,0x0704FEE7,0xC7EF3F46,0x063064C9,0x9FBB4091,0xFD83592E,0xE92EBBB0};
	U32 bd[32] = {0xDCAA9769,0x93C2720A,0x920ADC30,0x6BCCD811,0x60D7C689,0x72268624,0x9EB97615,0xE7A6D3EC,0x8377EE50,0xBC76F577,0xD20757E7,0xE02C44E5,0x8BD59D59,0x57C8DC33,0x5E5837DF,0x63E0ED59,0xA890A59C,0x72E4497F,0xA52FE6B5,0x6BE94DB7,0x378207AA,0x3B1D8CD6,0x1D5B725A,0xFDB9C6AE,0x50BBC28B,0x00049377,0xA0DA399C,0x9A26FB49,0xD71F7702,0x5BF25B98,0x8471BAB2,0xE07C6182};
	U32 nd[64] = {0};
	U32 ed[64] = {0x49449C8B,0x1E913E8E,0x49416419,0xF1AE2C93,0xD6AE3696,0xAF757B84,0xEBD637A9,0x4E3893BA,0x8BD399D5,0x744CD6FA,0x3117F851,0xBB5EC4DC,0x85398758,0x91D5C4F7,0xC0B16C48,0xF99468EE,0xEAA44342,0x1BF6AD2B,0xFC611A37,0x3C339353,0x703235B2,0x9A54DC45,0xCF1B8474,0xF8B23658,0x1AB92BAA,0x60848321,0x1566519F,0xAC81DCD9,0x90C3A6E2,0x84715292,0x64302E21,0x845050DB,0xC64FA46B,0x750567CD,0xF911EF51,0x6FDE3CFA,0x042CBE48,0xA9F6B24D,0xA1D0AA1B,0x40810D57,0x02A0877E,0x93F7D378,0xA63E2B8E,0xC9099054,0x8652AE72,0x3B379E89,0xE035FEA6,0x34BCDE5E,0xD5199011,0xE74A4C31,0xA2E047E3,0xF609BC09,0xF115D197,0xA81D9FD4,0xAB55EBCF,0x9A6BDC83,0x7C384222,0xBDCBF1EB,0xDF34FFC6,0x594591B2,0x86F8E30A,0x2DF5A83C,0x1DD45E8F,0xAE0C1AD6};
	U32 dd[64] = {0};
	BigNum a[1] = {ad, 32, 32}, b[1] = {bd, 32, 32}, n[1] = {nd, 64, 0}, e[1] = {ed, 64, 64}, d[1] = {dd, 64, 0};
	
	BigNum_subOne(a, a);
	BigNum_prt(a, "a");
	BigNum_subOne(b, b);
	BigNum_prt(b, "b");
	BigNum_mul(a, b, n);
	BigNum_prt(n, "n");

	BigNum_modInv(e, n, d);
	BigNum_prt(d, "d");
	//BigNum_modInv(b, a, c);

	//BigNum_prt(c, 0);

	/*BigNum_modMul(c, n, p, c);

	BigNum_prt(c, 0);*/

#endif

#if 0

	BigNum_mont_ctx ctx[1];
	BigNum a[1], b[1], c[1], k[1];
	U32 aData[16] = {0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE};
	U32 bData[8] = {0}, cData[8] = {0};
	U32 kData[5] = {0xFDACB324, 0xFDACB324, 0xFDACB324, 0xFDACB324, 0xFDACB324};
	U32 i;

	a->data = aData;
	a->maxLen = 16;
	//memcpy(aData, SM2_a->data, 32);
	a->len = 16;

	b->data = bData;
	b->maxLen = 8;
	memcpy(bData, SM2_b->data, 32);
	b->len = 8;

	c->data = cData;
	c->maxLen = 8;
	c->len = 0;

	k->data = kData;
	k->maxLen = 5;
	k->len = 5;
	
	BigNum_prt(a, "a");
	BigNum_prt(b, "b");
	BigNum_prt(c, "c");
	for(i = 0; i < 1; i++)
	{
		BigNum_mont_set(ctx, SM2_p);
	}
	BigNum_mont_start(ctx, a, a);
	BigNum_mont_start(ctx, b, b);

	for(i = 0; i < 10000; i++)
	{
		BigNum_mont_mul(ctx, a, b, c);
		//BigNum_modMul(a, b, SM2_n, c);
	}

	BigNum_mont_exp(ctx, a, k, c);

	BigNum_mont_finish(ctx, a, a);
	BigNum_mont_finish(ctx, b, b);
	BigNum_mont_finish(ctx, c, c);
	BigNum_prt(a, "a");
	BigNum_prt(b, "b");
	BigNum_prt(c, "c");

	BigNum_modExp(a, k, SM2_p, c);
	BigNum_prt(c, "c");

	BigNum_setZero(c);
	BigNum_prt(c, "0");
	BigNum_mont_start(ctx, c, c);
	BigNum_prt(c, "0");

	/*c->len = 0;
	BigNum_clr(c);
	BigNum_prt(c, "c");

	BigNum_modMul(a, b, SM2_n, c);
	BigNum_prt(c, "c");


	BigNum_prt(k, "k");
	BigNum_mont_set(ctx, SM2_n);
	BigNum_mont_start(ctx, k, a);
	BigNum_mont_finish(ctx, a, b);
	BigNum_prt(b, "k");
	BigNum_mont_mul(ctx, a, a, a);
	BigNum_mont_finish(ctx, a, a);
	BigNum_prt(a, "kk");

	BigNum_modMul(k, k, SM2_n, c);
	BigNum_prt(c, "kk");*/

#endif

#if 0
	/*U32 data[32] = {0xA22F7797,0x8CDE59EB,0x14DAF1CC,0xB0E38FE1,0x6EBCDF51,0x8F4B08D9,0xB46FB71D,0xE8229DBC,0xB5A12536,0x212A6E82,0xE17A863F,0x053E406A,0x4D8AA809,0xB5CA9184,0x1403D691,0xBCBE37E1,0x32303A36,0xFE1E2D0B,0x4F363706,0x1FF4968E,0xD54CA67A,0xC5F342B5,0x94AF3BF1,0x5BC83CD2,0x07C179D0,0x05C75EA7,0x4AE9A00F,0x1314ECC1,0xF8D03E9B,0x1F3F69EF,0xA4CEE4D5,0x5BB4F219};
	U32 rdata[32] = {0xA22F7796,0x8CDE59EB,0x14DAF1CC,0xB0E38FE1,0x6EBCDF51,0x8F4B08D9,0xB46FB71D,0xE8229DBC,0xB5A12536,0x212A6E82,0xE17A863F,0x053E406A,0x4D8AA809,0xB5CA9184,0x1403D691,0xBCBE37E1,0x32303A36,0xFE1E2D0B,0x4F363706,0x1FF4968E,0xD54CA67A,0xC5F342B5,0x94AF3BF1,0x5BC83CD2,0x07C179D0,0x05C75EA7,0x4AE9A00F,0x1314ECC1,0xF8D03E9B,0x1F3F69EF,0xA4CEE4D5,0x5BB4F219};
	U32 tmp[32] = {0};
	BigNum n[1] = {data, 32, 32};
	BigNum r[1] = {rdata, 32, 32};
	BigNum t[1] = {tmp, 32, 0};
	U32 i;
	//BigNum_mont_ctx ctx[1];
	for(i = 0; i < 10; i++)
	{
		BigNum_modExp(r, r, n, t);
	}*/
	primeTest();
#endif



	return 0;
}