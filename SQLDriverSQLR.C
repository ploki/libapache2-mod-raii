#include "raii.H"
#include "SQLDriverSQLR.H"
#include "ResultSetImplSQLR.H"


namespace raii {
namespace sql {

extern "C" {

bool isPoolable() {
        return false;
}

raii::ptr<raii::sql::SQLDriver> getDriver( const raii::String& user, const raii::String& password,
                                           const raii::String& host, int port,
                                           const raii::String& path, const raii::String& query) {

        return new raii::sql::SQLDriverSQLR(host,port,path,user,password);
}

}
	SQLDriverSQLR::SQLDriverSQLR(const String& host, int port, const String& path, const String& user, const String& password)
		:	SQLDriver(),
			autoCommit(true)
		{

		conn= new sqlrconnection(host.c_str(),port,path.c_str(),user.c_str(),password.c_str(),1,1);
		if ( !conn )
			throw SQLrelayException("connexion impossible");

                //STRANGE !!! si on ne lance pas une requête à ce moment, ça ne marche poa...
                query("SELECT 1");
	}




	SQLDriverSQLR::~SQLDriverSQLR() {

		if ( !isAutoCommitOn() )
			rollback();
		delete (sqlrconnection*)conn;
	}


	ptr<ResultSetImpl> SQLDriverSQLR::query(const String& q) {

		return new ResultSetImplSQLR(*this,q);
	}

	bool SQLDriverSQLR::isAutoCommitOn() {
		return autoCommit;
	}

	void SQLDriverSQLR::autoCommitOn() {
		if ( !autoCommit ) {
			query("ROLLBACK");
			autoCommit=true;
		}
	}

	void SQLDriverSQLR::autoCommitOff() {
		if ( autoCommit ) {
			query("BEGIN");
			autoCommit=false;
		}
	}

	void SQLDriverSQLR::commit() {
		if ( !autoCommit ) {
			query("COMMIT");
			query("BEGIN");
		}
	}

	void SQLDriverSQLR::rollback() {
		if ( !autoCommit ) {
			query("ROLLBACK");
			query("BEGIN");
		}
	}

        String SQLDriverSQLR::identifyDriver() {

                String identification = "sqlrelay(";

                const char * sqlrID = ((sqlrconnection*)conn)->identify();
                if ( sqlrID )
                        identification += sqlrID;
                else
                        identification += "null";
                identification += ")";
                return identification;
        }

	String SQLDriverSQLR::sqlize(const String& str) {

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
