#ifndef RAIIRESULTSETIMPL_H
#define RAIIRESULTSETIMPL_H

namespace raii {
namespace sql {

class ResultSetImpl : public virtual Object {

        protected:
	SQLDriver& driver;
	String query;
	int cursor;

        private:
	//interdit
	ResultSetImpl(const ResultSetImpl& rsi);
	public:

	ResultSetImpl(SQLDriver& driver_, const String &q) : Object(), driver(driver_), query(q), cursor(-1) {}

	virtual bool next();

	virtual bool previous();

	virtual void first();

	virtual void last();


	//à surcharger !!!

	virtual ~ResultSetImpl() {}

	virtual void sendQuery(const String& query)=0;

	virtual int rowCount()=0;


	virtual String operator [] (const String& attrName)=0;


};

}
}

#endif
