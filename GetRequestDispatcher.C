#include "raii.H"

namespace raii {

  GetRequestDispatcher::GetRequestDispatcher(const String& cp) : contextPath(cp) {
  }
  GetRequestDispatcher::~GetRequestDispatcher() {
  }
  RequestDispatcher GetRequestDispatcher::getRequestDispatcher(const String& path) {
    return RequestDispatcher(contextPath, path);
  }
  
  RequestDispatcher GetRequestDispatcher::getNamedDispatcher(String name) {
    throw NotImplementedException("à coder");
    //return RequestDispatcher(this,name);
  }


}
