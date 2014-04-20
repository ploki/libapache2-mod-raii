#ifndef RAIICOOKIE_H
#define RAIICOOKIE_H

#include <Object.H>

namespace raii {

class Cookie : public Object
{
  String m_Comment;
  String m_Domain;
  long m_MaxAge;
  String m_Path;
  String m_Name;
  String m_Value;
  int m_Version;
  bool m_Secure;
public:
  Cookie();
  ~Cookie();

  operator String() const;
  void setComment(const String& theValue);
  String getComment() const;
  void setDomain(const String& theValue);
  String getDomain() const;
  void setMaxAge(long theValue);
  long getMaxAge() const;
  void setPath(const String& theValue);
  String getPath() const;
  void setName(const String& theValue);
  String getName() const;
  void setValue(const String& theValue);
  String getValue() const;
  void setVersion(const int& theValue);
  int getVersion() const;
  void setSecure(bool theValue);
  bool getSecure() const;

  virtual String dump(const String& str) const;

};

}

#endif
