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
 * ARE DISCLAIMED. IN NO EVENT SHALL G.GIMENEZ BE LIABLE FOR ANY DIRECT,
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
#ifndef RAIIRESULTSETIMPLSQLR_H
#define RAIIRESULTSETIMPLSQLR_H

namespace raii {
namespace sql {

class ResultSetImplSQLR : public ResultSetImpl {

	sqlrcursor *_cur;
	//interdit
	ResultSetImplSQLR(const ResultSetImpl& rsi);


	public:

	ResultSetImplSQLR(SQLDriverSQLR& driver, const String& q) : ResultSetImpl(driver,q), _cur(NULL) {
		_cur = new sqlrcursor((sqlrconnection*)driver.conn);
                sendQuery(q);
	}

	virtual ~ResultSetImplSQLR() {

		delete _cur;
	}

	virtual void sendQuery(const String& query) {

		cursor=-1;

                if ( _cur->sendQuery(query.c_str()) == 0 ) {
                        Logger log("raii::sql");
		      log.error(String("Attempted query was: ")+query);
		      throw SQLrelayException(_cur->errorMessage());
	       }
        }

	virtual int rowCount() {

		return _cur->rowCount();
	}

	virtual String operator [] (const String& attrName) {

		if ( cursor >= 0 && cursor < rowCount() )
			return _cur->getField(cursor,attrName.c_str());//?:"(null)";
		throw SQLrelayException("Cursor out of bound");
	}

};

}
}

#endif

