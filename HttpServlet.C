#include "raii.H"

namespace raii {
    void HttpServlet::doDelete(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }
    void HttpServlet::doGet(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }
    void HttpServlet::doHead(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }
    void HttpServlet::doOptions(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }
    void HttpServlet::doPost(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }
    void HttpServlet::doPut(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }
    void HttpServlet::doTrace(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }
    void HttpServlet::doLastModified(HttpServletRequest& req, HttpServletResponse& resp)
    {
      resp.setStatus(HTTP_METHOD_NOT_ALLOWED);
      throw RequestERROR();
    }

  void HttpServlet::preService(HttpServletRequest& req, HttpServletResponse& resp) {}
  void HttpServlet::postService(HttpServletRequest& req, HttpServletResponse& resp) {  }
  void HttpServlet::postService(HttpServletRequest& req, HttpServletResponse& resp,bool afterException) {
     firstRun=false;
     postService(req,resp);
  }
  void HttpServlet::service(HttpServletRequest& req, HttpServletResponse& resp)
  {
    switch(req.getMethodNumber())
      {
      case M_DELETE:
	doDelete(req,resp);
	break;
      case M_GET:
        if ( apacheRequest->header_only )
          doHead(req,resp);
        else
	        doGet(req,resp);
	      break;
      case M_OPTIONS:
	      doOptions(req,resp);
	      break;
      case M_POST:
	      doPost(req,resp);
	      break;
      case M_TRACE:
	      doTrace(req,resp);
	      break;
	/*
	  case M_LASTMODIFIED:
	  doLastModified(req,resp);
	  break;
	*/
      }
  };
  
  
  
}
