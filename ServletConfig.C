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

ptr<ServletContext> ServletConfig::getServletContext() const
{
  RaiiConfig *cfg=get_dconfig(apacheRequest);

  Lock l1(servletContextMutex);
  if ( !cfg )
    throw ApacheException("la configuration du répertoire obtenue est nulle\n");
  
  ptr<ServletContext>
      ctx=(*servletContext)[cfg->contextpath];

  if ( !ctx )
    {
	    ctx=new ServletContext(cfg->contextpath);
	    if ( ! ctx )
	      throw Exception("Arg!");
      (*servletContext)[cfg->contextpath]=ctx;
    }
  return ctx;
}
  
Vector<String> ServletConfig::getInitParameterNames() const
{
  RaiiConfig *cfg=get_dconfig(apacheRequest);
  
  if ( !cfg )
    throw ApacheException("la configuration du répertoire obtenue est nulle\n");
  
  Lock(cfg->mutex);
  
  Vector<String> params;
  for ( Map<String,String>::const_iterator it=cfg->parameter->begin();
        it != cfg->parameter->end();
        it++ )
    params.push_back(it->first);
  
  return params;
}


  String ServletConfig::getInitParameter(const String& name) const
  {
    RaiiConfig *cfg=get_dconfig(apacheRequest);
  
    if ( !cfg )
      throw ApacheException("la configuration du répertoire obtenue est nulle\n");
    Lock(cfg->mutex);
    return (*(cfg->parameter))[name];
  }


  String ServletConfig::getServletName () const
  {
    return String(apacheRequest->filename);
  }
  /*
  ptr<ServletConfig> ServletConfig::getServletConfig(const string& uri)
  {
    return NULL;
  }
  */

  String ServletConfig::dump(const String& str) const {
	return String("ServletConfig(")+str+")="+getServletName();
  }
  
}

