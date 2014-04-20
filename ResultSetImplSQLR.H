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

