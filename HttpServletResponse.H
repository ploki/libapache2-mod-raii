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
#ifndef __HTTPSERVLETRESPONSE_H__
#define __HTTPSERVLETRESPONSE_H__


#include "Object.H"

namespace raii {

	/*! The response class
	 * You have not to instanciate this class. An instance is created for
	 * you and transmitted to you by reference via raii::HttpServlet::service()
	 *
	 * \sa raii::HttpServletRequest
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp, FILE *f);
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp,void *);
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp,const String& str);
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp,int i);
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp,long l);
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp,double f);
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp,unsigned int i);
	 * \sa const HttpServletResponse& operator<<(const HttpServletResponse& resp,unsigned long l);
	 */
	class HttpServletResponse : public Object {
		String contentType;
		String characterEncoding;

                friend apr_status_t raii_launch(request_rec *r);
                friend class HttpServletRequest;
		HttpServletResponse();
		public:
		/*! set the specified cookie on the client */
		void addCookie(const Cookie& cookie);

		/*! set a response header containing the specified rfc822 date
		 * \date an integer representing the number of seconds since January 1st, 1970 at 00:00:00
		 */
		void addDateHeader(const String& name, long date);

		/*! set a response header to the specified String value
		 * \param name the name of response header to set
		 * \param value the String value
		 */
		void addHeader(const String& name, const String& value);

		/*! set a response header to the specified int value
		 * \param name the name of response header to set
		 * \param value the int value
		 */
		void addHeader(const String& name, int value);

		/*! set a response header to the specified int value
		 * \param name the name of response header to set
		 * \param value the int value
		 */
		void addIntHeader(const String& name, int value);

		/*! set a response header to the specified String value
		 * \param name the name of response header to set
		 * \param value the String value
		 */
		void setDateHeader(const String& name, long date);

		/*! set a response header to the specified String value
		 * \param name the name of response header to set
		 * \param value the String value
		 */
		void setHeader(const String& name, const String& value);

		/*! set a response header to the specified int value
		 * \param name the name of response header to set
		 * \param value the int value
		 * \sa raii::HttpServletResponse::setHeader(const String& name, const String& value);
		 */
		void setHeader(const String& name, int value);

		/*! set a response header to the specified int value
		 * \param name the name of response header to set
		 * \param value the int value
		 * \sa raii::HttpServletResponse::setHeader(const String& name, int value);
		 */
		void setIntHeader(const String& name, int value);

		/*! check if specified response header is set
		 * \name the response header name to check
		 * \return true if specified response header is present
		 */
		bool containsHeader(const String& name) const;

		/*! Append to the specifed url the current session id
		 * \param url is a fully qualifed url, an absolute path or a relative path, containing or not a querystring or anchor
		 * \return the url with the session id
		 */
		String encodeRedirectURL(const String& url) const;

		/*! Append to the specifed url the current session id
		 * \sa raii::HttpServletResponse::encodeRedirectURL()
		 */
		String encodeURL(const String& url) const;

		/*! Set the HTTP status code and put a message in the response
		 * \param sc is the status code to set
		 * \param msg is the message to put in the response
		 * \return never returns
		 * \note throw RequestOK("sendError") which is catched by mod_raii to finalize the request
		 */
		void sendError(int sc,const String& msg=String());

		/*! Redirect the client to the specified location
		 * Redirect the client to the specified location using Location header and HTTP_MOVED_TEMPORARILY status
		 * \param location is a fully qualified url, an absolute path or a relative path.
		 * \param status optional status, default is 302 Moved Temporarily
		 * \return never returns
		 * \sa raii::HttpServletRequest::getContextPath()
		 * \sa raii::HttpServletResponse::encodeRedirectURL()
		 * \note on success, RequestOK("sendRedirect") is thrown and catched by mod_raii to finalize the request
		 */
		void sendRedirect(const String& location, int status=HTTP_MOVED_TEMPORARILY);

		/*! Set the HTTP status code
		 * \param sc is the status code to set
		 */
		void setStatus(int sc);

		void applyCT();

		//ServletResponse

		/*! Return the character encoding of the response */
		String getCharacterEncoding() const;

		/*! Return the content type of the response */
		String getContentType() const;

		/*! Set the character encoding of the response */
		void setCharacterEncoding(const String& charset);

		/*! Set nothing, NotImplementedException is thrown */
		void setContentLength(int len);

		/*! Set MIME content type of the response
		 * \param type is the MIME content type to set
		 */
		void setContentType(const String& type);

		/*! Set nothing, NotImplementedException is thrown */
		void setBufferSize(int size);

		/*! get nothing, NotImplementedException is thrown */
		int getBufferSize() const;

		/*! Attempt to flush the response buffer
		 * \note no warranty is provided by apache
		 */
		void flushBuffer();

		/*! reset nothing, NotImplementedException is thrown */
		void resetBuffer();

		/*! return nothing, NotImplementedException is thrown */
		bool isCommitted() const;

		/*! reset nothing, NotImplementedException is thrown */
		void reset();

		/*! set nothing, NotImplementedException is thrown */
		void setLocale(const String& loc);

		/*! get nothing, NotImplementedException is thrown */
		String getLocale() const;

		virtual String dump(const String& str) const;

	};

	/*! Append the specified file to the output
	 * \code
	 * FILE *f=fopen(filename.c_str(),"r");
	 * if ( !f ) throw IOException(filename.c_str());
	 * try {
	 *     response << f;
	 *     fclose(f);
	 * }
	 * catch ( ... ) {
	 *    fclose(f);
	 *    response.setContentType("text/html");
	 *    throw;
	 * }
	 * \endcode
	 */
	const HttpServletResponse& operator<<(const HttpServletResponse& resp, FILE *f);

	/*! Append to the output the value of the pointer in hex format */
	const HttpServletResponse& operator<<(const HttpServletResponse& resp,void *);

	/*! Append to the output the specified string */
	const HttpServletResponse& operator<<(const HttpServletResponse& resp,const String& str);

	/*! Append to the output the specified int */
	const HttpServletResponse& operator<<(const HttpServletResponse& resp,int i);

	/*! Append to the output the specified long */
	const HttpServletResponse& operator<<(const HttpServletResponse& resp,long l);

	/*! Append to the output the specified double with 2 digits after the dot.
	 *
	 * this code
	 * \code
	 * double pi = M_PI;
	 * response << pi
	 * \endcode
	 * appends "3.14" to the output
	 */
	const HttpServletResponse& operator<<(const HttpServletResponse& resp,double f);

	/*! Append to the output the specified unsigned int */
	const HttpServletResponse& operator<<(const HttpServletResponse& resp,unsigned int i);

	/*! Append to the output the specified unsigned long*/
	const HttpServletResponse& operator<<(const HttpServletResponse& resp,unsigned long l);

}

#endif /* __HTTPSERVLERRESPONSE_H__ */
