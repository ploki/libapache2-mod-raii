#include "raii.H"

namespace raii {
  apr_status_t object_cleanup(Object *obj)
  {
    delete obj;
    return OK;
  }
  Object::Object() {}
  Object::Object(const Object& o) {}
  Object::~Object() {}
  String Object::dump(const String& str) const {
    throw NotImplementedException("Object->dump doesn't exist");
  }

}
