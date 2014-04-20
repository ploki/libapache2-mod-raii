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
#include <apreq2/apreq.h>
#include <apreq2/apreq_module.h>
#include <apreq2/apreq_param.h>
#include <apr_date.h>
//#include <apreq2/apreq_cookie.h>
//#include <apreq2/apreq_error.h>
//#include <apreq2/apreq_parser.h>
//#include <apreq2/apreq_util.h>
//#include <apreq2/apreq_version.h>

#if ( AP_SERVER_MAJORVERSION_NUMBER == 2 ) && (AP_SERVER_MINORVERSION_NUMBER == 2)
# define RAII_APACHE22
#endif


namespace raii {

	extern "C" apreq_handle_t* apreq_handle_apache2(request_rec*);

	const char *HttpServletRequest::CSESSIONID = "CSESSIONID";

	extern "C" int callback_table(void *rec, const char *key, const char *value)
	{
		reinterpret_cast<Vector<String>*>(rec)->push_back(key);
		return 1;
	}


	static char *strstrip(char *str)
	{
		char *s=str,
				*d=str;
		if ( !str )
			return NULL;

		while ( *s==' ' || *s=='\t' ) s++;

		do *d++=*s; while ( *s++ !='\0' );

		d-=2;

		while ( *d == ' ' || *d == '\t' ) *d--='\0';

		return str;
	}


	static apr_table_t *parseCookie()
	{
		char *tokbuf,*p,*val;
		char *cookie;
		apr_table_t *cookies=apr_table_make(apacheRequest->pool,0);

		cookie=apr_pstrdup(apacheRequest->pool,
				apr_table_get(apacheRequest->headers_in,"Cookie"));
		if ( cookie )
		{
			if ( (p=strtok_r(cookie,";",&tokbuf) ) )
				do
				{
					val=strchr(p,'=');
					if ( val )
					{
						*val++='\0';
						strstrip(val);
					}
					strstrip(p);
					apr_table_add(cookies,p,val);

				}
				while ( ( p=strtok_r(NULL,";",&tokbuf) ) );
		}
		return cookies;

	}


	HttpServletRequest::HttpServletRequest() :
    				Object(),
    				AttributeHolder(),
    				GetRequestDispatcher( getContextPath() ),
    				get_(NULL),
    				post_(NULL),
    				cookie_(NULL),
    				body(""),
    				requestedSessionId(),
    				sessionId(),
    				sessionIdFromCookie(false),
    				sessionIdFromURL(false),
    				sessionIdValid(false)
    				{


		cookie_=parseCookie();
		String contentType=getContentType();

		//tué à la fin de la requête
		apreq_handle_t *apreq=apreq_handle_apache2(apacheRequest);
		if ( apreq_args(apreq,&get_) != APR_SUCCESS )
			get_=apr_table_make(apacheRequest->pool,0);

		//	Logger mlog("ct check");
		//	mlog(contentType);
		if ( contentType.empty()
				|| contentType.matches(Regex("^application/x-www-form-urlencoded").caseInsensitive())
				|| contentType.matches(Regex("^multipart/form-data").caseInsensitive()) ) {

			//	mlog("with apreq");
			ap_discard_request_body(apacheRequest);

			if ( apreq_body(apreq,&post_) != APR_SUCCESS )
				post_=apr_table_make(apacheRequest->pool,0);
		}
		else {
			//lecture body
			apr_bucket_brigade *bb;
			apr_bucket *b;
			int seen_eos = 0;
			bb = apr_brigade_create(apacheRequest->pool, apacheRequest->connection->bucket_alloc);

			do {
				apr_status_t rc;

				rc = ap_get_brigade(apacheRequest->input_filters, bb,
						AP_MODE_READBYTES,
						APR_BLOCK_READ, 1024);

				if (rc != APR_SUCCESS) {
					throw ServletException("Could not get next bucket brigade");
					break;
				}

				for (	b = APR_BRIGADE_FIRST(bb);
						b != APR_BRIGADE_SENTINEL(bb);
						b = APR_BUCKET_NEXT(b) ) {

					const char *data;
					apr_size_t len;

					if (APR_BUCKET_IS_EOS(b)) {
						seen_eos = 1;
						break;
					}

					if (APR_BUCKET_IS_METADATA(b)) {
						continue;
					}

					rc = apr_bucket_read(b, &data, &len, APR_BLOCK_READ);
					if (rc != APR_SUCCESS) {
						throw ServletException(
								"An error occurred while reading "
								"the request body.");
						break;
					}
					body+=String(data,len);

				}

				apr_brigade_cleanup(bb);
			} while (!seen_eos);

			apr_brigade_destroy(bb);

		}

		if ( ! post_ )
			post_=apr_table_make(apacheRequest->pool,0);

                const char *reqsessid = apr_table_get(cookie_,CSESSIONID);
                if ( reqsessid )
        		requestedSessionId=reqsessid;
		

		if ( requestedSessionId.empty() ) {
		        const char *reqsessid = apr_table_get(get_,CSESSIONID);
                        if ( reqsessid )
        			requestedSessionId=reqsessid;
			if ( !requestedSessionId.empty() )
				sessionIdFromURL=true;
		} else
			sessionIdFromCookie=true;

                sessionId = requestedSessionId;

        }


	String HttpServletRequest::getAuthType() const {
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		ptr<HttpSession> session=getSession(false);
		if ( session && !session->getUser().empty() ) {
			return String("Form");
		} else {
			if ( !apacheRequest->ap_auth_type )
				return String();
			return String(apacheRequest->ap_auth_type);
		}
	}

	String HttpServletRequest::getContextPath() const
			{
		RaiiConfig *cfg=get_dconfig(apacheRequest);
		if ( !cfg ) throw UnavailableException("No config data found");
		if ( !cfg->contextpath )
			throw UnavailableException("No contextpath defined");
		return String(cfg->contextpath);
			}

	long HttpServletRequest::getDateHeader(const String& name) const
			{
		if ( !apacheRequest ) throw RaiiException("Apache request is null");

		if ( name.empty() ) throw IllegalArgumentException("header name is empty");
		String dateHeader=getHeader(name);
		if ( dateHeader.empty() ) return -1;
		apr_time_t ts = apr_date_parse_rfc(dateHeader.c_str());
		if ( 0 == ts ) return -1;
		return ts/1000;
			}

	String HttpServletRequest::getHeader(const String& name) const
			{
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		if ( !apacheRequest->headers_in ) throw ApacheException("headers_in is null");
		if ( name.empty() )
			throw IllegalArgumentException("String argument is empty");

		const char *value=apr_table_get(apacheRequest->headers_in,name.c_str());
		if ( value ) return String(value);
		else return String();
			}

	int HttpServletRequest::getIntHeader(const String& name) const {
		/*
    if ( !r_ ) throw RaiiException("Apache request is null");
    if ( !r_->headers_in ) throw ApacheException("headers_in is null");
    if ( name.empty() || name.size() == 0 )
      throw IllegalArgumentException("String argument is empty");
    return atoi(apr_table_get(r_->headers_in,
			      name.c_str()));
		 */
		return atoi(getHeader(name).c_str());
	}

	String HttpServletRequest::getMethod() const {
		return String(apacheRequest->method);
	}
	int HttpServletRequest::getMethodNumber() const {
		return apacheRequest->method_number;
	}

	int HttpServletRequest::getContentLength() const {
		return apacheRequest->clength;
	}

	String HttpServletRequest::getContentType() const {
		return getHeader( "Content-Type" );
	}
	String HttpServletRequest::getBody() const {
		return body;
	}

	String HttpServletRequest::getPathInfo() const {
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		if  ( !apacheRequest->path_info )
			return String();
		return apacheRequest->path_info;
	}

	String HttpServletRequest::getQueryString() const {
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		if  ( !apacheRequest->args )
			return String();
		return apacheRequest->args;
	}

	String HttpServletRequest::getRequestURL() const {
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		return apr_uri_unparse(apacheRequest->pool,&apacheRequest->parsed_uri,0);
	}

	extern "C"
	int _cookies_callback_table(void*rec,const char*key, const char*value) {
		Cookie cookie;
		cookie.setName(key);
		cookie.setValue(value);
		(reinterpret_cast<Vector<Cookie>*>(rec))->push_back(cookie);
		return 1;
	}

	Vector<Cookie> HttpServletRequest::getCookies() const {
		Vector<Cookie> ret;
		apr_table_do(_cookies_callback_table,
				reinterpret_cast<void*>(&ret),
				cookie_,NULL);
		return ret;
	}


	Vector<String> HttpServletRequest::getHeaderNames() const {
		return raii_get_tables_keys(apacheRequest->pool, apacheRequest->headers_in);
	}



	extern "C"
	int _headers_callback_table(void *rec, const char *key, const char *value) {
		(reinterpret_cast<Vector<String>*>(rec))->push_back(value);
		return 1;
	}

	Vector<String> HttpServletRequest::getHeaders(const String& name) const {
		Vector<String> ret;
		apr_table_do(_headers_callback_table,
				reinterpret_cast<void*>(&ret),
				apacheRequest->headers_in,
				name.c_str(),NULL);
		return ret;
	}
	String HttpServletRequest::getTranslatedPath() const {
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		if ( !apacheRequest->filename )
			return String();
		return String(apacheRequest->filename);
	}

	String HttpServletRequest::getRemoteUser() const {
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		ptr<HttpSession> session=getSession(false);
		if ( session && !session->getUser().empty() )
			return session->getUser();
		if ( !apacheRequest->user )
			return String();
		return String(apacheRequest->user);
	}

	String HttpServletRequest::getRequestedSessionId() const {
	        return requestedSessionId;
	}

	String HttpServletRequest::getRequestURI() const {
		if ( !apacheRequest ) throw RaiiException("Apache request is null");
		if ( !apacheRequest->uri )
			return String();
		return apacheRequest->uri;
	}

	String HttpServletRequest::getServletPath() const {
		int i=0;
		RaiiConfig *cfg=get_dconfig(apacheRequest);
		if ( !cfg ) throw UnavailableException("No config data found");
		if ( String(cfg->contextpath) == String("/") )
			return apacheRequest->uri;
		while ( apacheRequest->uri[i] == cfg->contextpath[i] )
			i++;
		String tmp(apacheRequest->uri+i);
		tmp=tmp.substr(0,tmp.size()-getPathInfo().size());
		while ( tmp.rfind("/") != String::npos )
			tmp=tmp.substr(0,tmp.size()-1);
		return tmp;
	}

	ptr<HttpSession> HttpServletRequest::getSession(bool create) const {
		ServletConfig cfg;
		ptr<ServletContext> ctx=cfg.getServletContext();
		ptr<HttpSession> s=ctx->getSession(sessionId);
		if ( !s && !create)
			return NULL;
		if ( !s || !s->isValid() ) {
			sessionIdValid=false;
			s=ctx->createSession();
			sessionId=s->getId();
			//FIXME: ici il faut initialiser le cookie de session
			Cookie csessionid;
			csessionid.setName(HttpServletRequest::CSESSIONID);
			csessionid.setValue(sessionId);
			csessionid.setPath(getContextPath());
			HttpServletResponse response;
			response.addCookie(csessionid);
		}
		sessionIdValid=true;
		return s;
	}

	String HttpServletRequest::getUserPrincipal() const {
		throw NotImplementedException("NotImplemented");
	}

	bool HttpServletRequest::isRequestedSessionIdFromCookie() const {
		return sessionIdFromCookie;
	}

	bool HttpServletRequest::isRequestedSessionIdFromURL() const {
		return sessionIdFromURL;
	}

	bool HttpServletRequest::isRequestedSessionIdValid() const {

		if ( ! sessionIdValid ) {
			ServletConfig cfg;
			ptr<ServletContext> ctx=cfg.getServletContext();
			ptr<HttpSession> s=ctx->getSession(getRequestedSessionId());
			sessionIdValid=s?true:false;
		}
		return sessionIdValid;
	}

	bool HttpServletRequest::isUserInRole(const String& role) const {
		throw NotImplementedException("à coder");
	}


	String HttpServletRequest::getParameter(const String& name) const {
		//ordre POST GET COOKIE
		
		if ( const char *parameterValue=apr_table_get(post_,name.c_str()) )
		        return parameterValue;

                if ( const char *parameterValue=apr_table_get(get_,name.c_str()) )
        		return parameterValue;

                if ( const char *parameterValue=apr_table_get(cookie_,name.c_str()) )
        		return parameterValue;

		return String();
	}

	Vector<String> HttpServletRequest::getParameterNames() const {
		Vector<String> ret;
		apr_table_do(callback_table,reinterpret_cast<void*>(&ret),post_,NULL);
		apr_table_do(callback_table,reinterpret_cast<void*>(&ret),get_,NULL);
		apr_table_do(callback_table,reinterpret_cast<void*>(&ret),cookie_,NULL);
		return ret;
	}

	Vector<String> HttpServletRequest::getParameterValues() const { throw NotImplementedException("à coder"); }
	String HttpServletRequest::getProtocol() const { return apacheRequest->protocol; }
	String HttpServletRequest::getScheme() const { return

#ifdef ap_http_method
		// apache 2.0
		ap_http_method(apacheRequest);
#else
	// apache 2.2
	ap_run_http_scheme(apacheRequest);
#endif
	}
	String HttpServletRequest::getServerName() const { return apacheRequest->hostname; }
	unsigned short HttpServletRequest::getServerPort() const { return ap_default_port(apacheRequest); }
	String HttpServletRequest::getRemoteAddr() const {
#ifdef RAII_APACHE22
	  return apacheRequest->connection->remote_ip;
#else
	  return apacheRequest->useragent_ip;
#endif
	}
	String HttpServletRequest::getRemoteHost() const { return apacheRequest->connection->remote_host; }
	String HttpServletRequest::getRemotePort() const { throw NotImplementedException("à coder"); }
	String HttpServletRequest::getLocale() const { throw NotImplementedException("à coder"); }
	Vector<String> HttpServletRequest::getLocales() const { throw NotImplementedException("à coder"); }
	bool HttpServletRequest::isSecure() const { return ap_default_port(apacheRequest)?true:false; }

	String HttpServletRequest::dump(const String& str) const {
		return String("HttpServletRequest(")+str+")="+getRequestURL();
	}

};
