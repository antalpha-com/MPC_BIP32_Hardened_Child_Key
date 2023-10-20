#ifndef __HMAC512_CIRCUIT_H_
#define __HMAC512_CIRCUIT_H_

#include <abycore/circuit/booleancircuits.h>
#include <abycore/circuit/arithmeticcircuits.h>
#include <abycore/circuit/circuit.h>
#include <abycore/aby/abyparty.h>
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <cassert>
#include "../B2A/B2Aparty.h"
#include "../B2A/B2Asharing.h"


class BooleanCircuit;
//class ArithmeticCircuit;

// defining constants
#define ABY_HMAC512_MAXUINT32   2147483647      // 2^31-1
#define ABY_HMAC512_MAXUINT64   18446744073709551615    // 2^64-1
#define ABY_HMAC512_BlOCKSIZE   1024     // SHA512BlOCKSIZE
#define ABY_HMAC512_BlOCKSIZE_BYTES   ABY_HMAC512_BlOCKSIZE/8


#define ABY_HMAC512_PADDING   896

#define ABY_HMAC512_OUTPUT_BITS 512
#define ABY_HMAC512_OUTPUT_BYTES ABY_HMAC512_OUTPUT_BITS/8


const uint64_t ABY_HMAC512_H0 = 0x6a09e667f3bcc908;
const uint64_t ABY_HMAC512_H1 = 0xbb67ae8584caa73b;
const uint64_t ABY_HMAC512_H2 = 0x3c6ef372fe94f82b;
const uint64_t ABY_HMAC512_H3 = 0xa54ff53a5f1d36f1;
const uint64_t ABY_HMAC512_H4 = 0x510e527fade682d1;
const uint64_t ABY_HMAC512_H5 = 0x9b05688c2b3e6c1f;
const uint64_t ABY_HMAC512_H6 = 0x1f83d9abfb41bd6b;
const uint64_t ABY_HMAC512_H7 = 0x5be0cd19137e2179;

const uint64_t ConstantSHA512[80]={
    	0x428a2f98d728ae22,
		0x7137449123ef65cd,
		0xb5c0fbcfec4d3b2f,
		0xe9b5dba58189dbbc,
		0x3956c25bf348b538,
		0x59f111f1b605d019,
		0x923f82a4af194f9b,
		0xab1c5ed5da6d8118,
		0xd807aa98a3030242,
		0x12835b0145706fbe,
		0x243185be4ee4b28c,
		0x550c7dc3d5ffb4e2,
		0x72be5d74f27b896f,
		0x80deb1fe3b1696b1,
		0x9bdc06a725c71235,
		0xc19bf174cf692694,
		0xe49b69c19ef14ad2,
		0xefbe4786384f25e3,
		0x0fc19dc68b8cd5b5,
		0x240ca1cc77ac9c65,
		0x2de92c6f592b0275,
		0x4a7484aa6ea6e483,
		0x5cb0a9dcbd41fbd4,
		0x76f988da831153b5,
		0x983e5152ee66dfab,
		0xa831c66d2db43210,
		0xb00327c898fb213f,
		0xbf597fc7beef0ee4,
		0xc6e00bf33da88fc2,
		0xd5a79147930aa725,
		0x06ca6351e003826f,
		0x142929670a0e6e70,
		0x27b70a8546d22ffc,
		0x2e1b21385c26c926,
		0x4d2c6dfc5ac42aed,
		0x53380d139d95b3df,
		0x650a73548baf63de,
		0x766a0abb3c77b2a8,
		0x81c2c92e47edaee6,
		0x92722c851482353b,
		0xa2bfe8a14cf10364,
		0xa81a664bbc423001,
		0xc24b8b70d0f89791,
		0xc76c51a30654be30,
		0xd192e819d6ef5218,
		0xd69906245565a910,
		0xf40e35855771202a,
		0x106aa07032bbd1b8,
		0x19a4c116b8d2d0c8,
		0x1e376c085141ab53,
		0x2748774cdf8eeb99,
		0x34b0bcb5e19b48a8,
		0x391c0cb3c5c95a63,
		0x4ed8aa4ae3418acb,
		0x5b9cca4f7763e373,
		0x682e6ff3d6b2b8a3,
		0x748f82ee5defb2fc,
		0x78a5636f43172f60,
		0x84c87814a1f0ab72,
		0x8cc702081a6439ec,
		0x90befffa23631e28,
		0xa4506cebde82bde9,
		0xbef9a3f7b2c67915,
		0xc67178f2e372532b,
		0xca273eceea26619c,
		0xd186b8c721c0c207,
		0xeada7dd6cde0eb1e,
		0xf57d4f7fee6ed178,
		0x06f067aa72176fba,
		0x0a637dc5a2c898a6,
		0x113f9804bef90dae,
		0x1b710b35131c471b,
		0x28db77f523047d84,
		0x32caab7b40c72493,
		0x3c9ebe0a15c9bebc,
		0x431d67c49c100d4c,
		0x4cc5d4becb3e42b6,
		0x597f299cfc657e2a,
		0x5fcb6fab3ad6faec,
		0x6c44198c4a475817,
};

// test the hmac circuit
uint8_t* test_hmac512_circuit(e_role role, const std::string &address, uint16_t port, seclvl seclvl, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, uint32_t bitlen, std::string skey, std::string sdata, uint32_t ser);

// Convert string to bytes
uint8_t* str2byte(std::string str, uint32_t len);

// Determine whether the keys and indexs inputted by both parties are equal
share* judge(e_role role, BooleanCircuit *circ, uint8_t* key, uint32_t ser);

// Construct data
share *cat_data(uint8_t *data, uint32_t data_len, uint32_t ser, BooleanCircuit *bcirc, BooleanCircuit *ycirc, e_role role);

// Construct hmac512 routines, Steps are taken from the RFC 6234
share* hmac512_routine(uint8_t *key, uint32_t keylen, share *s_data, uint32_t datalen, BooleanCircuit *ycirc, ABYParty *party);

uint8_t* keypadding(uint8_t* key, uint32_t keylen);

share* CatAndHash(uint8_t* key, uint32_t kaylen, share* s_data, uint32_t datalen, BooleanCircuit* circ);

share* BuildSHA512Circuit(share* s_msg, BooleanCircuit* circ);

share* process_block(share* s_msg, share** s_H, share** s_k, BooleanCircuit* circ);

void init_variables(share** s_H, share** s_k, BooleanCircuit* circ);

void break_message_to_chunks(share** s_w, share* s_msg, BooleanCircuit* circ);

void expand_ws(share** s_w, BooleanCircuit* circ);

void sha512_main_loop(share** s_H, share** s_w, share** s_k, BooleanCircuit* circ);

uint8_t* padding(std::string data);

share* SSIG1(share* s_x, BooleanCircuit* circ);
share* SSIG0(share* s_x, BooleanCircuit* circ);
share* BSIG1(share* s_x, BooleanCircuit* circ);
share* BSIG0(share* s_x, BooleanCircuit* circ);
share* CH(share* s_x, share* s_y, share* s_z, BooleanCircuit* circ);
share* MAJ(share* s_x, share* s_y, share* s_z, BooleanCircuit* circ);

#endif /* __HMAC512_CIRCUIT_H_ */

