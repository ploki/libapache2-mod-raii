#ifndef RAIICONNECTION_H
#define RAIICONNECTION_H

#include "Object.H"
#include "Exception.H"

namespace raii {
namespace sql {

class ResultSet;

class ConnectionProbe : public Object {
        public:
        Mutex access;
        int count;
        Vector<String> request;
        Vector<double> requestDuration;
        Vector<int> requestRowCount;
        timeval total;
        bool trace;
	
	ConnectionProbe(bool b=false);
	void reset();
	
	timeval startTimer(const String& query);
	void stopTimer(const timeval& start, int rowCount);


};


class Connection : public virtual Object {


	// private static
	static const char* getDefaultURI();

	static const char* getScheme(const String& uri);

	static const char* getHost(const String& uri);

	static  int getPort(const String& uri);

	static const char* getPath(const String& uri);

	static const char* getUser(const String& uri);

	static const char* getPassword(const String& uri);

	static const char* getQuery(const String& uri);

        public:
        static ptr<Map<String, ptr<Vector<ptr<SQLDriver> > > > > getPools();
        static Mutex& getPoolsMutex();
        static time_t& getLastCleanup();
        static void clearPools();
        private:
	static ptr<SQLDriver> getConnection(const String& uri);


	public:

	//ATTRs

	ptr<SQLDriver> driver;
	ptr<ConnectionProbe> probe;


	//CTORs
	Connection(const String& uri, ptr<ConnectionProbe> cp=NULL);

	Connection(ptr<ConnectionProbe> cp=NULL);

	Connection(const Connection& conn);

	Connection& operator=(const Connection& conn);

	//DTOR
	~Connection();


	ResultSet query(const String& query);

	bool	isAutoCommitOn();

	void	autoCommitOn();

	void    autoCommitOff();

	void    commit();

	void    rollback();

	String sqlize(const String& str);

        String  identifyDriver();

};

}
}

#endif

