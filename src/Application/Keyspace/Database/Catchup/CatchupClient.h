#ifndef CATCHUPCLIENT_H
#define CATCHUPCLIENT_H

#include "Framework/Transport/TCPConn.h"
#include "Framework/Database/Table.h"
#include "Framework/Database/Transaction.h"
#include "Application/Keyspace/Database/KeyspaceConsts.h"
#include "CatchupMsg.h"

#define CATCHUP_CONNECT_TIMEOUT	2000

class KeyspaceDB;

class CatchupClient : public TCPConn<>
{
public:
	void						Init(KeyspaceDB* keyspaceDB_, Table* table_);

	void						Start(unsigned nodeID);
	void						OnRead();
	void						OnClose();
	virtual void				OnConnect();
	virtual void				OnConnectTimeout();

private:
	void						ProcessMsg();
	void						OnKeyValue();
	void						OnCommit();

	Table*						table;
	CatchupMsg					msg;
	ulong64						paxosID;
	Transaction					transaction;
	KeyspaceDB*					keyspaceDB;
};

#endif