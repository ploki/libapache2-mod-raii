#ifndef RAIIRESULTSET_H
#define RAIIRESULTSET_H

#include "Exception.H"
#include "Connection.H"


namespace raii {
namespace sql {

class ResultSet : public Object {

	Connection& conn;
	ptr<ResultSetImpl> impl;

        public:

	ResultSet(Connection& conn_,const String& q);

	ResultSet(const ResultSet& rs);

	ResultSet& operator=(const ResultSet& rs);

	void sendQuery(const String& query);

	int rowCount();

	bool next();
	bool previous();

	void first();

	void last();

	String operator [] (const String& attrName);

};

}
}

#endif
