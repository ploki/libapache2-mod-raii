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

