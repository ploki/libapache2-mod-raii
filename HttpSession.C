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
#include <unistd.h>
#include "raii.H"

namespace raii {

    HttpSession::HttpSession(ServletContext *sc) :
        Object(),
        AttributeHolder(),
    invalid(false),
    id(),
    user(),
    lastAccessedTime(0),
    savedLastAccessedTime(0),
    maxInactiveInterval(3600),
    m_new(true),
    m_servletContext(sc),
    mutex()
    {
      creationTime=time(NULL);
      lastAccessedTime=creationTime;
      savedLastAccessedTime=lastAccessedTime;
      int fd=open("/dev/urandom",O_RDONLY);
      if ( fd < 0 ) {
        throw IOException("open: /dev/urandom");
      }
      long long unsigned int r1,r2;
      if (    read(fd,&r1,sizeof(r1)) != sizeof(r1)
	   || read(fd,&r2,sizeof(r2)) != sizeof(r2) ) {
	  close(fd);
          throw IOException("read: /dev/urandom");
      }
      char buf[33];
      snprintf(buf,sizeof(buf),"%08llx%08llx",r1,r2);
      close(fd);
      id=buf;
      RaiiConfig *cfg=get_dconfig(apacheRequest);
      if ( cfg && cfg->raiiroute && cfg->raiiroute[0] != '\0' )
	id=id+"."+cfg->raiiroute;
    };

    void HttpSession::setUser(const String& name) { Lock l1(mutex); user=name; };
    String HttpSession::getUser() const { Lock l1(mutex);  return user; };

    long HttpSession::getCreationTime() const {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      return creationTime;
    };
    String HttpSession::getId() const {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      return id;
    };
    long HttpSession::getLastAccessedTime() const {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      return lastAccessedTime;
    };
    void HttpSession::touch() const {
      //if ( invalid ) throw InvalidSessionException("Session were invalidated");
      Lock l1(mutex);
      if ( ! invalid ) {
        savedLastAccessedTime=lastAccessedTime;
        lastAccessedTime=time(NULL);
      }
    };
    void HttpSession::untouch() const {
	Lock l1(mutex);
	if ( !invalid )
		lastAccessedTime=savedLastAccessedTime;
    }

    int HttpSession::getMaxInactiveInterval() const {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      return maxInactiveInterval;
    };
    ServletContext* HttpSession::getServletContext() const {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      return m_servletContext;
    }
    void HttpSession::invalidate()
    {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      clearAttributes();
      invalid=true;
	m_servletContext->unrefSession(id);
    }
    bool HttpSession::isValid() const {
      return !invalid;
    }
    bool HttpSession::isNew() const {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      return m_new;
    };
    void HttpSession::setNew(bool theValue) {
      //if ( invalid ) throw InvalidSessionException("Session were invalidated");
      if ( !invalid )
        m_new=theValue;
    };
    void HttpSession::setMaxInactiveInterval(int interval) {
      if ( invalid ) throw InvalidSessionException("Session were invalidated");
      maxInactiveInterval=interval;
    };

    String HttpSession::dump(const String& str) const {


	Vector<String> attrNames = getAttributeNames();

	StringStream ss;
		ss << "Session("<<str<<")={\n";
	for ( Vector<String>::const_iterator attrName = attrNames.begin();
		attrName != attrNames.end();
		++attrName ) {
		ptr<Object> obj = getAttribute(*attrName);
		obj->dump(*attrName);
		ss << ", ";
	}
	ss << "}";
	return ss.str();
    }

}
