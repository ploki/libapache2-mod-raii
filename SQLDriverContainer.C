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
