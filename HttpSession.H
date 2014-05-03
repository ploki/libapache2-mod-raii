/* 
 * Copyright (c) 2005-2011, Guillaume Gimenez <guillaume@blackmilk.fr>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of G.Gimenez nor the names of its contributors may
 *       be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL G.GIMENEZ BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors:
 *     * Guillaume Gimenez <guillaume@blackmilk.fr>
 *
 */
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
