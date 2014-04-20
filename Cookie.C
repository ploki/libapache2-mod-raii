#include "raii.H"

namespace raii {
  Cookie::Cookie() : Object(), m_MaxAge(-1), m_Secure(false) {
  }

  Cookie::~Cookie() {};

  Cookie::operator String() const
  {
    String ret;

    ret+= m_Name + "=" + m_Value;
    if ( m_MaxAge != -1 )
    {
      char *date=static_cast<char*>(apr_pcalloc(apacheRequest->pool,APR_RFC822_DATE_LEN));
      apr_rfc822_date(date,(static_cast<apr_time_t>(m_MaxAge)) * 1000 * 1000 );
      ret+= String("; expires=") + date;
    }
    if ( ! m_Domain.empty() )
      ret+="; domain=" + m_Domain;
    if ( ! m_Path.empty() )
      ret+="; path=" + m_Path;
    if ( m_Secure )
      ret+="; secure";

    return ret;
  }

  void Cookie::setComment(const String& theValue)
  {
    m_Comment = theValue;
  }


  String Cookie::getComment() const
  {
    return m_Comment;
  }

  void Cookie::setDomain(const String& theValue)
  {
    m_Domain = theValue;
  }


  String Cookie::getDomain() const
  {
    return m_Domain;
  }

  void Cookie::setMaxAge(long theValue)
  {
    m_MaxAge = theValue;
  }


  long Cookie::getMaxAge() const
  {
    return m_MaxAge;
  }

  void Cookie::setPath(const String& theValue)
  {
    m_Path = theValue;
  }


  String Cookie::getPath() const
  {
    return m_Path;
  }

  void Cookie::setName(const String& theValue)
  {
    m_Name = theValue;
  }


  String Cookie::getName() const
  {
    return m_Name;
  }

  void Cookie::setValue(const String& theValue)
  {
    m_Value = theValue;
  }


  String Cookie::getValue() const
  {
    return m_Value;
  }

  void Cookie::setVersion(const int& theValue)
  {
    m_Version = theValue;
  }


  int Cookie::getVersion() const
  {
    return m_Version;
  }

  void Cookie::setSecure(bool theValue)
  {
    m_Secure = theValue;
  }


  bool Cookie::getSecure() const
  {
    return m_Secure;
  }

  String Cookie::dump(const String& str) const {

    return String("Cookie(")+str+"): { "
	"Comment="+m_Comment
	+", Domain="+m_Domain
	+", MaxAge="+itostring(m_MaxAge)
	+", Path="+m_Path
	+", Name="+m_Name
	+", Value="+m_Value
	+", Version="+itostring(m_Version)
	+", Secure="+(m_Secure?"true":"false")+" }";
  }


}
