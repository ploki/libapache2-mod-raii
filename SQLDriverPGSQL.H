#ifndef SQLDRIVERPGSQL_H
#define SQLDRIVERPGSQL_H

#include<libpq-fe.h>

namespace raii {
namespace sql {

DECLARE_EXCEPTION(PostgreSQLException,SQLException);

class ResultSetImplPGSQL;

class SQLDriverPGSQL: public SQLDriver {

	bool autoCommit;
	//PGconn *conn;

	public:
	SQLDriverPGSQL(const String&  host, const String&  dbname, const String& user, const String& passwd);

	virtual ~SQLDriverPGSQL();

	virtual ptr<ResultSetImpl> query(const String& q);

	virtual bool isAutoCommitOn();

	virtual void autoCommitOn();

	virtual void autoCommitOff();

	virtual void commit();

	virtual void rollback();

	virtual String sqlize(const String& str);

        virtual String identifyDriver() { return "pgsql"; }
};


}
}


#endif
