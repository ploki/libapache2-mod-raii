#ifndef __RAII_H__
#define __RAII_H__

#include <cstdlib>
#include "Object.H"
#include "Vector.H"
#include "Exception.H"
#include "String.H"
#include "Regex.H"
#include "StringStream.H"
#include "List.H"
#include "Map.H"
#include "Set.H"
#ifdef __GXX_EXPERIMENTAL_CXX0X__
# include "HMap.H"
# include "HSet.H"
#endif


#if 0
//check complet
#include <cstring>
#include <cstdlib>
#include <cstdio>
void dumpst(FILE *f);

inline void operator delete(void* p) {
        char filename[1024];
        snprintf(filename,1024,"/tmp/malloc/%p",p);
        unlink(filename);
	if ( p ) free(p);
}
inline void* operator new(size_t s) {
	void *p=malloc(s);
        char filename[1024];
        snprintf(filename,1024,"/tmp/malloc/%p",p);
        FILE *f = fopen(filename,"w");
        if (!f ) throw raii::error::BadAllocException("debug file");
        fprintf(f,"malloc(%ld)\n",s);
        dumpst(f);
        fclose(f);
	if (!p) throw raii::error::BadAllocException("NULL");
	return p;
}

inline void operator delete[](void* p) {
	if ( p ) free(p);
}
inline void* operator new[](size_t s) {
	void *p=malloc(s);
	if (!p) throw raii::error::BadAllocException("NULL");
	return p;
}
#endif

#if 0
//check de malloc(0)
inline void operator delete(void* p) {
	if ( p ) free(p);
}
inline void* operator new(size_t s) {
        if ( s == 0 )  throw raii::error::BadAllocException("requested size is 0, it is a bug");
	void *p=malloc(s);
	if (!p) throw raii::error::BadAllocException("NULL");
	return p;
}
#endif


#include "mod_raii.H"
#include "RaiiConfig.H"
#include "pthread_iface.H"
#include "AttributeHolder.H"
#include "DefaultTags.H"
#include "ptr.H"


#include <setjmp.h>

#include <pthread.h>

  namespace raii {


/*!
 * \mainpage mod_raii documentation
 * \section intro_sec Introduction
 * This is a module for apache2 whiche enables compilation, loading and execution of C++ classes
 * and HTML pages with c++ embeded. I work on this module for over 4 years and I use it in production
 * on several applications (both professionally and personally). It is stable and reactive.
 * Web applications development is very pleasant and quite fast.
 *
 * Here is a list of features and characteristics.
 *
 * As the module is today :
 * \list

 * \li seamlessly integrates the Apache's operation and configurations directives ;
 * \li applications you develop are composed of .C (your controller) and .csp (your views) directly inside your web directories, heade files and/or libraries (your model) can be found elsewhere ;
 * \li controllers  (.C) and views (.csp) are compiled and loaded on the fly when the corresponding binary object is outdated. The server has not to be restarted too often ;
 * \li Each source file contains a class that inherits from HttpServlet and only overloads service(req,resp) (or doGet, doPost...) ;
 * \li raiipp preprocessor takes care of converting .csp files containing mainly html code into a class that inherits from HttpServlet ;
 * \li syntax used by the preprocessor is similar to JSP - you can also use personalized tags ;
 * \li Untrapped exceptions are displayed in the web %page with the stack unwinded to display functions names, source files and line numbers for easy debugging. Besides the com the compilation errors are alos exceptions and are displayed with pretty colors ;
 * \li signals "Illegal Instruction", "Floating Point Exception", "Segmentation Violation", "Bus Error" et "Abort" are intercepted weither they come from a request or not, and are detailed and processed like exceptions which are displayed. This prevents the server to die (there are limits anyway...) ;
 * \li connection to a lot of database via sqlrelay ;
 * \li or to PostgreSQL natively with internalt connection pool ;
 * \li or to SQLite with the same mechanism ;
 * \li or the database you love, two classes are to be overloaded (15 trival methods in total) to implement a database driver ;
 * \li some of the STL classes are overloaded (but very slightly) so you can embed them in sessions.
 * \endlist
 *
 * Home %page is http://blackmilk.fr/www/cms/dev/libapache2_mod_raii_en
 * \section classes Classes of interest
 * \subsection classes_main Main classes
 * \list
 * \li raii::HttpServletRequest The served http request
 * \li raii::HttpServletResponse The served http response
 * \li raii::HttpServlet The class you overload to implement your servlets
 * \endlist
 * \subsection classes_container Container classes
 * \list
 * \li raii::ptr A smart point with reference counting
 * \li raii::String %String manipulation class like %std::string
 * \li raii::Vector %Vector container like %std::vector
 * \li raii::Map %Map container like %std::map
 * \li raii::List %List container like %std::list
 * \li raii::StringStream %String Stream container like %std::stringstream
 * \endlist
 * \subsection classes_utilities Utility classes
 * \list
 * \li raii::Logger
 * \li raii::Regex
 * \li raii::Lock
 * \li raii::Mutex
 * \endlist
 * \subsection classes_sql Database access
 * \list
 * \li raii::sql::Connection
 * \li raii::sql::ResultSet
 * \li raii::sql::Begin to handle transactions
 * \endlist
 * \subsection classes_rendering Rendering classes
 * \list
 * \li raii::Cairo::Document to render PDF, PostScript, SVG and PNG surfaces.
 * \li raii::Cairo::BoxedText to embed in Cairo::Document formatted texts.
 * \endlist
 */



inline String ftostring(double d)
{
    StringStream ss;
    ss << d;
    return ss.str();
}
inline String itostring(int i)
{
    StringStream ss;
    ss << i;
    return ss.str();
}

inline String ltostring(long int li)
{
    StringStream ss;
    ss << li;
    return ss.str();
}

inline String lltostring(long long int lli )
{
    StringStream ss;
    ss << lli;
    return ss.str();
}

#ifdef HAVE_TLS
#define tls_ptr(type) __thread type*

#else

  template<typename _Tp>
  class tls_ptr
  {
    public:
      typedef _Tp element_type;
    private:
      pthread_key_t tsdkey;
      bool initialized;
      pthread_mutex_t _M_lock;

      void set(element_type *__p)
      {
        pthread_mutex_lock(&_M_lock);
        if ( ! initialized )
        {
          pthread_key_create(&tsdkey,NULL);
          initialized=true;
        }
        pthread_mutex_unlock(&_M_lock);

        pthread_setspecific(tsdkey,__p);
      }

      element_type *get()
      {
        if ( !initialized ) throw raii::error::NullPointerException("tsd key not initialized");
        return (element_type*)pthread_getspecific(tsdkey);
      }

    public:

      explicit tls_ptr(element_type* __p = 0)
      {
        pthread_mutex_init(&_M_lock,NULL);
        set(__p);
      }

      tls_ptr(tls_ptr& __a)
      {
        pthread_mutex_init(&_M_lock,NULL);
        set(__a.get());
      }

      template<typename _Tp1>
          tls_ptr(tls_ptr<_Tp1>& __a)
      {
        pthread_mutex_init(&_M_lock,NULL);
        set(__a.get());
      }

      tls_ptr& operator=(tls_ptr& __a)
      {
        set(__a.get());
        return *this;
      }
      template<typename _Tp1>
          tls_ptr& operator=(tls_ptr<_Tp1>& __a)
      {
        set(__a.get());
        return *this;
      }
      ~tls_ptr() {}
      element_type& operator*() const
      {
        return *get();
      }
      element_type* operator->()
      {
        return get();
      }
      tls_ptr& operator=(element_type *__p)
      {
        set(__p);
        return *this;
      }
      operator element_type*()
      {
        return get();
      }
  };
#define tls_ptr(type) tls_ptr<type>
#endif

  class SegfaultBuffer {
    public:
      Signal *exception;
      jmp_buf env;
      int signal;
      SegfaultBuffer(){}
  };

    extern tls_ptr(request_rec) apacheRequest;
    extern tls_ptr(SegfaultBuffer) segfaultBuffer;
  }


#include "ApacheSubRequest.H"
#include "ApacheInteraction.H"

#include "SQLDriver.H"
#include "SQLDriverContainer.H"
#include "ResultSetImpl.H"
#include "ResultSet.H"
#include "Connection.H"
#include "Begin.H"


#include "ServletContext.H"
#include "ServletConfig.H"
#include "HttpServletRequest.H"
#include "HttpServletResponse.H"
#include "HttpSession.H"
#include "HttpServlet.H"
#include "ServletContainer.H"
#include "RequestDispatcher.H"
#include "Cookie.H"

#ifndef RAIIMARK
#define SERVLET(name) Servlet_ ## name
#define exportServlet(name)
#else
#define exportServlet(ServletClassName) \
  extern "C" RAIIMARK(ServletClassName) *servletFactory() { return new RAIIMARK(ServletClassName)();}
#define SERVLET(ServletClassName)                  \
  RAIIMARK(ServletClassName)
#endif

#endif /* __RAII_H__ */
