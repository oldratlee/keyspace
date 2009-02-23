#ifndef ASYNCDATABASEOP_H
#define ASYNCDATABASEOP_H

#include "System/Buffer.h"
#include "System/Events/Callable.h"

class Table;
class Transaction;
class ByteString;
class ThreadPool;

class DatabaseOp
{
public:
	enum Operation {
		GET,
		PUT
	};
	
	Operation			op;
	Table*				table;
	ByteString			key;
	ByteString			value;
	bool				ret;
};

class MultiDatabaseOp
{
public:
	MultiDatabaseOp();
	
	void					Init();

	bool					Get(Table* table, const ByteString& key, ByteString& value);
	bool					Put(Table* table, const ByteString& key, ByteString& value);
	bool					Put(Table* table, char* key, ByteString &value);
	bool					Put(Table* table, char* key, char* value);

	void					SetTransaction(Transaction* tx = 0);

	void					SetCallback(Callable* userCallback);
	Callable*				GetOperation();
	const ByteString*		GetKey(int i);
	ByteString*				GetValue(int i);
	bool					GetReturnValue(int i);
	int						GetNumOp();

	bool					active;
	
private:
	DatabaseOp				ops[1024];
	int						numop;
	
	Callable*				userCallback;
	Transaction*			tx;
	
	void					OnComplete();
	MFunc<MultiDatabaseOp>	onComplete;
	
	void					Operation();
	MFunc<MultiDatabaseOp>	operation;
};
										

#endif
