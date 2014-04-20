#ifndef RAIIGETREQUESTDISPATCHER_H
#define RAIIGETREQUESTDISPATCHER_H

#include "Object.H"
#include "RequestDispatcher.H"


namespace raii {


class GetRequestDispatcher : public virtual Object{
  String contextPath;
public:
  GetRequestDispatcher(const String& cp);
  ~GetRequestDispatcher();
  /*! returns a raii::RequestDispatcher for the specified servlet
   * \param path should point to a valid servlet
   */
  RequestDispatcher getRequestDispatcher(const String& path);

  /*! not implemented */
  RequestDispatcher getNamedDispatcher(String name);
};

}

#endif
