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
#ifndef __HTTPSERVLETREQUEST_H__
#define __HTTPSERVLETREQUEST_H__

#include "Object.H"
#include "GetRequestDispatcher.H"
#include "Cookie.H"

namespace raii {
	extern "C" int callback_table(void *rec, const char *key, const char *value);

	class HttpSession;

	/*! The request class
	 * You have not to instanciate this class. An instance is created for
	 * you and transmitted to you by reference via raii::HttpServlet::service()
	 *
	 * \sa raii::HttpServletResponse
	 */
	class HttpServletRequest : public AttributeHolder, public GetRequestDispatcher {

		private:

		static const char * CSESSIONID;
		const apr_table_t *get_;
		const apr_table_t *post_;
		const apr_table_t *cookie_;
		String body;
		//RaiiConfig *cfg;
		String requestedSessionId;
		mutable String sessionId;
		bool sessionIdFromCookie;
		bool sessionIdFromURL;
		mutable bool sessionIdValid;


		static apr_table_t *parse_cookie();

                friend apr_status_t raii_launch(request_rec *r);
                friend class HttpServletResponse;
		HttpServletRequest();
		public:

		/*! Return the type of authentication
		 * \return "Form" if authentication is done in an applicative way or "Basic"
		 * \return or something else if apache is responsible of the authentication
		 * \return or an empty string if user isn't authenticated.
		 *
		 */
		String getAuthType() const;

		/*! Return the context path as defined in configuration file
		 * \return something like "/myapp"
		 */
		String getContextPath() const;

		/*! Return a raii::Vector containing all cookies sent by the client */
		Vector<Cookie> getCookies() const;

		/*! Return seconds since EPOCH of the date contained in the header of specified name */
		long getDateHeader(const String& name) const;

		/*! Return value of specified header */
		String getHeader(const String& name) const;

		/*! Return a raii::Vector containing all header names */
		Vector<String> getHeaderNames() const;

		/*! Return a raii::Vector containing all values of specified header
		 * \note useful in case of multiple headers with the same name
		 */
		Vector<String> getHeaders(const String& name) const;

		/*! Return the int value of specified header*/
		int getIntHeader(const String& name) const;

		/*! return request HTTP method */
		String getMethod() const;

		/*! return request HTTP method number */
		int getMethodNumber() const;

		/*! return the pathinfo part of the request
		 * pathinfo is the remaining part of the url after resource is found by apache
		 * \note with url like http : //localhost/myapp/index.C/user/id , pathInfo is /user/id
		 */
		String getPathInfo() const;

		/*! return real path of the current servlet source */
		String getTranslatedPath() const;

		/*! return the query string of the current request */
		String getQueryString() const;

		/*! return the current remote user if defined or an empty string*/
		String getRemoteUser() const;

		/*! return the value of the requested session id*/
		String getRequestedSessionId() const;

		/*! return the path part of the requested URL */
		String getRequestURI() const;

		/*! return the path part and the querystring of the requested URL */
		String getRequestURL() const;

		/*! return path of the servlet within the context */
		String getServletPath() const;

		/*! return the session associated to the current session id defined in headers or in querystring
		 * \param create wheither to create a new session if it doesn't exist
		 */
		ptr<HttpSession> getSession(bool create=true) const;

		/*! get nothing, not implemented, an exception is thrown*/
		String getUserPrincipal() const;

		/*! returns true if session id comes from cookie */
		bool isRequestedSessionIdFromCookie() const;

		/*! return true if session id comes from querystring */
		bool isRequestedSessionIdFromURL() const;

		/*! return true if session id provided by client exists */
		bool isRequestedSessionIdValid() const;

		/*! not implemented */
		bool isUserInRole(const String& role) const;

		//ServletRequest
		/*! return the content length of the request body */
		int getContentLength() const;

		/*! return the content type of the request body */
		String getContentType() const;

		/*! return the content of the request body */
		String getBody() const;

		/*! Not implemented! */
		String getLocalAddr() const;

		/*! Not implemented! */
		String getLocalName() const;

		/*! Not implemented! */
		int getLocalPort() const;

		/*! returns parameter value
		 * \param name of parameter to get
		 * \note You do not have to url encode parameter names
		 * \note You do not have to url decode returned values
		 * \note search order is GET - POST - COOKIE
		 */
		String getParameter(const String& name) const;

		/*! return a raii::Vector containing parameter names */
		Vector<String> getParameterNames() const;

		/*! return a raii::Vector containing parameter values for specified name */
		Vector<String> getParameterValues() const;

		/*! return a raii::Map containing parameter/value pairs */
		Map<String,String> getParameterMap() const;

		/*! return current request protocol
		 * \note actualy HTTP/0.9, HTTP/1.0 or HTTP/1.1
		 */
		String getProtocol() const;

		/*! return current request scheme
		 * \note actualy http or https
		 */
		String getScheme() const;

		/*! return host name of current request */
		String getServerName() const;

		/*! return server port of current request */
		unsigned short getServerPort() const;

		/*! return remote ip address of current request */
		String getRemoteAddr() const;

		/*! return remote hostname of current request */
		String getRemoteHost() const;

		/*! return remote tcp port of current request */
		String getRemotePort() const;

		/*! not implemented */
		String getLocale() const;

		/*! not implemented */
		Vector<String> getLocales() const;

		/*! return true if ap_default_port is true */
		bool isSecure() const;
		virtual String dump(const String& str) const;
	};
};


#endif /* __HTTPSERVLETREQUEST_H__ */
