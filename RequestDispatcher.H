#ifndef __REQUESTDISPATCHER_H__
#define __REQUESTDISPATCHER_H__

#include "Object.H"

namespace raii {
  class ServletContext;
  class HttpServletRequest;
  class HttpServletResponse;

  class RequestDispatcher : public Object {
  public:
    RequestDispatcher(const String& _contextPath,
					 const String& _uri);
    /*! forward the current process to this configured RequestDispatcher
     * \param request is the current request, not a newly created
     * \param response is the current response, not a newly created
     * \return never return, RequestOK("RequestDispatcher::forward") is thrown and should be catched by mod_raii
     */
    void forward(HttpServletRequest& request, HttpServletResponse& response);

    /*! include in the current process the execution of this configured RequestDispatcher
     * \param request is the current request, not a newly created
     * \param response is the current response, not a newly created
     */
    void include(HttpServletRequest& request, HttpServletResponse& response);

    virtual String dump(const String& str) const;

  private:
    void service(HttpServletRequest& request,
                 HttpServletResponse& response);
    String contextPath;
    String uri;
  };

}

#endif /* __REQUESTDISPATCHER_H__ */
