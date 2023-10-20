#include "hmac512_circuit.h"
#include <abycore/circuit/booleancircuits.h>
#include <abycore/sharing/sharing.h>
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <cstring>

uint8_t* test_hmac512_circuit(e_role role, const std::string &address, uint16_t port, seclvl seclvl, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing, uint32_t bitlen, std::string key_s, std::string data_s, uint32_t ser)
{
	// init
	ABYParty *party = new ABYParty(role, address, port, seclvl, 256, nthreads, mt_alg);
	std::vector<Sharing *> &sharings = party->GetSharings();

	Circuit *ycirc = sharings[S_YAO]->GetCircuitBuildRoutine();
	Circuit *bcirc = sharings[S_BOOL]->GetCircuitBuildRoutine();

	// Circuit build routine works for Boolean circuits only right now
	assert(ycirc->GetCircuitType() == C_BOOLEAN);
	assert(bcirc->GetCircuitType() == C_BOOLEAN);
	
	/***********************************************************/
	// Process the input data
	// the size of key should be 256bits
	uint8_t *key = str2byte(key_s, 32);
	// data
	uint8_t *data = str2byte(data_s, 32);

	// test
	std::cout << "key:\t";
	for (uint32_t j = 0; j < 32; j++)
	{
		std::cout << std::hex << ((key[j] & 0xf0) >> 4) << (key[j] & 0x0f);
	}
	std::cout << "\n";

	//test
	std::cout << "data:\t";
	for (uint32_t j = 0; j < 32; j++)
	{
		std::cout << std::hex << ((data[j] & 0xf0) >> 4) << (data[j] & 0x0f);
	}
	std::cout << "\n";

	std::cout << "index:\t" << ser << "\n";

	// Used to judge whether the keys and indexs entered by both parties are equal
	share *s_judge = judge(role, (BooleanCircuit *)ycirc, key, ser);

	// Add sharing and cat
	share *s_data = cat_data(data, 32, ser, (BooleanCircuit *)bcirc, (BooleanCircuit *)ycirc, role);

	// hmac512
	share *s_hash_out = hmac512_routine(key, 32, s_data, 37,  (BooleanCircuit *)ycirc,  party);
	
	// the IL is kept secret-shared between parties after the evaluation
	// IR will be chain code 
	share* s_IL = new boolshare(ABY_HMAC512_OUTPUT_BITS/2, ycirc);
	share* s_IR = new boolshare(ABY_HMAC512_OUTPUT_BITS/2, ycirc);
	for(uint32_t i = 0; i < ABY_HMAC512_OUTPUT_BITS/2; i++){
		s_IL->set_wire_id(i, s_hash_out->get_wire_id(i));
		s_IR->set_wire_id(i, s_hash_out->get_wire_id(i + ABY_HMAC512_OUTPUT_BITS/2));
	}
	
	// IL:Y2B
	s_IL = ((BooleanCircuit *)bcirc)->PutY2BGate(s_IL);
	s_IL = ((BooleanCircuit *)bcirc)->PutSharedOUTGate(s_IL);
	s_IR = ((BooleanCircuit *)ycirc)->PutOUTGate(s_IR, ALL);
	s_judge = ((BooleanCircuit *)ycirc)->PutOUTGate(s_judge, ALL);
	
	party->ExecCircuit();
	
	// If the keys or indexs entered by both parties are not equal, then return
	uint8_t *res = s_judge->get_clear_value_ptr();
	if(res[0])
	{
		std::cout << "The keys or indexs entered by both parties are not equal.\n";
		return NULL;
	}

	uint8_t *IL, *IR;
	IL = s_IL->get_clear_value_ptr();
	IR = s_IR->get_clear_value_ptr();
		

	// test
	std::cout << "IL（BOOL Share）:\t";
	for(uint32_t j = 0; j < ABY_HMAC512_OUTPUT_BYTES/2; j++){
		std::cout << std::hex << ((IL[j] & 0xf0)>>4) <<(IL[j] & 0x0f);
	}
	
	std::cout << "\n";
	
	std::cout << "IR（Chaincode）:\t";
	for(uint32_t j = 0; j < ABY_HMAC512_OUTPUT_BYTES/2; j++){
		std::cout << std::hex << ((IR[j] & 0xf0)>>4) <<(IR[j] & 0x0f);
	}
	std::cout << "\n";
	
	delete party;
	free(key);
	free(data);
	
	/**********************************************************/
	// Now we have the boolean share of IL, and then convert it to arithmetic share
	// Init, similar to abyparty
	B2AParty *b2aparty = new B2AParty(role, address, port, seclvl, bitlen,  nthreads, mt_alg);
	B2Asharing *b2asharing = b2aparty->GetSharing();
	
	// Input share, which will be converted
	b2asharing->InputConv(IL);
	
	// perform conversion
	b2aparty->ExecConv();
	
	// Obtain the output
	uint8_t* output = b2asharing->GetOutput();
	
	// Print
	std::cout << "IL（Arith Share）:\t";
	for(int j = 0; j < ABY_HMAC512_OUTPUT_BYTES/2; j++){
		std::cout << std::hex << ((output[j] & 0xf0)>>4) <<(output[j] & 0x0f);
	}
	std::cout << "\n";
	
	// return IL(Arith Share) || IR
	uint8_t *result = (uint8_t *)malloc(sizeof(uint8_t) * ABY_HMAC512_OUTPUT_BYTES);
	for(uint32_t j = 0; j < ABY_HMAC512_OUTPUT_BYTES/2; j++){
		result[j] = output[j];
		result[j + ABY_HMAC512_OUTPUT_BYTES/2] = IR[j];
	}	
	
	delete b2aparty;
	return result;
}

// The length of byte array is no more than len
uint8_t* str2byte(std::string str, uint32_t len)
{
	uint8_t *msg = (uint8_t *)malloc(sizeof(uint8_t) * len);
	memset(msg, 0, sizeof(uint8_t) * len);
	for (uint32_t i = 0; i < str.length()/2 && i < len; i++)
	{	std::string sub = str.substr(2*i, 2);
		msg[i] = stoi(sub, 0, 16);
	}
	return msg;
	
}

share* judge(e_role role, BooleanCircuit *circ, uint8_t* key, uint32_t ser)
{	
	// The keys entered by both parties must be equal
	share* skey1, *skey2, *ser1, *ser2;
	if (role == SERVER)
	{
		skey2 = circ->PutDummyINGate(256);
		skey1 = circ->PutINGate(key, 256, SERVER);
		ser2 = circ->PutDummyINGate(32);
		ser1 = circ->PutINGate(ser, 32, SERVER);
	}
	else
	{ // role == CLIENT
		skey2 = circ->PutINGate(key, 256, CLIENT);
		skey1 = circ->PutDummyINGate(256);
		ser2 = circ->PutINGate(ser, 32, CLIENT);
		ser1 = circ->PutDummyINGate(32);
	}
	share *s_GT1 = circ->PutGTGate(skey1, skey2);
	share *s_GT2 = circ->PutGTGate(skey2, skey1);
	share *s_GT3 = circ->PutGTGate(ser2, ser1);
	share *s_GT4 = circ->PutGTGate(ser1, ser2);
	share *res = circ->PutADDGate(s_GT1, s_GT2);
	res = circ->PutADDGate(res, s_GT3);
	res = circ->PutADDGate(res, s_GT4);
	return res;
}

share *cat_data(uint8_t *data, uint32_t data_len, uint32_t ser, BooleanCircuit *bcirc, BooleanCircuit *ycirc, e_role role)
{
	share *s_ser, *bs_share, *ys_share;
	bs_share = bcirc->PutSharedINGate(data, 256);	
	ys_share = ycirc->PutB2YGate(bs_share);
	
	s_ser = ycirc->PutCONSGate(ser, 32);

	// cat
	// data = 0x00||data1+data2||ser, (8+256+32)
	share *s_data = new boolshare(296, ycirc);

	uint64_t zero = 0;
	share *s_zero = ycirc->PutCONSGate(zero, 1);

	for (uint32_t i = 0; i < 8; i++)
	{
		s_data->set_wire_id(i, s_zero->get_wire_id(0));
	}
	for (uint32_t i = 0; i < 256; i++)
	{
		s_data->set_wire_id(i + 8, ys_share->get_wire_id(i));
	}
	// ser=0
	for (uint32_t i = 0; i < 32; i++)
	{
		s_data->set_wire_id(i + 8 + 256, s_ser->get_wire_id(i));
	}
	return s_data;
}

// hmac512
share* hmac512_routine(uint8_t *key, uint32_t keylen, share *s_data, uint32_t datalen, BooleanCircuit *ycirc, ABYParty *party)
{	
	// Init ipadkey and opadkey
	uint8_t *ipadkey = (uint8_t *)malloc(sizeof(uint8_t) * 128);
	uint8_t *opadkey = (uint8_t *)malloc(sizeof(uint8_t) * 128);
	
	memset(ipadkey, 0, sizeof(uint8_t) * 128);
	memset(opadkey, 0, sizeof(uint8_t) * 128);

	for (uint32_t i = 0; i < ABY_HMAC512_BlOCKSIZE_BYTES; i++)
	{
		if (i < keylen)
		{
			ipadkey[i] = key[i];
			opadkey[i] = key[i];
		}
		ipadkey[i] ^= 0x36;
		opadkey[i] ^= 0x5c;
	}

	share *s_firstState = CatAndHash(ipadkey, 128, s_data, datalen, ycirc);
	share *s_hash_out = CatAndHash(opadkey, 128, s_firstState, ABY_HMAC512_OUTPUT_BYTES, ycirc);
	
	free(ipadkey);
	free(opadkey);
	return s_hash_out;
}

// len(key)=1024bits
share *CatAndHash(uint8_t *key, uint32_t keylen, share *s_data, uint32_t datalen, BooleanCircuit *circ)
{
	share *s_msg = new boolshare(ABY_HMAC512_BlOCKSIZE * 2, circ);
	share *s_key;

	s_key = circ->PutCONSGate(key, keylen * 8);

	for (uint32_t i = 0; i < keylen * 8; i++)
	{
		s_msg->set_wire_id(i, s_key->get_wire_id(i));
	}
	for (uint32_t i = 0; i < datalen * 8; i++)
	{
		s_msg->set_wire_id(i + keylen * 8, s_data->get_wire_id(i));
	}

	uint8_t one = 0x80;
	share *s_one = circ->PutCONSGate(one, 8);
	for (uint32_t i = 0; i < 8; i++)
	{
		s_msg->set_wire_id((keylen + datalen) * 8 + i, s_one->get_wire_id(i));
	}

	uint8_t len[2];
	len[1] = (keylen + datalen) * 8;
	len[0] = ((keylen + datalen) * 8) >> 8;

	share *s_len;
	s_len = circ->PutCONSGate(len, 16);
	for (uint32_t i = 0; i < 16; i++)
	{
		s_msg->set_wire_id(2032 + i, s_len->get_wire_id(i));
	}

	// padding 0
	uint64_t zero = 0;
	share *s_zero = circ->PutCONSGate(zero, 1);
	for (uint32_t i = (keylen + datalen + 1) * 8; i < 2032; i++)
	{
		s_msg->set_wire_id(i, s_zero->get_wire_id(0));
	}

	share *s_out = BuildSHA512Circuit(s_msg, circ);
	return s_out;
}

// finished
//  construct SHA512 circuit
share *BuildSHA512Circuit(share *s_msg, BooleanCircuit *circ)
{
	// initialize state variables
	share **s_H = (share **)malloc(sizeof(share *) * 8);
	share **s_k = (share **)malloc(sizeof(share *) * 80);
	init_variables(s_H, s_k, circ);

	// Process data block
	share *firstblock = new boolshare(ABY_HMAC512_BlOCKSIZE, circ);
	share *secondblock = new boolshare(ABY_HMAC512_BlOCKSIZE, circ);

	for (uint32_t i = 0; i < ABY_HMAC512_BlOCKSIZE; i++)
	{
		firstblock->set_wire_id(i, s_msg->get_wire_id(i));
		secondblock->set_wire_id(i, s_msg->get_wire_id(i + ABY_HMAC512_BlOCKSIZE));
	}
	
	// First hash
	share *out = process_block(firstblock, s_H, s_k, circ);
	// Second hash
	out = process_block(secondblock, s_H, s_k, circ);

	free(s_H);
	free(s_k);
	return out;
}

// Initialize variables
void init_variables(share **s_H, share **s_k, BooleanCircuit *circ)
{
	// h0~h7
	s_H[0] = circ->PutCONSGate(ABY_HMAC512_H0, 64);
	s_H[1] = circ->PutCONSGate(ABY_HMAC512_H1, 64);
	s_H[2] = circ->PutCONSGate(ABY_HMAC512_H2, 64);
	s_H[3] = circ->PutCONSGate(ABY_HMAC512_H3, 64);
	s_H[4] = circ->PutCONSGate(ABY_HMAC512_H4, 64);
	s_H[5] = circ->PutCONSGate(ABY_HMAC512_H5, 64);
	s_H[6] = circ->PutCONSGate(ABY_HMAC512_H6, 64);
	s_H[7] = circ->PutCONSGate(ABY_HMAC512_H7, 64);

	// k0~k79
	for (uint32_t j = 0; j < 80; j++)
	{
		s_k[j] = circ->PutCONSGate(ConstantSHA512[j], 64);
	}
}

// Process data block
share *process_block(share *s_msg, share **s_H, share **s_k, BooleanCircuit *circ)
{
	share *out = new boolshare(ABY_HMAC512_OUTPUT_BITS, circ);
	share **s_w = (share **)malloc(sizeof(share *) * 80);

	// break message into 1024-bit chunks
	// for each chunk
	//     break chunk into sixteen 64-bit big-endian words w[i], 0 ≤ i ≤ 15
	break_message_to_chunks(s_w, s_msg, circ);

	/* for t from 16 to 79
		  Wt = SSIG1(W(t-2)) + W(t-7) + SSIG0(W(t-15)) + W(t-16)
	*/
	// SSIG1(x) = ROTR^19(x) XOR ROTR^61(x) XOR SHR^6(x)
	// SSIG0(x) = ROTR^1(x) XOR ROTR^8(x) XOR SHR^7(x)
	expand_ws(s_w, circ);

	// Main Loop; result is written into s_H
	sha512_main_loop(s_H, s_w, s_k, circ);

	// ******************s?
	for (uint32_t i = 0, wid; i < 8; i++)
	{
		for (uint32_t j = 0; j < 64; j++)
		{
			if (j < 8)
			{
				wid = 56;
			}
			else if (j < 16)
			{
				wid = 48;
			}
			else if (j < 24)
			{
				wid = 40;
			}
			else if (j < 32)
			{
				wid = 32;
			}
			else if (j < 40)
			{
				wid = 24;
			}
			else if (j < 48)
			{
				wid = 16;
			}
			else if (j < 56)
			{
				wid = 8;
			}
			else
			{
				wid = 0;
			}
			out->set_wire_id(i * 64 + j, s_H[i]->get_wire_id(wid + (j % 8)));
		}
	}

	free(s_w);

	return out;
}

void break_message_to_chunks(share **s_w, share *s_msg, BooleanCircuit *circ)
{
	for (uint32_t i = 0; i < 16; i++)
	{
		s_w[i] = new boolshare(64, circ);
	}
	// iterate over message bytes
	uint32_t wid;
	for (uint32_t i = 0; i < 16; i++)
	{
		// iterate over bits
		for (uint32_t j = 0; j < 64; j++)
		{
			if (j < 8)
			{
				wid = 56;
			}
			else if (j < 16)
			{
				wid = 48;
			}
			else if (j < 24)
			{
				wid = 40;
			}
			else if (j < 32)
			{
				wid = 32;
			}
			else if (j < 40)
			{
				wid = 24;
			}
			else if (j < 48)
			{
				wid = 16;
			}
			else if (j < 56)
			{
				wid = 8;
			}
			else
			{
				wid = 0;
			}
			s_w[i]->set_wire_id((j % 8) + wid, s_msg->get_wire_id(i * 64 + j));
		}
	}
}

/* for t from 16 to 79
	  Wt = SSIG1(W(t-2)) + W(t-7) + SSIG0(W(t-15)) + W(t-16)
*/
// SSIG1(x) = ROTR^19(x) XOR ROTR^61(x) XOR SHR^6(x)
// SSIG0(x) = ROTR^1(x) XOR ROTR^8(x) XOR SHR^7(x)
void expand_ws(share **s_w, BooleanCircuit *circ)
{

	for (uint32_t i = 16; i < 80; i++)
	{
		s_w[i] = new boolshare(64, circ);
		s_w[i] = circ->PutADDGate(s_w[i - 7], s_w[i - 16]);
		s_w[i] = circ->PutADDGate(s_w[i], SSIG1(s_w[i - 2], circ));
		s_w[i] = circ->PutADDGate(s_w[i], SSIG0(s_w[i - 15], circ));
	}
}

share *SSIG1(share *s_x, BooleanCircuit *circ)
{
	// ROTR^19(x)
	share *s_tmp1 = new boolshare(64, circ);
	// ROTR^61(x)
	share *s_tmp2 = new boolshare(64, circ);
	// SHR^6(x)
	share *s_tmp3 = new boolshare(64, circ);

	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp1->set_wire_id(j, s_x->get_wire_id((j + 19) % 64));
	}
	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp2->set_wire_id(j, s_x->get_wire_id((j + 61) % 64));
	}

	uint64_t zero = 0;
	share *s_zero = circ->PutCONSGate(zero, 1);
	for (uint32_t j = 0; j < 58; j++)
	{
		s_tmp3->set_wire_id(j, s_x->get_wire_id(j + 6));
	}
	for (uint32_t j = 58; j < 64; j++)
	{
		s_tmp3->set_wire_id(j, s_zero->get_wire_id(0));
	}

	// s_tmp1=s_tmp1 xor s_tmp2 xor s_tmp3
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp2);
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp3);

	return s_tmp1;
}

share *SSIG0(share *s_x, BooleanCircuit *circ)
{
	// ROTR^1(x)
	share *s_tmp1 = new boolshare(64, circ);
	// ROTR^8(x)
	share *s_tmp2 = new boolshare(64, circ);
	// SHR^7(x)
	share *s_tmp3 = new boolshare(64, circ);
	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp1->set_wire_id(j, s_x->get_wire_id((j + 1) % 64));
	}
	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp2->set_wire_id(j, s_x->get_wire_id((j + 8) % 64));
	}
	uint64_t zero = 0;
	share *s_zero = circ->PutCONSGate(zero, 1);
	for (uint32_t j = 0; j < 57; j++)
	{
		s_tmp3->set_wire_id(j, s_x->get_wire_id(j + 7));
	}
	for (uint32_t j = 57; j < 64; j++)
	{
		s_tmp3->set_wire_id(j, s_zero->get_wire_id(0));
	}

	// s_tmp1=s_tmp1 xor s_tmp2 xor s_tmp3
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp2);
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp3);

	return s_tmp1;
}

share *BSIG1(share *s_x, BooleanCircuit *circ)
{
	// ROTR^14(x)
	share *s_tmp1 = new boolshare(64, circ);
	// ROTR^18(x)
	share *s_tmp2 = new boolshare(64, circ);
	// ROTR^41(x)
	share *s_tmp3 = new boolshare(64, circ);

	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp1->set_wire_id(j, s_x->get_wire_id((j + 14) % 64));
	}
	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp2->set_wire_id(j, s_x->get_wire_id((j + 18) % 64));
	}
	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp3->set_wire_id(j, s_x->get_wire_id((j + 41) % 64));
	}

	// s_tmp1=s_tmp1+s_tmp2+s_tmp3
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp2);
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp3);

	return s_tmp1;
}

share *BSIG0(share *s_x, BooleanCircuit *circ)
{
	// ROTR^28(x)
	share *s_tmp1 = new boolshare(64, circ);
	// ROTR^34(x)
	share *s_tmp2 = new boolshare(64, circ);
	// ROTR^39(x)
	share *s_tmp3 = new boolshare(64, circ);

	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp1->set_wire_id(j, s_x->get_wire_id((j + 28) % 64));
	}
	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp2->set_wire_id(j, s_x->get_wire_id((j + 34) % 64));
	}
	for (uint32_t j = 0; j < 64; j++)
	{
		s_tmp3->set_wire_id(j, s_x->get_wire_id((j + 39) % 64));
	}

	// s_tmp1=s_tmp1+s_tmp2+s_tmp3
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp2);
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp3);

	return s_tmp1;
}

share *CH(share *s_x, share *s_y, share *s_z, BooleanCircuit *circ)
{
	// CH( x, y, z) = (x AND y) XOR ( (NOT x) AND z)
	share *s_tmp1, *s_tmp2;
	s_tmp1 = circ->PutANDGate(s_x, s_y);
	s_tmp2 = circ->PutINVGate(s_x);
	s_tmp2 = circ->PutANDGate(s_tmp2, s_z);
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp2);
	return s_tmp1;
}

share *MAJ(share *s_x, share *s_y, share *s_z, BooleanCircuit *circ)
{
	// MAJ( x, y, z) = (x AND y) XOR (x AND z) XOR (y AND z)
	share *s_tmp1, *s_tmp2, *s_tmp3;
	s_tmp1 = circ->PutANDGate(s_x, s_y);
	s_tmp2 = circ->PutANDGate(s_x, s_z);
	s_tmp3 = circ->PutANDGate(s_y, s_z);
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp2);
	s_tmp1 = circ->PutXORGate(s_tmp1, s_tmp3);
	return s_tmp1;
}

void sha512_main_loop(share **s_H, share **s_w, share **s_k, BooleanCircuit *circ)
{
	/*
	 * Initialize hash value for this chunk:
	 * a = h0; b = h1; c = h2; d = h3; e = h4; f = h5; g=h6; h=h7
	 */
	share *s_a, *s_b, *s_c, *s_d, *s_e, *s_f, *s_g, *s_h;

	s_a = new boolshare(64, circ);
	s_b = new boolshare(64, circ);
	s_c = new boolshare(64, circ);
	s_d = new boolshare(64, circ);
	s_e = new boolshare(64, circ);
	s_f = new boolshare(64, circ);
	s_g = new boolshare(64, circ);
	s_h = new boolshare(64, circ);

	s_a->set_wire_ids(s_H[0]->get_wires());
	s_b->set_wire_ids(s_H[1]->get_wires());
	s_c->set_wire_ids(s_H[2]->get_wires());
	s_d->set_wire_ids(s_H[3]->get_wires());
	s_e->set_wire_ids(s_H[4]->get_wires());
	s_f->set_wire_ids(s_H[5]->get_wires());
	s_g->set_wire_ids(s_H[6]->get_wires());
	s_h->set_wire_ids(s_H[7]->get_wires());

	share *s_t1, *s_t2, *s_tmp;
	for (uint32_t i = 0; i < 80; i++)
	{
		// T1
		s_t1 = circ->PutADDGate(s_h, BSIG1(s_e, circ));
		s_t1 = circ->PutADDGate(s_t1, CH(s_e, s_f, s_g, circ));
		s_t1 = circ->PutADDGate(s_t1, s_k[i]);
		s_t1 = circ->PutADDGate(s_t1, s_w[i]);

		// T2
		s_t2 = circ->PutADDGate(BSIG0(s_a, circ), MAJ(s_a, s_b, s_c, circ));

		/*
			h = g
			g = f
			f = e
			e = d + T1
			d = c
			c = b
			b = a
			a = T1 + T2
		*/
		s_h->set_wire_ids(s_g->get_wires());
		s_g->set_wire_ids(s_f->get_wires());
		s_f->set_wire_ids(s_e->get_wires());
		s_d = circ->PutADDGate(s_d, s_t1);
		s_e->set_wire_ids(s_d->get_wires());
		s_d->set_wire_ids(s_c->get_wires());
		s_c->set_wire_ids(s_b->get_wires());
		s_b->set_wire_ids(s_a->get_wires());
		s_t1 = circ->PutADDGate(s_t1, s_t2);
		s_a->set_wire_ids(s_t1->get_wires());
	}

	s_H[0] = circ->PutADDGate(s_H[0], s_a);
	s_H[1] = circ->PutADDGate(s_H[1], s_b);
	s_H[2] = circ->PutADDGate(s_H[2], s_c);
	s_H[3] = circ->PutADDGate(s_H[3], s_d);
	s_H[4] = circ->PutADDGate(s_H[4], s_e);
	s_H[5] = circ->PutADDGate(s_H[5], s_f);
	s_H[6] = circ->PutADDGate(s_H[6], s_g);
	s_H[7] = circ->PutADDGate(s_H[7], s_h);
}
