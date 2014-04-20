#ifndef SQLBEGIN_H
#define SQLBEGIN_H

namespace raii {
namespace sql {

class Begin {
	Connection& conn;
	typedef enum {	READ_COMMITTED,
			SERIALIZABLE,
			REPEATABLE_READ,
			READ_UNCOMMITTED,
			READ_WRITE,
			READ_ONLY } IsolationLevel;

	IsolationLevel isolation;

	//mis à true si la connection est déjà en autoCommitOff
	bool noop;
	bool factice;
	bool used;

	void setIsolationLevel();
	public:
	void resetIsolationLevel();
	void setIsolationLevelSerializable();
	void setIsolationLevelRepeatableRead();
	void setIsolationLevelReadCommitted();
	void setIsolationLevelReadUncommitted();
	void setIsolationLevelReadWrite();
	void setIsolationLevelReadOnly();

	Begin(Connection& c, bool f=false);

	void commit();
	void rollback();

	virtual ~Begin();
};


}}
#endif

