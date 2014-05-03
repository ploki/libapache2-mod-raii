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
#ifndef __SERVLETCONTEXT_H__
#define __SERVLETCONTEXT_H__

#include "Object.H"
#include "AttributeHolder.H"
#include "GetRequestDispatcher.H"
#include "HttpServlet.H"
#include "ServletContainer.H"
#include "ptr.H"

#include <pthread.h>

namespace raii {
  class ServletContext : public AttributeHolder, public GetRequestDispatcher {
  public:
    ServletContext(const String& ct)
        : Object(),
          AttributeHolder(),
          GetRequestDispatcher(ct),
          lastCollection(time(NULL)),
          initParameterMutex(),
          initParameter(),
          containerMutex(),
          container(),
          contextPath(ct),
          sessionMutex(),
          session() {};

    virtual ~ServletContext() {
	Logger log("raii");
	log.debug();
	log("cleaning context "+contextPath);
	clearAttributes();
    }

    void init(const Map<String,String> &ip) { initParameter=ip; };
    ptr<ServletContext> getContext(const String& uripath) const;
    String getInitParameter(const String& name) const;
    Vector<String> getInitParameterNames() const;
    int getMajorVersion() const;
    int getMinorVersion() const;
    String getMimeType(const String& file) const;
    //string getResource(String path);
    String getServerInfo() const;
    String getServletContextName() const;
    ptr<HttpServlet> getServlet();
    void log(const String& msg) const;
    ptr<HttpSession> getSession(const String& id);
    int getSessionCount() const;
    ptr<HttpSession> createSession();
    virtual String dump(const String& str) const;

    void unrefSession(const String& id) {
		session.erase(session.find(id));
    }
    Map<String,ptr<HttpSession> >& getSessionMap() {
		return session;
	}

    //static Map <String, ptr<ServletContext> > servletContext;

  private:
    long lastCollection;
    Mutex initParameterMutex;
    mutable Map<String,String> initParameter;

    Mutex containerMutex;
    Map<String,ptr<ServletContainer> > container;
    String contextPath;

    Mutex sessionMutex;
    Map<String,ptr<HttpSession> > session;
  };

  extern Mutex servletContextMutex;
  extern Map <String, ptr<ServletContext> > *servletContext;
}

#endif /* __SERVLETCONTEXT_H__ */
