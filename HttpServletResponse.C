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
#include <unistd.h>
#include "raii.H"
#include <limits.h>



namespace raii {

extern "C" void ap_flush_conn(conn_rec *c);

#define check(name)  checkStringParameter(name,#name "is empty")

static inline void checkStringParameter(const String& name,const char *msg)
{
  if ( name.empty() ) throw IllegalArgumentException( msg );
}
static inline void checkApacheRequest()
{
  if ( !apacheRequest ) throw RaiiException("apacheRequest is empty");
}
 
 
const HttpServletResponse& operator<<(const HttpServletResponse& resp,const String& str)
{
  checkApacheRequest();
  size_t size = str.size();
  if ( size > INT_MAX )
        throw OverflowException("String size exceeds int capacity");
  if ( ap_rwrite(str.data(),static_cast<int>(size),apacheRequest) < 0 )
    throw ResponseException("Connection closed");
  return resp;
}

const HttpServletResponse& operator<<(const HttpServletResponse& resp,int i)
{
  checkApacheRequest();
  if ( ap_rprintf(apacheRequest,"%d",i) < 0 )
    throw ResponseException("Connection closed");
  return resp;
}

const HttpServletResponse& operator<<(const HttpServletResponse& resp,long l)
{
  checkApacheRequest();
  if ( ap_rprintf(apacheRequest,"%ld",l) < 0 )
    throw ResponseException("Connection closed");
  return resp;
}

const HttpServletResponse& operator<<(const HttpServletResponse& resp,double f)
{
  checkApacheRequest();
  if ( ap_rprintf(apacheRequest,"%.2f",f) < 0 )
    throw ResponseException("Connection closed");
  return resp;
}

const HttpServletResponse& operator<<(const HttpServletResponse& resp,unsigned int i)
{
  checkApacheRequest();
  if ( ap_rprintf(apacheRequest,"%u",i) < 0 )
    throw ResponseException("Connection closed");
  return resp;
}

const HttpServletResponse& operator<<(const HttpServletResponse& resp,unsigned long l)
{
  checkApacheRequest();
  if ( ap_rprintf(apacheRequest,"%lu",l) < 0 )
    throw ResponseException("Connection closed");
  return resp;
}

const HttpServletResponse& operator<<(const HttpServletResponse& resp,void *p)
{
  checkApacheRequest();
  char buf[32];
  snprintf(buf,32,"%p",p);
  if ( ap_rputs(buf,apacheRequest) < 0 )
    throw ResponseException("Connection closed");
  return resp;
}

const HttpServletResponse& operator<<(const HttpServletResponse& resp, FILE *f)
{
  const int Q=1024;
  char buf[Q];
  int i,fd;
  if ( !f )
    throw IllegalArgumentException("FILE* is null");
  fd=fileno(f);
  if ( fd == -1 )
    throw IllegalArgumentException("FILE* is not valid");
  
  do {
    i=read(fd,buf,Q);
    if ( i == -1 )
      throw IOException("Read error");
    
    if ( ap_rwrite(buf,i,apacheRequest) < 0 )
      throw ResponseException("Connection closed");
    
  } while ( i == Q );
  return resp;
}

HttpServletResponse::HttpServletResponse()	:	Object(),
							contentType(apacheRequest->content_type),
							characterEncoding(apacheRequest->content_encoding?apacheRequest->content_encoding:"") {
}

void HttpServletResponse::addCookie( const Cookie & cookie )
{
  checkApacheRequest();
  apr_table_add(apacheRequest->headers_out,"Set-Cookie",String(cookie).c_str());
}

void HttpServletResponse::addDateHeader( const String& name, long date )
{
  check(name);
  char *date_str=(char*)apr_pcalloc(apacheRequest->pool,APR_RFC822_DATE_LEN);
  apr_rfc822_date(date_str,((apr_time_t)date)  * 1000 * 1000 );
  apr_table_add(apacheRequest->headers_out,name.c_str(),date_str);
}

void HttpServletResponse::addHeader( const String & name, const String & value )
{
  check(name);
  apr_table_add(apacheRequest->headers_out,name.c_str(),value.c_str());
}

void HttpServletResponse::addHeader( const String& name, int value )
{
  check(name);
  apr_table_add(apacheRequest->headers_out,name.c_str(),
                apr_itoa(apacheRequest->pool,value));
}

void HttpServletResponse::setDateHeader( const String& name, long date )
{
  check(name);
  char *date_str=(char*)apr_pcalloc(apacheRequest->pool,APR_RFC822_DATE_LEN);
  apr_rfc822_date(date_str,((apr_time_t)date) * 1000 * 1000 );
  apr_table_set(apacheRequest->headers_out,name.c_str(),date_str);
}

void HttpServletResponse::setHeader( const String& name, const String& value )
{
  check(name);
  apr_table_set(apacheRequest->headers_out,name.c_str(),value.c_str());
}

void HttpServletResponse::setHeader( const String& name, int value )
{
  check(name);
  apr_table_set(apacheRequest->headers_out,name.c_str(),
                apr_itoa(apacheRequest->pool,value));
}

bool HttpServletResponse::containsHeader( const String& name ) const
{
  check(name);
  return apr_table_get(apacheRequest->headers_out,name.c_str())
          ?true:false;
}

#define CSESSIONID "CSESSIONID"
String HttpServletResponse::encodeRedirectURL(const String& url ) const
{
  check(url);
  HttpServletRequest request;
  ptr<HttpSession> session=request.getSession(false);
  if ( ! session )
    return url;

  String anchor;
  String tmp;
  size_t apos;
  if ( (apos=url.find('#')) != String::npos ) {
        anchor=url.substr(apos,String::npos);
        tmp=url.substr(0,apos);
  }
  else {
        anchor="";
        tmp=url;
  }
         
  char* ret=apr_pstrdup(apacheRequest->pool,tmp.c_str());
  
  
  if ( ( request.isRequestedSessionIdFromURL() || session->isNew() )
         && !strstr(ret, "?" CSESSIONID "=")
         && !strstr(ret, "&" CSESSIONID "=" ) )
  {
    if ( strchr(ret, '?' ) )
      ret=apr_pstrcat(apacheRequest->pool,ret,"&" CSESSIONID "=" ,url_encode(session->getId()).c_str(),NULL);
    else
      ret=apr_pstrcat(apacheRequest->pool,ret,"?" CSESSIONID "=" ,url_encode(session->getId()).c_str(),NULL);
  }
  //return response_encodeURL(url);
  return ret+anchor;
}

String HttpServletResponse::encodeURL( const String& url ) const
{
  check(url);
  return encodeRedirectURL( url );
}

void HttpServletResponse::sendError( int sc, const String & msg )
{
  HttpServletResponse response;
  setStatus( sc );
  if ( !msg.empty() ) {
    response << msg;
    throw RequestOK("sendError");
  }
  else
    throw RequestERROR("sendError");
}

void HttpServletResponse::sendRedirect( const String & location, int status )
{
  check(location);
  HttpServletRequest request;
  const char *url;
  
  if ( location[0] == '/' )
    url=ap_construct_url(apacheRequest->pool,
                                     location.c_str(),
                         apacheRequest);
  else if ( strncmp("http://",location.c_str(),sizeof("http://")-1)
            && strncmp("ftp://",location.c_str(),sizeof("ftp://")-1) )
  {
    char *p,*q=apr_pstrdup(apacheRequest->pool,apacheRequest->uri);
    p=strrchr(q,'/');
    if ( p )
      *p='\0';
    url=ap_construct_url(apacheRequest->pool,
                         apr_pstrcat(apacheRequest->pool,q,"/",location.c_str(),NULL),
                         apacheRequest);
  }
  else
    url=apr_pstrdup(apacheRequest->pool,location.c_str());
  fflush(stderr);
  setHeader("Location", url);
  setStatus(status);
  throw RequestOK("sendRedirect");
}

void HttpServletResponse::setStatus( int sc )
{
  checkApacheRequest();
  apacheRequest->status=sc;
}


    void HttpServletResponse::addIntHeader(const String& name, int value) { addHeader(name,value); }
    void HttpServletResponse::setIntHeader(const String& name, int value) { setHeader(name,value); }
    String HttpServletResponse::getCharacterEncoding() const { throw NotImplementedException("à coder"); }
    String HttpServletResponse::getContentType() const {
      return contentType;
      //return apacheRequest->content_type;
    }
    void HttpServletResponse::setCharacterEncoding(const String& charset) {
      characterEncoding=charset;
      applyCT();
    }
    void HttpServletResponse::setContentLength(int len) { throw NotImplementedException("à coder"); }
    void HttpServletResponse::setContentType(const String& type)
    {
      contentType=type;
      applyCT();
    }
    void HttpServletResponse::applyCT() {
    	String ct=contentType;
	if ( ! characterEncoding.empty() ) {
	  apacheRequest->content_encoding=apr_pstrdup(apacheRequest->pool, characterEncoding.c_str());
	  ct = ct + "; charset=" + characterEncoding;
	}
	apacheRequest->content_type=apr_pstrdup(apacheRequest->pool,ct.c_str());
    }
    void HttpServletResponse::setBufferSize(int size) { throw NotImplementedException("à coder"); }
    int HttpServletResponse::getBufferSize() const { throw NotImplementedException("à coder"); }
    void HttpServletResponse::flushBuffer() {
         checkApacheRequest();
	 ap_rflush(apacheRequest);
	 ap_flush_conn(apacheRequest->connection);
    }
    void HttpServletResponse::resetBuffer() { throw NotImplementedException("à coder"); }
    bool HttpServletResponse::isCommitted() const { throw NotImplementedException("à coder"); }
    void HttpServletResponse::reset() { throw NotImplementedException("à coder"); }
    void HttpServletResponse::setLocale(const String& loc) { throw NotImplementedException("à coder"); }
    String HttpServletResponse::getLocale() const { throw NotImplementedException("à coder"); }

    String HttpServletResponse::dump(const String& str) const {
      return String("HttpServletResponse(")+str+")";
    }

}
