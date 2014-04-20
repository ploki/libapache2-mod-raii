#ifndef __HTTPSESSION_H__
#define __HTTPSESSION_H__

#include "Object.H"
#include "ServletContext.H"
#include "AttributeHolder.H"

#include <ctime>

namespace raii {

  class HttpSession : public AttributeHolder {
    bool invalid;
    time_t creationTime;
    String id;
    String user;
    mutable time_t lastAccessedTime;
    mutable time_t savedLastAccessedTime;
    int maxInactiveInterval;
    bool m_new;
    // il est primordial de ne pas utiliser de ptr<> pour cause
    // de référence circulaire qui provoque un déchargement
    // des servlet au moment de la collection
    ServletContext *m_servletContext;
    Mutex mutex;
  public:

    HttpSession(ServletContext *sc);
    void setUser(const String& name);
    String getUser() const;
    time_t getCreationTime() const;
    String getId() const;
    time_t getLastAccessedTime() const;
    void touch() const;
    void untouch() const;
    int getMaxInactiveInterval() const;
    ServletContext* getServletContext() const;
    void invalidate();
    bool isValid() const;
    bool isNew() const;
    void setNew(bool theValue);
    void setMaxInactiveInterval(int interval);
    String dump(const String& str) const;
  };

}

#endif /* __HTTPSESSION_H__ */
