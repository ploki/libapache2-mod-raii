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
#include "raii.H"

namespace raii {

	RequestDispatcher::RequestDispatcher(const String& _contextPath, const String& _uri)

		: Object(), contextPath(_contextPath), uri(_uri) {}


	void RequestDispatcher::forward(HttpServletRequest& request, HttpServletResponse& response) {

		service(request,response);
		throw RequestOK("RequestDispatcher::forward");
	}


	void RequestDispatcher::include(HttpServletRequest& request, HttpServletResponse& response) {

		try {
			service(request,response);
		}
		catch (RequestOK& e) {
			//nop! si la page chargée forward sur une autre page, il ne faut pas propager l'exception
			if ( e.getMessage() != "RequestDispatcher::forward" )
				throw;
		}
	}

        String RequestDispatcher::dump(const String& str) const {

                return String("RequestDispatcher(")+str+")="+contextPath+uri;
        }

	void RequestDispatcher::service(HttpServletRequest& request,HttpServletResponse& response) {

		//request_rec *oldApacheRequest=apacheRequest;

		ptr<ServletContext> ctx=NULL;
		{
			Lock l1(servletContextMutex);
			ctx=(*servletContext)[contextPath];
		}

		String context_n_uri=contextPath+uri;
		ptr<HttpServlet> sv=NULL;

		{
			//changement de context
			ApacheSubRequest subreq(context_n_uri);
			sv=ctx -> getServlet();
		}

		if ( ! sv )
			throw UnavailableException("Pas de servlet");

		try {
			sv->preService(request, response);
			sv->service (request, response);
		}
		catch (...) {
			sv->postService(request, response,true);
			throw;
		}
		sv->postService(request, response,false);
	}



}
