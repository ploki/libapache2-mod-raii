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

String url_encode(const String& str)
{
  if ( str.empty() )
    return String();
  apr_pool_t *pool=apacheRequest->pool;
  char *s=apr_pstrdup(pool,str.c_str());
  int i=0,j=0;
  char *ns=static_cast<char*>(apr_palloc(pool,strlen(s)*3+1));
  while ( s[i] != '\0' )
  {
    if ( s[i] == ' ' )
    {
      ns[j++]='+';
      i++;
    }
    else if ( ! ( isalnum(s[i]) || s[i] == '-' || s[i] == '.' || s[i] == '_' ) )
      j+=sprintf(&ns[j],"%%%02X",static_cast<unsigned char>(s[i++]) );
    else
      ns[j++]=s[i++];

  }
  ns[j]='\0';
  return ns;
}
String url_decode(const String& str)
{
  if ( str.empty() )
    return String();
  apr_pool_t *pool=apacheRequest->pool;
  char *s=apr_pstrdup(pool,str.c_str());
  int i=0,
    j=0,
    l=strlen(s);
  char *ns=static_cast<char*>(apr_palloc(pool,strlen(s)+1));
  char s1,s2;
  while ( s[i] != '\0' )
    {
      if ( s[i] == '+' )
        {
          ns[j++]=' ';
          i++;
        }
      else if ( s[i] == '%' && l-i > 2 && isxdigit(s[i+1]) && isxdigit(s[i+2]) )        {
          //gr8k !
          s1=s[i+1]>='a'?s[i+1]-'a'+10:(s[i+1]>='A'?s[i+1]-'A'+10:(s[i+1]-'0'));          s2=s[i+2]>='a'?s[i+2]-'a'+10:(s[i+2]>='A'?s[i+2]-'A'+10:(s[i+2]-'0'));          ns[j++]=s1*16+s2;
          i+=3;
        }
      else
        ns[j++]=s[i++];
    }
  ns[j]='\0';
  return ns;
}

String escapeHTML(const String& str) {
	String ret;
	for ( String::const_iterator it = str.begin(); it != str.end() ; ++it ) {
		if ( *it == '&' )
			ret += "&amp;";
		else if ( *it == '<' )
			ret += "&lt;";
		else
			ret += *it;
	}
	return ret;
}
String escapeAttribute(const String& str) {
	String ret;
	for ( String::const_iterator it = str.begin(); it != str.end() ; ++it ) {
		if ( *it == '+' )
			ret += "%2B";
		else if ( *it == '&' )
			ret += "&amp;";
		else if ( *it == '"' )
			ret += "&quot;";
		else
			ret += *it;
	}
	return ret;
}

extern "C"
int _callback_table(void *rec, const char *key, const char *value)
{
  (reinterpret_cast<Vector<String>*>(rec))->push_back(key);
  return 1;
}

Vector<String> raii_get_tables_keys(apr_pool_t *pool, apr_table_t *t)
{
  Vector<String> ret;
  if ( !( pool && t ) )
    throw IllegalArgumentException("pool or table is null");
  apr_table_do(_callback_table,
               reinterpret_cast<void*>(&ret),t,NULL);
  return ret;
}

}
