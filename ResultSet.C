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

namespace raii {
namespace sql {

	ResultSet::ResultSet(Connection& conn_,const String& q)

		: 	Object(),
			conn(conn_),
			impl(conn.driver->query(q))
		{}

	ResultSet::ResultSet(const ResultSet& rs)

		:	Object(),
			conn(rs.conn),
			impl(rs.impl)
		{}

	ResultSet& ResultSet::operator=(const ResultSet& rs) {

                if ( &conn != &rs.conn )
                        throw SQLException("Connection <=> ResultSet missmatch");
		//conn=rs.conn;
		impl=rs.impl;
		return *this;
	}

	void ResultSet::sendQuery(const String& query) {

                timeval start;
                if ( conn.probe ) start=conn.probe->startTimer(query);
                try {
        		impl->sendQuery(query);
        		if ( conn.probe ) conn.probe->stopTimer(start,rowCount());
        	}
        	catch (...) {
        		if ( conn.probe ) conn.probe->stopTimer(start,rowCount());
        		throw;
        	}
	}

	int ResultSet::rowCount() {

                int rc = impl->rowCount();
		return rc;
	}

	bool ResultSet::next() {

                //manipulation locale, pas de timing
                return impl->next();
	}

	bool ResultSet::previous() {

                //manipulation locale, pas de timing
		return impl->previous();

	}

	void ResultSet::first() {

                //manipulation locale, pas de timing
		return impl->first();
	}

	void ResultSet::last() {

                //manipulation locale, pas de timing
		return impl->last();
	}


	String ResultSet::operator [] (const String& attrName) {

                String value = (*impl)[attrName];
		return value;
	}


}
}

