#ifndef __SQLDRIVERCONTAINER_H__
#define __SQLDRIVERCONTAINER_H__

namespace raii {
namespace sql {

class SQLDriverContainer : public virtual Object {

        void *dlhandle;

        SQLDriverContainer(const SQLDriverContainer&); //no impl
        SQLDriverContainer& operator=(const SQLDriverContainer&); //no impl

        SQLDriverContainer(const String& scheme);

        static Mutex& getMapMutex();

        static Mutex& getDLMutex();

        static Map<String, ptr<SQLDriverContainer> >& getSQLDriverContainers();

        ptr<SQLDriver> (*getDriverImpl)( const String& user,
                                     const String& password,
                                     const String& host,
                                     int port,
                                     const String& path,
                                     const String& query);
        public:

        ~SQLDriverContainer();

        bool (*isPoolable)();

        ptr<SQLDriver> getDriver( const String& user,
                                     const String& password,
                                     const String& host,
                                     int port,
                                     const String& path,
                                     const String& query);

        static void clear();

        static ptr<SQLDriverContainer> getSQLDriverContainer(const String& scheme);

};
}
}
#endif

