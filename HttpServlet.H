#ifndef __HTTPSERVLET_H__
#define __HTTPSERVLET_H__

#include "ServletConfig.H"
#include "HttpServletRequest.H"
#include "HttpServletResponse.H"



namespace raii {

  class HttpServlet : public ServletConfig {
  protected:
    bool firstRun;
    virtual void doDelete(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void doGet(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void doHead(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void doOptions(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void doPost(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void doPut(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void doTrace(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void doLastModified(HttpServletRequest& req, HttpServletResponse& resp);
  public:
    virtual void preService(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void service(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void postService(HttpServletRequest& req, HttpServletResponse& resp);
    virtual void postService(HttpServletRequest& req, HttpServletResponse& resp,bool afterException);
    HttpServlet() : ServletConfig(), firstRun(true) {}
    virtual ~HttpServlet() {};
  };

}

#endif /* __HTTPSERVLET_H__ */
