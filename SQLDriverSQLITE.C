#include "raii.H"
#include "SQLDriverSQLITE.H"
#include "ResultSetImplSQLITE.H"


namespace raii {
namespace sql {

extern "C" {

bool isPoolable() {
        // si non poolable génère trop de fichiers ouverts...
        return true;
}

raii::ptr<raii::sql::SQLDriver> getDriver( const raii::String& user, const raii::String& password,
                                           const raii::String& host, int port,
                                           const raii::String& path, const raii::String& query) {

        return new raii::sql::SQLDriverSQLITE(path);
}

}
	SQLDriverSQLITE::SQLDriverSQLITE(const String& path)
		:	SQLDriver(),
			autoCommit(true),
			errMessage(NULL)
			{

		conn = sqlite_open(path.c_str(),0,&errMessage);
		if ( !conn ) {
			String error("No error reported");
			if ( errMessage) {
				error=errMessage;
				free(errMessage);
				errMessage=NULL;
			}
			throw SQLiteException(error.c_str());
		}
		else {
			sqlite_busy_timeout((sqlite*)conn,10000);
		}
	}

	SQLDriverSQLITE::~SQLDriverSQLITE() {

		if ( !isAutoCommitOn() )
			rollback();
		if ( conn )
			sqlite_close((sqlite*)conn);
		if ( errMessage ) {
			free(errMessage);
			errMessage=NULL;
		}
	}

	ptr<ResultSetImpl> SQLDriverSQLITE::query(const String& q) {

		return new ResultSetImplSQLITE(*this,q);
	}

	bool SQLDriverSQLITE::isAutoCommitOn() {
		return autoCommit;
	}

	void SQLDriverSQLITE::autoCommitOn() {
		if ( !autoCommit ) {
			query("ROLLBACK");
			autoCommit=true;
		}
	}

	void SQLDriverSQLITE::autoCommitOff() {
		if ( autoCommit ) {
			query("BEGIN");
			autoCommit=false;
		}
	}

	void SQLDriverSQLITE::commit() {
		if ( !autoCommit ) {
			query("COMMIT");
			query("BEGIN");
		}
	}

	void SQLDriverSQLITE::rollback() {
		if ( !autoCommit ) {
			query("ROLLBACK");
			query("BEGIN");
		}
	}

	String SQLDriverSQLITE::sqlize(const String& str) {

		StringStream res;
		for ( size_t i = 0 ; i < str.size() ; i++ ) {
			res << str[i];
			if ( str[i] == '\'' )
				res << "'";
		}
		return res.str();
	}

}
}
