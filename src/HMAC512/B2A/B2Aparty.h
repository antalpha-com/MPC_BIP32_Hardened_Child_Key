// Copyright © 2023 Antalpha
//
// This file is part of Antalpha. The full Antalpha copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#ifndef __B2APARTY_H__
#define __B2APARTY_H__

#include <abycore/ABY_utils/ABYconstants.h>
#include <abycore/aby/abysetup.h>
#include <ENCRYPTO_utils/timer.h>
#include <memory>
#include <vector>

class ABYSetup;
class channel;
struct comm_ctx;
class crypto;
class B2Asharing;
class CEvent;
class CLock;

class B2AParty {
public:
    B2AParty(e_role pid, const std::string& addr = "127.0.0.1", uint16_t port = 7766, seclvl seclvl = LT, uint32_t bitlen = 256,
		uint32_t nthreads =	2, e_mt_gen_alg mg_algo = MT_OT);
	~B2AParty();

    	/**
	 * Online part of initialization. Needs to be called after B2AParty has been
	 * construced. 
	 */
	void ConnectAndBaseOTs();  	//

	void Reset();	//

	double GetTiming(ABYPHASE phase);	//
	uint64_t GetSentData(ABYPHASE phase);	//
	uint64_t GetReceivedData(ABYPHASE phase);	//

	void ExecConv();	//
	B2Asharing* GetSharing();
	BOOL InitConv(uint32_t bitlen);

private:
    BOOL Init();	//
	void Cleanup();	//

	BOOL EstablishConnection();	//

	BOOL B2APartyListen();	//
	BOOL B2APartyConnect();	//

	// BOOL EvaluateCircuit();
	BOOL EvaluateConv();

	BOOL PerformInteraction();	//
	BOOL ThreadSendValues(uint32_t id);	//
	BOOL ThreadReceiveValues();	//
	
	class CPartyWorkerThread;
	
	enum EPartyJobType {
		e_Party_Comm, e_Party_Stop, e_Party_Undefined
	};

	BOOL WakeupWorkerThreads(EPartyJobType);	//
	BOOL WaitWorkerThreads();	//
	BOOL ThreadNotifyTaskDone(BOOL);	//

	std::vector<CPartyWorkerThread*> vThreads;
	std::unique_ptr<CEvent> m_evt;
	std::unique_ptr<CLock> m_lock;

	bool is_online = false;	

	std::unique_ptr<crypto> Crypt;
	std::unique_ptr<CLock> glock;

	e_role eRole; // thread id
	uint32_t NumOTThreads;
	e_mt_gen_alg m_eMTGenAlg;

	// Order of destruction is important:
	// ABYSetup << comm_ctx << sockets
	std::vector<std::unique_ptr<CSocket>> Sockets; // sockets for threads

	std::unique_ptr<comm_ctx> Comm;

	std::unique_ptr<ABYSetup> Setup;

	uint16_t Port;
	seclvl SecLvl;

	uint32_t nHelperThreads;

	const std::string Address;

	B2Asharing* Sharing;

	channel* PartyChan;

	uint32_t nWorkingThreads;
	BOOL WorkerThreadSuccess;
	
	

};

#endif
