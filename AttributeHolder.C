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
 * ARE DISCLAIMED. IN NO EVENT SHALL G.Gimenez SA BE LIABLE FOR ANY DIRECT,
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
