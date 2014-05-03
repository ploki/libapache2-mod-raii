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
#include "raii.H"


#include <dlfcn.h>

namespace raii {
namespace sql {
        SQLDriverContainer::SQLDriverContainer(const String& scheme) : Object() {

                Logger log("raii::sql::SQLDriverContainer::SQLDriverContainer");
                log.debug();
                log("loading sql driver "+scheme);

                Lock l1(getDLMutex());

                dlhandle=dlopen(("libraii_sqldriver_"+scheme+".so").c_str(),RTLD_NOW|RTLD_GLOBAL);

                if ( ! dlhandle )
                        throw DynamicLinkerException(dlerror());


                isPoolable=reinterpret_cast<bool (*)()>(dlsym(dlhandle,"isPoolable"));
                if ( ! isPoolable )
                        throw DynamicLinkerException(dlerror());

                getDriverImpl=reinterpret_cast<ptr<SQLDriver> (*)( const String&,
                                     const String&,
                                     const String&,
                                     int port,
                                     const String&,
                                     const String&)>(dlsym(dlhandle,"getDriver"));
                if ( ! getDriverImpl )
                        throw DynamicLinkerException(dlerror());

                log("sql driver "+scheme+" loaded");

        }

        ptr<SQLDriver> SQLDriverContainer::getDriver(const String& user,
                                     const String& password,
                                     const String& host,
                                     int port,
                                     const String& path,
                                     const String& query) {

                return getDriverImpl(user,password,host,port,path,query);
        }


        /*static*/ Mutex& SQLDriverContainer::getMapMutex() {

                static Mutex mutex;
                return mutex;
        }

        /*static*/ Mutex& SQLDriverContainer::getDLMutex() {

                static Mutex mutex;
                return mutex;
        }

        /*static*/ Map<String, ptr<SQLDriverContainer> >& SQLDriverContainer::getSQLDriverContainers() {

                static Map<String, ptr<SQLDriverContainer> > map;
                return map;
        }



        SQLDriverContainer::~SQLDriverContainer() {

                Logger log("raii::sql::SQLDriverContainer::~SQLDriverContainer");
                log.debug();
                log("unloading sql driver");

                Lock l1(getDLMutex());
                isPoolable=NULL;
                getDriverImpl=NULL;
                if ( dlhandle )
                        dlclose(dlhandle); //FIXME: valgrind dit que dlclose fait malloc dans la gestion d'erreur, faut-il s'en soucier ?
        }


        /*static*/ void SQLDriverContainer::clear() {
                Lock l1(getMapMutex());
                getSQLDriverContainers().clear();
        }

        /*static*/ ptr<SQLDriverContainer> SQLDriverContainer::getSQLDriverContainer(const String& scheme) {

                Lock l1(getMapMutex());
                Map<String, ptr<SQLDriverContainer> >& driverMap = getSQLDriverContainers();
                ptr<SQLDriverContainer> sqlDriverContainer = driverMap[scheme];

                if ( ! sqlDriverContainer ) {
                        sqlDriverContainer = new SQLDriverContainer(scheme);
                        driverMap[scheme]= sqlDriverContainer;
                }

                return sqlDriverContainer;
        }


}
}
