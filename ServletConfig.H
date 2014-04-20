#ifndef __SERVLETCONFIG_H__
#define __SERVLETCONFIG_H__

#include "Object.H"
#include "ServletContext.H"
#include "ptr.H"


namespace raii {
  class ServletContext;
  class ServletConfig : public virtual Object {
    //static Map <const char *, ServletContext *> servletContext;
    Map<String, String> initParameter;
  public:
    ServletConfig() : Object(), initParameter() {}
    void init(const Map<String,String>& initParameters);
    ptr<ServletContext> getServletContext() const;
    Vector<String> getInitParameterNames() const;
    String getInitParameter(const String& name) const;
    String getServletName () const;

    //ServletConfig *getServletConfig(const String& uri);

    virtual String dump(const String& str) const;
  };
}

#endif /* __SERVLETCONFIG_H__ */
