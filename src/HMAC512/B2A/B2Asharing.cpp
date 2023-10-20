#include <algorithm>
#include "B2Asharing.h"
#include <abycore/aby/abysetup.h>


void B2Asharing::Init() {
	ConvShareSndCtr = 0;
	ConvShareRcvCtr = 0;
	sharelen_byte = sharelen_bit/8;
	sharelen_word = sharelen_bit/32;
	Share = (uint8_t*) malloc(sizeof(uint8_t) * (sharelen_byte));
}

void B2Asharing::PrepareSetupPhase(ABYSetup* setup) {
		XORMasking* fXORMaskFct = new XORMasking(sharelen_bit); 

		uint32_t NumCONVs = sharelen_bit;

		IKNP_OTTask* task = (IKNP_OTTask*) malloc(sizeof(IKNP_OTTask));
        	// The bitlen of OT
		task->bitlen = sharelen_bit;      
		task->snd_flavor = Snd_R_OT;
		task->rec_flavor = Rec_OT;
		// The number of OT
		task->numOTs = sharelen_bit;
		task->mskfct = fXORMaskFct;
		task->delete_mskfct = TRUE;
		if ((eRole) == SERVER) {
			ConversionMasks[0].Create(sharelen_bit, sharelen_bit);
			ConversionMasks[1].Create(sharelen_bit, sharelen_bit);
			task->pval.sndval.X0 = &(ConversionMasks[0]);
			task->pval.sndval.X1 = &(ConversionMasks[1]);
		} else {
			ConversionMasks[0].Create(sharelen_bit, Crypto); //the choice bits of the receiver
			ConversionMasks[1].Create(sharelen_bit, sharelen_bit); //the resulting masks
			task->pval.rcvval.C = &(ConversionMasks[0]);
			task->pval.rcvval.R = &(ConversionMasks[1]);
		}

		// std::cout << "Conv: Adding an OT task to perform " << task->numOTs << " OTs for B2A" << std::endl;

		setup->AddOTTask(task, 0);

		// Pre-create some conversion buffers
		// TODO Network send buffers could be much smaller, only used per round.
		// But since there's currently no way of knowing in advance each round's
		// conversion gates, the send buffer is created with max possible size...
		if (eRole == CLIENT) {
			ConvShareSndBuf.Create(sharelen_bit, 1);
		} else {
			ConvShareSndBuf.Create(2 * sharelen_bit, sharelen_bit);
		}
		// Note: Network receive buffer m_vConvShareRcvBuf is created per round in
		// GetBuffersToReceive
		ConversionRandomness.Create(sharelen_bit, sharelen_bit, Crypto);
	
}

// The client will send the permbit to the server
void B2Asharing::EvaluateCONV() {
	if (eRole == SERVER) {
		// Network receive counter per round
		ConvShareRcvCtr += sharelen_bit;	
	} else {
		//Client routine - send bits
		//copy values into snd buffer
		ConvShareSndBuf.SetBitsPosOffset(ConversionMasks[0].GetArr(),
				ConvShareSndCtr, ConvShareSndCtr, sharelen_bit);	
		
		
	for(uint32_t i = 0; i < sharelen_bit; i+=8){
		for(uint32_t j=0;j<8;j++){
			uint8_t val[1];
			val[0] = Conv[i/8] >> (7-j%8);
			ConvShareSndBuf.XORBitNoMask(i+j, val[0]);
			//ConvShareSndBuf.XORBits((BYTE*)val, i, 1);
		}
	}
		
	// Update ConvShareSndCtr
	ConvShareSndCtr += sharelen_bit;
	}
}


void B2Asharing::AssignConversionShares() {
	if (eRole == SERVER) {
		// Reset send counter in case we sent something this round
		ConvShareSndCtr = 0;
		if (ConvShareRcvCtr > 0) {
			AssignServerConversionShares();
		}
	} else {
		if (ConvShareRcvCtr > 0) {
			// 2nd round - we received OT data
			AssignClientConversionShares();
		}
		// We sent something this round - prepare next layer and round
		// For server side, this all happens in AssignServerConversionShares
		if (ConvShareSndCtr > 0) {
			// Now is 1st round (send) - prepare 2nd round (receive OT data)
			// Backup old index for next round's AssignClientConversionShares

			// In round 2 we receive the same amount of data that we just sent.
			ConvShareRcvCtr = ConvShareSndCtr;
			ConvShareSndCtr = 0; // reset 
		}
	}
}

void B2Asharing::AssignServerConversionShares() {
	// Prepare the send counter. We'll send as much next round as we received this
	// round. Since we consume what was received this round, reset the receive
	// counter.
	ConvShareSndCtr = ConvShareRcvCtr;
	ConvShareRcvCtr = 0;
	// We received conversion shares from the client this round. Now we consume
	// that data and prepare the send buffer for sending the OTs in the next
	// round.

	// BigNum
	BigNum B_cor[1], B_rnd[1],  B_tmpa[1], B_tmpb[1], B_tmpj[1], B_tmp1[1], B_tmp2[1], B_tmpsum[1], B_n[1];

	uint8_t clientpermbit;

	// conversion share
	uint32_t cor[1];
	B_cor->data = cor;
	B_cor->maxLen = 1;
	B_cor->len = 1;

	uint8_t tmp_cor[4] = {0};

	// Randomness
	uint32_t rnd[sharelen_word];
	B_rnd->data = rnd;
	B_rnd->maxLen = (sharelen_word);
	B_rnd->len = (sharelen_word);

	// tmpa
	uint32_t tmpa[sharelen_word];
	B_tmpa->data = tmpa;
	B_tmpa->maxLen = (sharelen_word);
	B_tmpa->len = (sharelen_word);

	// B_tmpb
	uint32_t tmpb[sharelen_word];
	B_tmpb->data = tmpb;
	B_tmpb->maxLen = (sharelen_word);
	B_tmpb->len = (sharelen_word);

	// B_tmpj
	uint32_t tmpj[sharelen_word];
	B_tmpj->data = tmpj;
	B_tmpj->maxLen = (sharelen_word);
	B_tmpj->len = (sharelen_word);

	// B_tmp1
	uint32_t tmp1[sharelen_word];
	B_tmp1->data = tmp1;
	B_tmp1->maxLen = (sharelen_word);
	B_tmp1->len = (sharelen_word);

	// B_tmp2
	uint32_t tmp2[sharelen_word];
	B_tmp2->data = tmp2;
	B_tmp2->maxLen = (sharelen_word);
	B_tmp2->len = (sharelen_word);

	// tmpsum initialized to 0
	uint32_t tmpsum[sharelen_word] = {0};
	B_tmpsum->data = tmpsum;
	B_tmpsum->maxLen = (sharelen_word);
	B_tmpsum->len = (sharelen_word);

	// n = 2^256
	uint8_t n[(sharelen_byte) + 4] ={0};
	n[3] = 1;
	uint32_t nn[(sharelen_word) + 1];
	// Little Endian form ----> Big Endian form
	U8ToU32(n, nn, ((sharelen_byte) + 4));
	B_n->data = nn;
	B_n->len = (sharelen_word) + 1;
	B_n->maxLen = (sharelen_word) + 1;

	uint32_t* mask1, *mask2;
	mask1 = (uint32_t*) malloc(sizeof(uint32_t) * (sharelen_word));
	mask2 = (uint32_t*) malloc(sizeof(uint32_t) * (sharelen_word));


	for (uint32_t i = 0, lctr = 0, gctr = 0; i < sharelen_bit; i++) {
		// clientpermbit = pi ^ val
		clientpermbit = ConvShareRcvBuf.GetBitNoMask(i);

		//cor
		tmp_cor[3] = (Conv[i/8]>>(7-i%8)) & 0x01;
		U8ToU32(tmp_cor, cor, 4);

		// Randomness
		for(uint32_t j = 0; j < (sharelen_word); j++, lctr++){
			rnd[j] = ConversionRandomness.template Get<uint32_t>(lctr * sizeof(uint32_t) * 8, sizeof(uint32_t) * 8);
		}

		// tmpa = 2^256 - rnd + 2^i * cor[0] 
		// 2^256-rnd
		BigNum_sub(B_n, B_rnd, B_tmp1);

		// 1 << (sharelen_bit-1)-i
		uint8_t *t;
		t = (uint8_t*) malloc(sizeof(uint8_t) * (sharelen_byte));
		memset(t, 0, sizeof(uint8_t) * (sharelen_byte));
		t[i/8] = 1 << (7 - i % 8);
		U8ToU32(t, tmpj, sharelen_byte);

		BigNum_mul(B_cor, B_tmpj, B_tmp2);
		BigNum_modAdd(B_tmp1, B_tmp2, B_n, B_tmpa);

		//tmpb = 2^64 - rnd + 2^j * (1-x)
		tmp_cor[3] = tmp_cor[3] ^ 1;
		U8ToU32(tmp_cor, cor, 4);

		BigNum_mul(B_cor, B_tmpj, B_tmp2);
		BigNum_modAdd(B_tmp1, B_tmp2, B_n, B_tmpb);

		// xor ConversionMasks
		for(uint32_t j = 0; j < sharelen_word; j++, gctr++){
			mask1[j] = ConversionMasks[clientpermbit].template Get<uint32_t>(gctr * sizeof(uint32_t) * 8, sizeof(uint32_t) * 8);
			mask2[j] = ConversionMasks[!clientpermbit].template Get<uint32_t>(gctr * sizeof(uint32_t) * 8, sizeof(uint32_t) * 8);

			tmpa[j] ^= mask1[j];
			tmpb[j] ^= mask2[j];
		}
		
		// Sum rnd
		BigNum_modAdd(B_tmpsum, B_rnd, B_n, B_tmpsum);

		// send
		for(uint32_t j = 0; j < sharelen_word; j++){
			ConvShareSndBuf.template Set<uint32_t>(tmpa[j], 2 * i * sharelen_bit + j * sizeof(uint32_t) * 8, sizeof(uint32_t) * 8);
			ConvShareSndBuf.template Set<uint32_t>(tmpb[j], (2 * i + 1) * sharelen_bit + j * sizeof(uint32_t) * 8, sizeof(uint32_t) * 8);
		}
		
	}

	//Big Endian form ----> Little Endian form
	U32ToU8(B_tmpsum->data, Share, sharelen_byte);
	
	free(mask1);
	free(mask2);
}

void B2Asharing::AssignClientConversionShares() {
	// Reset - we'll now consume what we received.
	ConvShareRcvCtr = 0;
	// We sent conversion shares in the last round and received data from server this round. 
	// Unmask the data using values that were precomputed in the OTs
	
	// BigNum
	BigNum B_rcv[1], B_mask[1], B_tmp[1], B_tmpsum[1], B_n[1];

	// rcv
	uint32_t rcv[sharelen_word];
	B_rcv->data = rcv;
	B_rcv->len = sharelen_word;
	B_rcv->maxLen = sharelen_word;

	// mask
	uint32_t mask[sharelen_word];
	//B_mask->data = mask;
	//B_mask->len = sharelen_word;
	//B_mask->maxLen = sharelen_word;

	// B_tmp
	uint32_t tmp[sharelen_word];
	B_tmp->data = tmp;
	B_tmp->len = sharelen_word;
	B_tmp->maxLen = sharelen_word;

	// B_tmpsum
	uint32_t tmpsum[sharelen_word] = {0};
	B_tmpsum->data = tmpsum;
	B_tmpsum->len = sharelen_word;
	B_tmpsum->maxLen = sharelen_word;
	
	// B_n
	uint8_t n[(sharelen_byte) + 4] ={0};
	n[3] = 1;
	uint32_t nn[(sharelen_word) + 1];
	U8ToU32(n, nn, ((sharelen_byte) + 4));
	B_n->data = nn;
	B_n->len = (sharelen_word) + 1;
	B_n->maxLen = (sharelen_word) + 1;

	for(uint32_t i = 0, gctr = 0; i < sharelen_bit; i++){
		// choose bit
		uint32_t cor = uint32_t((Conv[i/8]>>(7-i%8)) & 0x01);	

		// rcv
		for(uint32_t j = 0; j < sharelen_word; j++, gctr++){
			rcv[j] = ConvShareRcvBuf.template Get<uint32_t>((2 * i + cor) * sharelen_bit + j * sizeof(uint32_t) * 8, sizeof(uint32_t) * 8);
			mask[j] = ConversionMasks[1].template Get<uint32_t>(gctr * sizeof(uint32_t) * 8, sizeof(uint32_t) * 8);
			tmp[j] = rcv[j] ^ mask[j];
		}

		// Sum tmp
		BigNum_modAdd(B_tmpsum, B_tmp, B_n, B_tmpsum);
		
	}
	// Big Endian form ---> Little Endian form
	U32ToU8(B_tmpsum->data, Share, sharelen_byte);
}

void B2Asharing::GetDataToSend(std::vector<BYTE*>& sendbuf, std::vector<uint64_t>& sndbytes) {
	//Conversion shares
	if (ConvShareSndCtr > 0) {
		sendbuf.push_back(ConvShareSndBuf.GetArr());
		//the client sends shares of his choice bits, the server the masks
		uint32_t snd_bytes = (eRole == CLIENT) ?
			ceil_divide(ConvShareSndCtr, 8) : 2 * ConvShareSndCtr * sizeof(uint32_t) * sharelen_word;
		sndbytes.push_back(snd_bytes);
	}
}

void B2Asharing::GetBuffersToReceive(std::vector<BYTE*>& rcvbuf, std::vector<uint64_t>& rcvbytes) {
	//std::cout << "Getting buffers to receive!" << std::endl;

	//conversion shares
	if (ConvShareRcvCtr > 0) {
		//std::cout << "Receiving conversion values " << std::endl;
		// SERVER only receives bits, no ot masks
		uint32_t rcv_bytes = (eRole == SERVER) ?
			ceil_divide(ConvShareRcvCtr, 8) : 2 * ConvShareRcvCtr * sizeof(uint32_t) * sharelen_word;
		if (ConvShareRcvBuf.GetSize() < rcv_bytes) {
			if (eRole == SERVER) {
				ConvShareRcvBuf.Create(rcv_bytes * 8, 1);
			} else {
				ConvShareRcvBuf.Create(2 * ConvShareRcvCtr, sharelen_bit, 1);
			}
		}
		rcvbuf.push_back(ConvShareRcvBuf.GetArr());
		rcvbytes.push_back(rcv_bytes);
	}
}

ePreCompPhase B2Asharing::GetPreCompPhaseValue() {
	return PhaseValue;
}

void B2Asharing::Reset() {
	ConversionMasks[0].delCBitVector();
	ConversionMasks[1].delCBitVector();
	ConvShareSndBuf.delCBitVector();
	ConvShareRcvBuf.delCBitVector();
	ConversionRandomness.delCBitVector();
	free(Share);
	sharelen_bit = 0;
	ConvShareSndCtr = 0;
	ConvShareRcvCtr = 0;
}

uint8_t* B2Asharing::GetShare(){
	return Share;
}

void B2Asharing::InputConv(uint8_t* Input){
	//Conv = (uint8_t*)malloc(sizeof(uint8_t) * (sharelen_byte));
	Conv = Input;
}

// for debug only
uint8_t* B2Asharing::GetConv(){
	return Conv;
}

uint8_t* B2Asharing::GetOutput() {
		return Share;
}
