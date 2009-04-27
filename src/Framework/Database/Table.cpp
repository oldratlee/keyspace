#include <db_cxx.h>

#include "Table.h"
#include "Database.h"
#include "System/Buffer.h"
#include "Transaction.h"

Table::Table(Database* database, const char *name, int pageSize) :
database(database)
{
	DbTxn *txnid = NULL;
	const char *filename = name;
	const char *dbname = NULL;
	DBTYPE type = DB_BTREE;
	u_int32_t flags = DB_CREATE | DB_AUTO_COMMIT;
	int mode = 0;
	
	db = new Db(&database->env, 0);
	if (pageSize != 0)
		db->set_pagesize(pageSize);
	db->open(txnid, filename, dbname, type, flags, mode);
}

Table::~Table()
{
	db->close(0);
	delete db;
}

bool Table::Iterate(Transaction* transaction, Cursor& cursor)
{
	DbTxn* txn = NULL;
	
	if (transaction)
		txn = transaction->txn;
	
	if (db->cursor(txn, &cursor.cursor, 0) == 0)
		return true;
	else
		return false;
}

bool Table::Get(Transaction* transaction, const ByteString &key, ByteString &value)
{
	Dbt dbtKey(key.buffer, key.length);
//	Dbt dbtValue(value.buffer, value.size);
	Dbt dbtValue;
	DbTxn* txn = NULL;
	int ret;
	
	if (transaction)
		txn = transaction->txn;

//	dbtKey.set_flags(DB_DBT_USERMEM);
//	dbtValue.set_flags(DB_DBT_USERMEM);		
	ret = db->get(txn, &dbtKey, &dbtValue, 0);
	if (ret == DB_KEYEMPTY || ret == DB_NOTFOUND)
		return false;
	
	if (dbtValue.get_size() > (size_t) value.size)
		return false;

	value.length = dbtValue.get_size();
	memcpy(value.buffer, dbtValue.get_data(), value.length);
	
	return true;
}

bool Table::Get(Transaction* transaction, const char* key, ByteString &value)
{
	int len;
	
	len = strlen(key);
	
	ByteString bsKey(len, len, (char*) key);
	
	return Table::Get(transaction, bsKey, value);
}

bool Table::Set(Transaction* transaction, const ByteString &key, const ByteString &value)
{
	Dbt dbtKey(key.buffer, key.length);
	Dbt dbtValue(value.buffer, value.length);
	DbTxn* txn = NULL;
	int ret;

	if (transaction)
		txn = transaction->txn;
	
	ret = db->put(txn, &dbtKey, &dbtValue, 0);
	if (ret < 0)
		return false;
	
	return true;
}

bool Table::Set(Transaction* transaction, const char* key, const ByteString &value)
{
	int len;
	
	len = strlen(key);
	
	ByteString bsKey(len, len, (char*) key);
	
	return Table::Set(transaction, bsKey, value);
}

bool Table::Set(Transaction* transaction, const char* key, const char* value)
{
	int len;
	
	len = strlen(key);
	ByteString bsKey(len, len, (char*) key);
	
	len = strlen(value);
	ByteString bsValue(len, len, (char*) value);
	
	return Table::Set(transaction, bsKey, bsValue);
}

bool Table::Delete(Transaction* transaction, const ByteString &key)
{
	Dbt dbtKey(key.buffer, key.length);
	DbTxn* txn = NULL;
	int ret;

	if (transaction)
		txn = transaction->txn;
	
	ret = db->del(txn, &dbtKey, 0);
	if (ret < 0)
		return false;
	
	return true;
}

bool Table::Visit(TableVisitor &tv)
{
	Dbc* cursor = NULL;
	bool ret = true;
	u_int32_t flags = DB_NEXT;

	// TODO call tv.OnComplete() or error handling
	if (db->cursor(NULL, &cursor, 0) != 0)
		return false;
	
	Dbt key, value;
	if (tv.GetKeyHint())
	{
		key.set_data(tv.GetKeyHint()->buffer);
		key.set_size(tv.GetKeyHint()->length);
		flags = DB_SET_RANGE;		
	}
	
	while (cursor->get(&key, &value, flags) == 0)
	{
		ByteString bsKey(key.get_size(), key.get_size(), (char*) key.get_data());
		ByteString bsValue(value.get_size(), value.get_size(), (char*) value.get_data());

		ret = tv.Accept(bsKey, bsValue);
		if (!ret)
			break;

		flags = DB_NEXT;
	}
	
	if (cursor)
		cursor->close();
	
	tv.OnComplete();
	
	return ret;
}


bool Table::Truncate(Transaction* transaction)
{
	u_int32_t count;
	u_int32_t flags = 0;
	int ret;
	DbTxn* txn;
	
	txn = transaction ? transaction->txn : NULL;
	// TODO error handling
	ret = db->truncate(txn, &count, flags);
	
	return true;
}
