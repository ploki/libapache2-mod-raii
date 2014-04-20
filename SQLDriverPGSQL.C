/* 
 * Copyright (c) 2005-2011, Guillaume Gimenez <guillaume@blackmilk.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of G.Gimenez nor the names of its contributors may
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL G.Gimenez SA BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors:
 *     * Guillaume Gimenez <guillaume@blackmilk.fr>
 *
 */
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
