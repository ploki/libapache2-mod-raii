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

	ConnectionProbe::ConnectionProbe(bool b)
        	: access(), count(0), request(), requestDuration(), requestRowCount(), total(), trace(b) {

                timerclear(&total);
	}
	
	
	timeval ConnectionProbe::startTimer(const String& query) {
                timeval start;
                Lock l(access);
	        ++count;
	        if ( trace )
	                request.push_back(query);
                gettimeofday(&start,NULL);
                return start;

	}
	
	
	void ConnectionProbe::stopTimer(const timeval& start, int rowCount) {

		timeval now;
		gettimeofday(&now,NULL);
		timeval res;
		timersub(&now,&start,&res);
		{ Lock l(access);
		        timeradd(&total,&res,&total);

                        if ( trace ) {
                                requestRowCount.push_back(rowCount);
                                requestDuration.push_back( double(res.tv_sec) + res.tv_usec / 1000000. );
                        }
		}
	
	}


        void ConnectionProbe::reset() {
                Lock l(access);
                count=0;
                request.clear();
                requestDuration.clear();
                requestRowCount.clear();
                timerclear(&total);
        }


	const char* Connection::getDefaultURI() {

		RaiiConfig *cfg=get_dconfig(apacheRequest);
		return cfg->sqlconnection?cfg->sqlconnection:"";
	}

	const char* Connection::getScheme(const String& uri) {

		apr_uri_t parsed_uri;
		apr_uri_parse(apacheRequest->pool,uri.c_str(),&parsed_uri);
		return parsed_uri.scheme?parsed_uri.scheme:"";
	}

	const char* Connection::getHost(const String& uri) {

		apr_uri_t parsed_uri;
		apr_uri_parse(apacheRequest->pool,uri.c_str(),&parsed_uri);
		return parsed_uri.hostname?parsed_uri.hostname:"";
	}

	int Connection::getPort(const String& uri) {

		apr_uri_t parsed_uri;
		apr_uri_parse(apacheRequest->pool,uri.c_str(),&parsed_uri);
		return parsed_uri.port;
	}

	const char* Connection::getPath(const String& uri){

		apr_uri_t parsed_uri;
		apr_uri_parse(apacheRequest->pool,uri.c_str(),&parsed_uri);
		return parsed_uri.path?parsed_uri.path:"";
	}

	const char* Connection::getUser(const String& uri){

		apr_uri_t parsed_uri;
		apr_uri_parse(apacheRequest->pool,uri.c_str(),&parsed_uri);
		return parsed_uri.user?parsed_uri.user:"";
	}

	const char* Connection::getPassword(const String& uri) {

		apr_uri_t parsed_uri;
		apr_uri_parse(apacheRequest->pool,uri.c_str(),&parsed_uri);
		return parsed_uri.password?parsed_uri.password:"";
	}

	const char* Connection::getQuery(const String& uri) {

		apr_uri_t parsed_uri;
		apr_uri_parse(apacheRequest->pool,uri.c_str(),&parsed_uri);
		return parsed_uri.query?parsed_uri.query:"";
	}


        ptr<Map<String, ptr<Vector<ptr<SQLDriver> > > > > Connection::getPools() {
		static ptr<Map<String, ptr<Vector<ptr<SQLDriver> > > > > pools=NULL;

		if ( !pools ) {
			pools = new Map<String, ptr<Vector<ptr<SQLDriver> > > >();
                }
                return pools;

        }

        Mutex& Connection::getPoolsMutex() {
		static Mutex mutex;
                return mutex;
        }

        time_t& Connection::getLastCleanup() {
                static time_t lastCleanup = time(NULL);
                return lastCleanup;
        }

        void Connection::clearPools() {
                ptr<Map<String, ptr<Vector<ptr<SQLDriver> > > > > pools=getPools();
		Mutex& mutex=getPoolsMutex();
		Lock l1(mutex);
                pools->clear();
        }

	ptr<SQLDriver> Connection::getConnection(const String& uri) {

		time_t& lastCleanup = getLastCleanup();
		ptr<Map<String, ptr<Vector<ptr<SQLDriver> > > > > pools=getPools();
                Mutex& mutex=getPoolsMutex();


		String scheme = getScheme(uri);
		String user = getUser(uri);
		String password = getPassword(uri);
		String host = getHost(uri);
		int port = getPort(uri);
		String path = getPath(uri);
		String query = getQuery(uri);


                // s'il s'agit d'une connexion sqlrelay ou sqlite, pas besoin de passer par les pools

                ptr<SQLDriverContainer> driverContainer = SQLDriverContainer::getSQLDriverContainer(scheme);

                if ( ! driverContainer->isPoolable() ) {
                        ptr<SQLDriver> sqlDriver = driverContainer->getDriver(user,password,host,port,path,query);
                        return sqlDriver;
                }

		Lock l1(mutex);

		Map<String, ptr<Vector<ptr<SQLDriver> > > >::iterator  pool_it = pools->find(uri);

		if  ( pool_it == pools->end() ) {
			// création d'un nouveau pool
			Logger log("raii::sql::Connection");
			log.debug();
			log("Creating a pool for "+scheme+"://"+ ( !user.empty()?user+":******@":"" )
			                + ( !host.empty() ? host : "" )
			                + ( port?String(":")+itostring(port):"" )
			                + ( !path.empty() ? path : "/" )
			                + ( !query.empty() ? String("?")+query : "" ) );
			(*pools)[uri] = new Vector<ptr<SQLDriver> >();
			pool_it = pools->find(uri);
		}

		//recup d'un pool
		ptr<Vector<ptr<SQLDriver> > > pool = pool_it->second;

        	time_t now = time(NULL);
                bool cleanup = lastCleanup  +  60 < now ;
                if ( cleanup )
                        lastCleanup = now;

		int size = pool->size();
		int cleaned = 0;

		Vector<ptr<SQLDriver> >::iterator theConn = pool->end();
		for (	Vector<ptr<SQLDriver> >::iterator aConn = pool->begin() ;
			aConn != pool->end() ;
			/*nop*/) {
			if ( (*aConn)->busy ) {
				++aConn;
				continue;
			}
			else if ( theConn == pool->end() ) {
				theConn = aConn++;
                                time(&(*theConn)->lastAccess);
                                if ( !cleanup )
                                        break;
                        }
			else if ( (* aConn)->lastAccess + 2*60 < now ) {
				++cleaned;
				aConn = pool->erase(aConn);
                        }
			else {
				++aConn;
                        }
		}

		if ( cleaned ) {
			Logger log("raii::sql::Connection");
			log.debug();
			log("Cleaned "+itostring(cleaned)+" out of "+itostring(size)+" connections in "
			                +scheme+"://"+ ( !user.empty()?user+":******@":"" )
			                + ( !host.empty() ? host : "" )
			                + ( port?String(":")+itostring(port):"" )
			                + ( !path.empty() ? path : "/" )
			                + ( !query.empty() ? String("?")+query : "" ) );
		}

		if ( theConn == pool->end() ) {
			ptr<SQLDriver> newConn = driverContainer->getDriver(user,password,host,port,path,query);
                        if ( !newConn )
				throw SQLException("Unable to get new connection");
			newConn->busy = true;
			pool->push_back( newConn );
			return newConn;
		}
		else {
			(*theConn)->busy = true;
			(*theConn)->lastAccess = time(NULL);
			return *theConn;
		}
	}



	//CTORs
	Connection::Connection(const String& uri, ptr<ConnectionProbe> cp) : Object(), driver(NULL), probe(cp) {

		if ( uri.empty() )
			driver = getConnection(getDefaultURI());
		else
			driver = getConnection(uri);

	}

	Connection::Connection(ptr<ConnectionProbe> cp) : Object(), driver(NULL), probe(cp) {

		driver = getConnection(getDefaultURI());
	}


	Connection::Connection(const Connection& conn) : Object(), driver(conn.driver), probe(conn.probe) {}


        Connection& Connection::operator=(const Connection& conn) {

		driver = conn.driver;
		probe=conn.probe;

		return *this;
	}

	//DTOR
	Connection::~Connection() { driver->release(); }


	ResultSet Connection::query(const String& query) {

                timeval start;
		if ( probe )
		        start=probe->startTimer(query);

                try {
        		ResultSet rs(*this,query);
        		if ( probe ) probe->stopTimer(start,rs.rowCount());
	        	return rs;
	        }
	        catch (...) {
	                if ( probe ) probe->stopTimer(start,-1);
	                throw;
	        }
	}

	bool	Connection::isAutoCommitOn() {

                return driver->isAutoCommitOn();
        }

	void	Connection::autoCommitOn() {

                driver->autoCommitOn();
        }

	void    Connection::autoCommitOff() {

                driver->autoCommitOff();
        }

	void    Connection::commit() {

                driver->commit();
        }

	void    Connection::rollback() {

                driver->rollback();
        }

	String Connection::sqlize(const String& str) {
		return driver->sqlize(str);
	}

        String Connection::identifyDriver() {

                return driver->identifyDriver();
        }
}
}
