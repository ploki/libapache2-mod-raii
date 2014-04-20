#ifndef __OBJECT_H__
#define __OBJECT_H__

namespace raii {
class String;

  class Object {
  public:
    Object();
    Object(const Object& o);
    virtual ~Object();
    virtual String dump(const String& str) const;
  };

  //extern "C" apr_status_t object_cleanup(Object *obj);
}

#endif /* __OBJECT_H__ */
