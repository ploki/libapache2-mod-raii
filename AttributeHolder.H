#ifndef __ATTRIBUTE_HOLDER_H__
#define __ATTRIBUTE_HOLDER_H__

#include "Object.H"
#include "ptr.H"

#include "raii.H"

namespace raii {
class AttributeHolder : public virtual Object {
  mutable Map<String, ptr<Object> > attribute;
  Mutex lockMe;
  public:
    AttributeHolder();
    ptr<Object> getAttribute(const String& name) const;
    Vector<String> getAttributeNames() const;
    void removeAttribute(const String& name);
    void setAttribute(const String& name,const ptr<Object>& value);
    void clearAttributes();
    virtual ~AttributeHolder();
};

}
#endif /* __ATTRIBUTE_HOLDER_H__ */
