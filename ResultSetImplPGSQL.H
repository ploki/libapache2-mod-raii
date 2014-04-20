#ifndef RAIIRESULTSETIMPLPGSQL_H
#define RAIIRESULTSETIMPLPGSQL_H

#include<libpq-fe.h>

namespace raii {
namespace sql {

class ResultSetImplPGSQL : public ResultSetImpl {

	PGresult *result;
	//interdit
	ResultSetImplPGSQL(const ResultSetImpl& rsi);

	Map<String,int> tupleIndex;
	public:

	ResultSetImplPGSQL(SQLDriverPGSQL& driver, const String& q)
		:	ResultSetImpl(driver,q),
			result(NULL),
			tupleIndex()
			{

                sendQuery(q);
	}

	virtual ~ResultSetImplPGSQL() {

		if ( result )
			PQclear(result);
	}

	virtual void sendQuery(const String& query) {

		cursor=-1;

		if ( result ) PQclear(result);
		result=PQexec((PGconn*)driver.conn,query.c_str());
		if ( result == NULL ) {
			raii::Logger log("raii::sql");
			log.error(String("Attempted query was: ")+query);
			throw PostgreSQLException("result is null");
		}
		if (	( PQresultStatus(result) != PGRES_COMMAND_OK )
		     && ( PQresultStatus(result) != PGRES_TUPLES_OK) ) {
			Logger log("raii::sql");
			log.error(String("Attempted query was: ")+query);
			throw PostgreSQLException(PQerrorMessage((PGconn*)driver.conn));
		}
		
		tupleIndex.clear();
		int nfields = PQnfields(result);
		for ( int i = 0 ; i < nfields ; ++i ) {
			tupleIndex[PQfname(result,i)]=i;
		}
	}

	virtual int rowCount() {

		return PQntuples(result);
	}

	virtual String operator [] (const String& attrName) {

		Map<String,int>::iterator pair = tupleIndex.find(attrName);
		if ( pair == tupleIndex.end() )
			throw PostgreSQLException("Field "+attrName+" doesn't exist.");
		if ( cursor >= 0 && cursor < rowCount() )
			return PQgetvalue(result,cursor,pair->second);
		throw PostgreSQLException("Cursor out of bound");
	}

};

}
}

#endif

