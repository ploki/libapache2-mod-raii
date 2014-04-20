#ifndef __SERVLETCONTAINER_H__
#define __SERVLETCONTAINER_H__

//#include "mod_raii.H"

#include "Object.H"
#include "HttpServlet.H"

namespace raii {

  class ServletContainer : public Object {
    String file;
    String modification_time;
    String sofile;
    void *dlhandle;
    HttpServlet * (*servletFactory)(); 
    Map<pthread_t, ptr<HttpServlet> > servlet;
    String errorbuffer;
    Mutex mutex;
    //HttpServlet *(*servletFactory)();
    //string info;
    //string name;
    void buildServlet();
    bool isServletBuilt();
    void loadServlet();
    void unloadServlet();

  public:
    ServletContainer(request_rec *r);
    ~ServletContainer();
    ptr<HttpServlet> getServlet();
    virtual String dump(const String& str) const { return String("ServletContainer(")+str+")"; }
  };

  extern Mutex dlMutex;

}

#endif /* __SERVLETCONTAINER_H__ */
