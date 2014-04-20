#ifndef SQLDRIVER_H
#define SQLDRIVER_H

namespace raii {
namespace sql {

class ResultSetImpl;

class SQLDriver : public AttributeHolder {

	public:
	bool busy;
	time_t lastAccess;
        void *conn;

	SQLDriver() : Object(), busy(true), lastAccess() {
		time(&lastAccess);
	}
	virtual ~SQLDriver() {}

	virtual void release() {
                if ( ! this->isAutoCommitOn() ) {
                        this->rollback();
                        this->autoCommitOn();
                }
                busy=false;
        }

	virtual ptr<ResultSetImpl> query(const String& query)=0;

	virtual bool isAutoCommitOn()=0;
	virtual void autoCommitOn()=0;
	virtual void autoCommitOff()=0;
	virtual void commit()=0;
	virtual void rollback()=0;
	virtual String sqlize(const String& str)=0;
        virtual String identifyDriver()=0;

};


}
}


#endif
