#include "raii.H"
#include "SQLDriverPGSQL.H"
#include "ResultSetImplPGSQL.H"


namespace raii {
namespace sql {
extern "C" {

bool isPoolable() {
        return true;
}

raii::ptr<raii::sql::SQLDriver> getDriver( const raii::String& user, const raii::String& password,
                                           const raii::String& host, int port,
                                           const raii::String& path, const raii::String& query) {

        return new raii::sql::SQLDriverPGSQL(host,query,user,password);
}

}

	SQLDriverPGSQL::SQLDriverPGSQL(const String&  host, const String&  dbname, const String& user, const String& passwd)
		:	SQLDriver(),
			autoCommit(true)
		{
                conn = PQsetdbLogin(host.c_str(),NULL,NULL,NULL,dbname.c_str(),user.c_str(),passwd.c_str());

		if ( !conn ) {
			throw PostgreSQLException("connexion impossible");
		}
		if ( PQstatus((PGconn*)conn) != CONNECTION_OK ) {
			throw PostgreSQLException(PQerrorMessage((PGconn*)conn));
		}
	}

	SQLDriverPGSQL::~SQLDriverPGSQL() {

		if ( !isAutoCommitOn() )
			rollback();
		if ( conn )
			PQfinish((PGconn*)conn);
	}

	ptr<ResultSetImpl> SQLDriverPGSQL::query(const String& q) {

		return new ResultSetImplPGSQL(*this,q);
	}

	bool SQLDriverPGSQL::isAutoCommitOn() {
		return autoCommit;
	}

	void SQLDriverPGSQL::autoCommitOn() {
		if ( !autoCommit ) {
			query("ROLLBACK");
			autoCommit=true;
		}
	}

	void SQLDriverPGSQL::autoCommitOff() {
		if ( autoCommit ) {
			query("BEGIN");
			autoCommit=false;
		}
	}

	void SQLDriverPGSQL::commit() {
		if ( !autoCommit ) {
			query("COMMIT");
			query("BEGIN");
		}
	}

	void SQLDriverPGSQL::rollback() {
		if ( !autoCommit ) {
			query("ROLLBACK");
			query("BEGIN");
		}
	}

	String SQLDriverPGSQL::sqlize(const String& str) {

		StringStream res;
		for ( size_t i = 0 ; i < str.size() ; i++ ) {
			res << str[i];
			if ( str[i] == '\'' )
				res << "'";
			if ( str[i] == '\\' )
				res << "\\";
		}
		return res.str();
	}


}
}
