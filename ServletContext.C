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
#include "raii.H"


namespace raii {

  //static
  //map <string, ptr<ServletContext> > ServletContext::servletContext;

    Mutex servletContextMutex;
    Map <String, ptr<ServletContext> > * servletContext=NULL;

  ptr<ServletContext> ServletContext::getContext(const String& uripath) const
  {
    Lock l1(servletContextMutex);
    ApacheSubRequest subreq(uripath,NULL);
    RaiiConfig *cfg=get_dconfig(subreq.getApacheRequest());
    return (*servletContext)[cfg->contextpath];

  }
  String ServletContext::getInitParameter(const String& name) const
  {
    Lock l1(initParameterMutex);
    return initParameter[name];
  }
  Vector<String> ServletContext::getInitParameterNames() const
  {
    Lock l1(initParameterMutex);
    Vector<String> plop;
    for ( Map<String,String>::const_iterator it = initParameter.begin();
          it != initParameter.end();
          ++it ) {
        plop.push_back(it->first);
    }
    return plop;
  }
  int ServletContext::getMajorVersion() const
  {
    return 1;
  }
  int ServletContext::getMinorVersion() const
  {
    return 0;
  }
  String ServletContext::getMimeType(const String& file) const
  {
    String mimeType("");
    if ( apacheRequest ) {
      request_rec *rr=ap_sub_req_lookup_file(file.c_str(),apacheRequest,NULL);
      if ( rr ) {
        if ( rr->content_type )
          mimeType=rr->content_type;
        ap_destroy_sub_req(rr);
      }
      else
        throw UnavailableException("Apache sub-request unavailable");
    }
    else
      throw UnavailableException("Apache request unavailable");
    return mimeType;
  }
  //string getResource(string path);
  String ServletContext::getServerInfo() const
  {
    return ap_psignature("",apacheRequest);
  }
  String ServletContext::getServletContextName() const
  {
    Lock l1(servletContextMutex);
    for ( Map<String,ptr<ServletContext> >::const_iterator it = servletContext->begin();
          it != servletContext->end();
          ++it ) {
        if ( &*(it->second) == this )
          return it->first;
    }
    return String("");
  }
  ptr<HttpServlet> ServletContext::getServlet()
  {
    //fprintf(stderr,"filename=%s\n",r->filename);fflush(stderr);
    ptr<ServletContainer> lcontainer=NULL;
    {
      Lock l1(containerMutex);
      lcontainer=container[apacheRequest->filename];
      if ( ! lcontainer ) {
        lcontainer = new ServletContainer(apacheRequest);
        container[apacheRequest->filename]=lcontainer;
      }
    }
    return lcontainer->getServlet();

  }
  void ServletContext::log(const String& msg) const
  {
        Logger log(getServletContextName());
        log(msg);
  }

  ptr<HttpSession> ServletContext::getSession(const String& id)
  {
    Lock l1(sessionMutex);
    time_t ct=time(NULL);
    // nettoyage  ( s->getLastAccessedTime() plutôt que time(NULL) un appel système en moins )
    if ( ct >= lastCollection + 300 ) {
      Logger log("ServletContext::getSession()");
      log.debug();
      log("Starting session collection");
      lastCollection=ct;
      int count=0,cleaned=0;
      for ( Map<String,ptr<HttpSession> >::iterator it = session.begin();
            it != session.end();
            /* rien */ ) {
        count++;
        if ( it->first.empty()
             || !it->second
             || ! it->second->isValid()
             || it->second->getLastAccessedTime() + it->second->getMaxInactiveInterval() < ct ) {
          //nettoyage
          cleaned++;
          if ( it->second )
            it->second->clearAttributes();
          session.erase(it++);
        } else {
          it++;
        }

      }
      log(itostring(cleaned)+" cleaned session out of "+itostring(count));
    }

    Map<String, ptr<HttpSession> >::iterator s = session.find(id);

    if ( s != session.end() ) {
       if ( s->second ) {
         if ( s->second->getLastAccessedTime() + s->second->getMaxInactiveInterval() < ct ) return NULL;
         s->second->setNew(false);
       }
       return s->second;
    }
    return NULL;
  }

  ptr<HttpSession> ServletContext::createSession()
  {
    ptr<HttpSession> s=new HttpSession(this);
    Lock l1(sessionMutex);
    session[s->getId()]=s;
    return s;
  }

  int ServletContext::getSessionCount() const { Lock l1(sessionMutex); return session.size(); }

  String ServletContext::dump( const String& str) const {
        return String("ServletContext(")+str+")="+getServletContextName();
  }
}
