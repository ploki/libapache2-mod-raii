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

