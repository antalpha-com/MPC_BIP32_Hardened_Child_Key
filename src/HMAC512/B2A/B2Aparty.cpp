#include "B2Aparty.h"
#include <abycore/aby/abysetup.h>
#include "B2Asharing.h"
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/connection.h>
#include <ENCRYPTO_utils/thread.h>

#include <cstdlib>
#include <mutex>
#include <sstream>

class B2AParty::CPartyWorkerThread: public CThread {
public:
	CPartyWorkerThread(uint32_t id, B2AParty* callback) :
			threadid(id), m_pCallback(callback) {
		m_eJob = e_Party_Undefined;
	};

	void PutJob(EPartyJobType e) {
		std::lock_guard<std::mutex> lock(m_eJob_mutex_);
		m_eJob = e;
		m_evt.Set();
	}

	CEvent* GetEvent() {
		return &m_evt;
	}
private:
	void ThreadMain();
	uint32_t threadid;
	B2AParty* m_pCallback;
	CEvent m_evt;
	EPartyJobType m_eJob;
	std::mutex m_eJob_mutex_;
};

B2AParty::B2AParty(e_role pid, const std::string& addr, uint16_t port, seclvl seclvl,  uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mg_algo)
	: Crypt(std::make_unique<crypto>(seclvl.symbits)), glock(std::make_unique<CLock>()),
	m_eMTGenAlg(mg_algo), eRole(pid), NumOTThreads(nthreads),
	Comm(std::make_unique<comm_ctx>()),
	Setup(std::make_unique<ABYSetup>(Crypt.get(), NumOTThreads, eRole, m_eMTGenAlg)),
	Port(port), SecLvl(seclvl),
	Address(addr) {

	StartWatch("Initialization", P_INIT);

	m_evt = std::make_unique<CEvent>();
	m_lock = std::make_unique<CLock>();

	Init();
	StopWatch("Time for initiatlization: ", P_INIT);
	
	StartWatch("Generating sharing", P_CIRCUIT);
	if (!InitConv(bitlen)) {
		std::cout << "There was an while initializing the Sharing, ending! " << std::endl;
		std::exit(EXIT_FAILURE);
	}
	StopWatch("Time for sharing generation: ", P_CIRCUIT);
}

void B2AParty::ConnectAndBaseOTs() {
	if (!is_online) {
		//Establish network connection
		StartWatch("Establishing network connection: ", P_NETWORK);
		if (!EstablishConnection()) {
			std::cout << "There was an error during establish connection, ending! " << std::endl;
			std::exit(EXIT_FAILURE);
		}
		StopWatch("Time for network connect: ", P_NETWORK);

		/* Pre-Compute Naor-Pinkas base OTs by starting two threads */
		StartRecording("Starting NP OT", P_BASE_OT, Sockets);
		Setup->PrepareSetupPhase(Comm.get());
		StopRecording("Time for NP OT: ", P_BASE_OT, Sockets);

		is_online = true;
	}
}

B2AParty::~B2AParty() {
	Cleanup();
}

BOOL B2AParty::Init() {
	//Threads that support execution by e.g. concurrent sending / receiving
    nHelperThreads = 2;

	Sockets.resize(2);

	vThreads.resize(nHelperThreads);
	for (uint32_t i = 0; i < nHelperThreads; i++) {
		vThreads[i] = new CPartyWorkerThread(i, this); //First thread is started as receiver, second as sender
		vThreads[i]->Start();
	}

	return TRUE;
}

void B2AParty::Cleanup() {
	
	delete Sharing;

	for (uint32_t i = 0; i < nHelperThreads; i++) {
		vThreads[i]->PutJob(e_Party_Stop);
		vThreads[i]->Wait();
		delete vThreads[i];
	}
}

void B2AParty::ExecConv() {
    ConnectAndBaseOTs(); 

    StartRecording("Starting execution", P_TOTAL, Sockets);

    StartRecording("Starting setup phase: ", P_SETUP, Sockets);

    Sharing->PrepareSetupPhase(Setup.get());

    StartRecording("Starting OT Extension", P_OT_EXT, Sockets);
	Setup->PerformSetupPhase();
	StopRecording("Time for OT Extension phase: ", P_OT_EXT, Sockets);


    StopRecording("Time for setup phase: ", P_SETUP, Sockets);

    //Online phase
	if(Sharing->GetPreCompPhaseValue() != ePreCompStore) {
		StartRecording("Starting online phase: ", P_ONLINE, Sockets);
		EvaluateConv();
		StopRecording("Time for online phase: ", P_ONLINE, Sockets);
	}

    StopRecording("Total Time: ", P_TOTAL, Sockets);
}

BOOL B2AParty::EvaluateConv() {

	// create channel
	PartyChan = new channel(ABY_PARTY_CHANNEL, Comm->rcv_std.get(), Comm->snd_std.get());
    
	Sharing->EvaluateCONV();

	// perform interaction
	PerformInteraction();

	Sharing->AssignConversionShares();

    PerformInteraction();

    if(eRole == CLIENT){
        Sharing->AssignConversionShares();
    }
    
	PartyChan->synchronize_end();
	delete PartyChan;

	return true;
}

BOOL B2AParty::PerformInteraction() {
	WakeupWorkerThreads(e_Party_Comm);
	BOOL success = WaitWorkerThreads();
	return success;
}

BOOL B2AParty::ThreadSendValues(uint32_t id) {
	std::vector<BYTE*> sendbuf;
	std::vector<uint64_t> sndbytes;

	Sharing->GetDataToSend(sendbuf, sndbytes);

	uint64_t snd_buf_size_total = 0, ctr = 0;

	for(uint32_t i = 0; i < sendbuf.size(); i++){
		snd_buf_size_total += sndbytes[i];
	}

	uint8_t* snd_buf_total = (uint8_t*) malloc(snd_buf_size_total);

	for (uint32_t i = 0; i < sendbuf.size(); i++) {
		if(sndbytes[i] > 0) {
			memcpy(snd_buf_total+ctr, sendbuf[i], sndbytes[i]);
			ctr+= sndbytes[i];
		}
	}

	//gettimeofday(&tstart, NULL);
	if(snd_buf_size_total > 0) {
		PartyChan->blocking_send(vThreads[id]->GetEvent(), snd_buf_total, snd_buf_size_total);
	}

	free(snd_buf_total);
	sendbuf.clear();
	sndbytes.clear();

	return true;
}

BOOL B2AParty::ThreadReceiveValues() {
	std::vector<BYTE*> rcvbuf;
	std::vector<uint64_t> rcvbytes;

	uint64_t rcvbytestotal = 0, ctr = 0;

	Sharing->GetBuffersToReceive(rcvbuf, rcvbytes);

	for(uint32_t i = 0; i < rcvbuf.size(); i++){
		rcvbytestotal += rcvbytes[i];
	}

	uint8_t* rcvbuftotal = (uint8_t*) malloc(rcvbytestotal);
	assert(rcvbuftotal != NULL);

	if (rcvbytestotal > 0) {
		PartyChan->blocking_receive(rcvbuftotal, rcvbytestotal);
	}

	for (uint32_t i = 0; i < rcvbuf.size(); i++) {
		if (rcvbytes[i] > 0) {
			memcpy(rcvbuf[i], rcvbuftotal + ctr, rcvbytes[i]);
			ctr += rcvbytes[i];
		}
	}

	free(rcvbuftotal);

	rcvbuf.clear();
	rcvbytes.clear();

	return true;
}

//=========================================================
// Connection Routines
BOOL B2AParty::EstablishConnection() {
	BOOL success = false;
	if (eRole == SERVER) {
		/*#ifndef BATCH
		 std::cout << "Server starting to listen" << std::endl;
		 #endif*/
		success = B2APartyListen();
	} else { //CLIENT
		success = B2APartyConnect();

	}
	if (!success)
		return false;

	Comm->snd_std = std::make_unique<SndThread>(Sockets[0].get(), glock.get());
	Comm->rcv_std = std::make_unique<RcvThread>(Sockets[0].get(), glock.get());

	Comm->snd_inv = std::make_unique<SndThread>(Sockets[1].get(), glock.get());
	Comm->rcv_inv = std::make_unique<RcvThread>(Sockets[1].get(), glock.get());

	Comm->snd_std->Start();
	Comm->snd_inv->Start();

	Comm->rcv_std->Start();
	Comm->rcv_inv->Start();
	return true;
}

//Interface to the connection method
BOOL B2AParty::B2APartyConnect() {
	//Will open Sockets.size new sockets to
	return Connect(Address, Port, Sockets, static_cast<uint32_t>(eRole));
}

//Interface to the listening method
BOOL B2AParty::B2APartyListen() {
	std::vector<std::vector<std::unique_ptr<CSocket>> > tempsocks(2);

	for(uint32_t i = 0; i < 2; i++) {
		tempsocks[i].resize(Sockets.size());
	}

	bool success = Listen(Address, Port, tempsocks, Sockets.size(), static_cast<uint32_t>(eRole));
	for(uint32_t i = 0; i < Sockets.size(); i++) {
		Sockets[i] = std::move(tempsocks[1][i]);
	}
	return success;
}

void B2AParty::Reset() {
	Setup->Reset();
	Sharing->Reset();
}

double B2AParty::GetTiming(ABYPHASE phase) {
	return GetTimeForPhase(phase);
}

uint64_t B2AParty::GetSentData(ABYPHASE phase) {
	return GetSentDataForPhase(phase);
}

uint64_t B2AParty::GetReceivedData(ABYPHASE phase) {
	return GetReceivedDataForPhase(phase);
}

//===========================================================================
// Thread Management
BOOL B2AParty::WakeupWorkerThreads(EPartyJobType e) {
	WorkerThreadSuccess = TRUE;

	nWorkingThreads = 2;
	uint32_t n = nWorkingThreads;

	for (uint32_t i = 0; i < n; i++)
		vThreads[i]->PutJob(e);

	return TRUE;
}

BOOL B2AParty::WaitWorkerThreads() {
	{
		std::lock_guard<CLock> lock(*m_lock);
		if (!nWorkingThreads)
			return TRUE;
	}

	for (;;) {
		m_lock->Lock();
		uint32_t n = nWorkingThreads;
		m_lock->Unlock();
		if (!n)
			return WorkerThreadSuccess;
		m_evt->Wait();
	}
	return WorkerThreadSuccess;
}

BOOL B2AParty::ThreadNotifyTaskDone(BOOL bSuccess) {
	m_lock->Lock();
	uint32_t n = --nWorkingThreads;
	if (!bSuccess)
		WorkerThreadSuccess = FALSE;
	m_lock->Unlock();

	if (!n)
		m_evt->Set();
	return TRUE;
}

void B2AParty::CPartyWorkerThread::ThreadMain() {
	BOOL bSuccess = FALSE;
	for (;;) {
		m_evt.Wait();

		EPartyJobType job;
		{
			std::lock_guard<std::mutex> lock(m_eJob_mutex_);
			job = m_eJob;
		}

		switch (job) {
		case e_Party_Stop:
			return;
		case e_Party_Comm:
			if (threadid == 0){
				bSuccess = m_pCallback->ThreadSendValues(threadid);
			}
			else{
				bSuccess = m_pCallback->ThreadReceiveValues();
			}
			break;
		case e_Party_Undefined:
		default:
			std::cerr << "Error: Unhandled Thread Job!" << std::endl;
		}

		m_pCallback->ThreadNotifyTaskDone(bSuccess);
	}
}

BOOL B2AParty::InitConv(uint32_t bitlen){
	Sharing = new B2Asharing(eRole, bitlen, Crypt.get());
}

B2Asharing* B2AParty::GetSharing() {
		return Sharing;
}



