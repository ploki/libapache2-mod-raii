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
