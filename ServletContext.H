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
