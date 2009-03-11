#ifndef REPLICATEDDB_H
#define REPLICATEDDB_H

#include "Framework/Database/Transaction.h"
#include "Framework/ReplicatedLog/LogQueue.h"

class Entry;

class ReplicatedDB
{
public:
	virtual void OnAppend(Transaction* transaction, ulong64 paxosID, ByteString value) = 0;
	
	virtual void OnMasterLease(int nodeID) = 0;
	virtual void OnMasterLeaseExpired() = 0;
	
	virtual void OnDoCatchup() = 0;
	
	virtual void OnStop() = 0;
	
	virtual void OnContinue() = 0;
};

#endif
