#include "raii.H"

namespace raii {

  AttributeHolder::AttributeHolder() : attribute(), lockMe() {
  }
  ptr<Object> AttributeHolder::getAttribute(const String& name) const
  {
    Lock l1(lockMe);
    Map<String, ptr<Object> >::iterator it = attribute.find(name);
    if ( it != attribute.end() )
	return it->second;
    else
	return NULL;
  }
  Vector<String> AttributeHolder::getAttributeNames() const
  {
    Lock l1(lockMe);
    Vector<String> ret;
    for ( Map<String, ptr<Object> >::const_iterator it=attribute.begin() ;
          it != attribute.end() ;
          it ++ )
      ret.push_back(it->first);
    return ret;
  };
  void AttributeHolder::removeAttribute(const String& name)
  {
    //attribute[name]=NULL;
    Lock l1(lockMe);
    attribute.erase(name);
  };
  void AttributeHolder::setAttribute(const String& name,const ptr<Object>& value)
  {
    Lock l1(lockMe);
    attribute[name]=value;
  };
  void AttributeHolder::clearAttributes()
  {
    Lock l1(lockMe);
    attribute.erase(attribute.begin(),attribute.end());
  }
  AttributeHolder::~AttributeHolder()
  {
    clearAttributes();
  }


}
