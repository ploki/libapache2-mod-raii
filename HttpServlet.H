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
