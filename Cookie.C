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
  Cookie::Cookie() : Object(), m_MaxAge(-1), m_Secure(false) {
  }

  Cookie::~Cookie() {};

  Cookie::operator String() const
  {
    String ret;

    ret+= m_Name + "=" + m_Value;
    if ( m_MaxAge != -1 )
    {
      char *date=static_cast<char*>(apr_pcalloc(apacheRequest->pool,APR_RFC822_DATE_LEN));
      apr_rfc822_date(date,(static_cast<apr_time_t>(m_MaxAge)) * 1000 * 1000 );
      ret+= String("; expires=") + date;
    }
    if ( ! m_Domain.empty() )
      ret+="; domain=" + m_Domain;
    if ( ! m_Path.empty() )
      ret+="; path=" + m_Path;
    if ( m_Secure )
      ret+="; secure";

    return ret;
  }

  void Cookie::setComment(const String& theValue)
  {
    m_Comment = theValue;
  }


  String Cookie::getComment() const
  {
    return m_Comment;
  }

  void Cookie::setDomain(const String& theValue)
  {
    m_Domain = theValue;
  }


  String Cookie::getDomain() const
  {
    return m_Domain;
  }

  void Cookie::setMaxAge(long theValue)
  {
    m_MaxAge = theValue;
  }


  long Cookie::getMaxAge() const
  {
    return m_MaxAge;
  }

  void Cookie::setPath(const String& theValue)
  {
    m_Path = theValue;
  }


  String Cookie::getPath() const
  {
    return m_Path;
  }

  void Cookie::setName(const String& theValue)
  {
    m_Name = theValue;
  }


  String Cookie::getName() const
  {
    return m_Name;
  }

  void Cookie::setValue(const String& theValue)
  {
    m_Value = theValue;
  }


  String Cookie::getValue() const
  {
    return m_Value;
  }

  void Cookie::setVersion(const int& theValue)
  {
    m_Version = theValue;
  }


  int Cookie::getVersion() const
  {
    return m_Version;
  }

  void Cookie::setSecure(bool theValue)
  {
    m_Secure = theValue;
  }


  bool Cookie::getSecure() const
  {
    return m_Secure;
  }

  String Cookie::dump(const String& str) const {

    return String("Cookie(")+str+"): { "
	"Comment="+m_Comment
	+", Domain="+m_Domain
	+", MaxAge="+itostring(m_MaxAge)
	+", Path="+m_Path
	+", Name="+m_Name
	+", Value="+m_Value
	+", Version="+itostring(m_Version)
	+", Secure="+(m_Secure?"true":"false")+" }";
  }


}
