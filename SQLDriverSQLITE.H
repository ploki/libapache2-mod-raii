#ifndef SQLDRIVERSQLITE_H
#define SQLDRIVERSQLITE_H

#include <sqlite.h>

namespace raii {
namespace sql {

DECLARE_EXCEPTION(SQLiteException,SQLException);

class ResultSetImplSQLITE;

class SQLDriverSQLITE : public SQLDriver {

	bool autoCommit;
	//sqlite *conn;
	char *errMessage;

	public:
	SQLDriverSQLITE(const String& path);

	virtual ~SQLDriverSQLITE();

	virtual ptr<ResultSetImpl> query(const String& q);

	virtual bool isAutoCommitOn();

	virtual void autoCommitOn();

	virtual void autoCommitOff();

	virtual void commit();

	virtual void rollback();

	virtual String sqlize(const String& str);

        virtual String identifyDriver() { return "sqlite"; }

};


}
}


#endif
