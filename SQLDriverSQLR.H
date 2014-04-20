#ifndef SQLDRIVERSQLR_H
#define SQLDRIVERSQLR_H

#include <sqlrelay/sqlrclient.h>

namespace raii {
namespace sql {

DECLARE_EXCEPTION(SQLrelayException,SQLException);

class ResultSetImplSQLR;

class SQLDriverSQLR: public SQLDriver {

	bool autoCommit;
	//sqlrconnection *conn;

	public:
	SQLDriverSQLR(const String& host, int port, const String& path, const String& user, const String& password);


	virtual ~SQLDriverSQLR();


	virtual ptr<ResultSetImpl> query(const String& q);
	virtual bool isAutoCommitOn();
	virtual void autoCommitOn();

	virtual void autoCommitOff();

	virtual void commit();

	virtual void rollback();

	virtual String sqlize(const String& str);

        virtual String identifyDriver();
};


}
}


#endif
