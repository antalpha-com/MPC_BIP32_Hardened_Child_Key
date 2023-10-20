#ifndef __B2ASHARING_H__
#define __B2ASHARING_H__

#include <ENCRYPTO_utils/cbitvector.h>
#include <abycore/ABY_utils/ABYconstants.h>
#include <abycore/aby/abysetup.h>
#include <algorithm>
#include <vector>

extern "C" {
	#include "../BigNum/BigNum/BigNum.h"
}

class B2Asharing{
public:
    B2Asharing(e_role role, uint32_t sharebitlen, crypto* crypt) :
	eRole(role),
	ConversionMasks(2),
	sharelen_bit(sharebitlen),
	Crypto(crypt),
	PhaseValue(ePreCompDefault)
	{
		Init();
	}

	/** Destructor of the class.*/
	~B2Asharing() {
		Reset();
	}

	void Reset();
	
	// OT
	void PrepareSetupPhase(ABYSetup* setup);    

	// Evaluating Conversion
	void EvaluateCONV();    

   	// Method for assigning conversion shares.
	void AssignConversionShares();  

	// Getting precomputation phase value
	ePreCompPhase GetPreCompPhaseValue();      
	uint8_t *GetShare();

	// Input value
	void InputConv(uint8_t* Input);
	
	// Get output share
	uint8_t* GetOutput();
	
	// Send and Receive data
	void GetDataToSend(std::vector<BYTE*>& sendbuf, std::vector<uint64_t>& sndbytes);
	void GetBuffersToReceive(std::vector<BYTE*>& rcvbuf, std::vector<uint64_t>& rcvbytes);

	// for debug only
	uint8_t* GetConv();

private:
	e_role eRole;
	crypto* Crypto;

	// The length of the share
	uint32_t sharelen_bit;
	uint32_t sharelen_byte;
	uint32_t sharelen_word;

	// mask
	std::vector<CBitVector> ConversionMasks;

	// send buf
	CBitVector ConvShareSndBuf;
	// receive buf
	CBitVector ConvShareRcvBuf;

	// Randomness
	CBitVector ConversionRandomness;

	// Boolean sharing value
	uint8_t* Conv; 

	// Arith sharing value
	uint8_t* Share;

	// Network send counter per round
	uint32_t ConvShareSndCtr; 
	// Network receive counter per round
	uint32_t ConvShareRcvCtr; 
	
	//Variable storing the current Precomputation Mode
	ePreCompPhase PhaseValue; 

	//Method for assigning server conversion shares.
	void AssignServerConversionShares();    

	//Method for assigning client conversion shares.
	void AssignClientConversionShares();    

	// Method for initialising.
	void Init();    
};

#endif
