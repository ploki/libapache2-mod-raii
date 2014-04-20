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

