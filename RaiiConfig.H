#ifndef __RAIICONFIG_H__
#define __RAIICONFIG_H__

#include "pthread_iface.H"


namespace raii {
  
  class RaiiConfig {
    public:
    int debug;
    int override_eternal;
    int segfault_handler;
    const char *tmpdir;
    const char *dsodir;
    const char *raiipp;
    const char *raiiroute;
    const char *contextpath;
    const char *sqlconnection;
    int sqlpoolsize;
    const char *buildcmd;
    Map<String, String> *parameter;
    Mutex mutex;

    RaiiConfig() : debug(-1),
		override_eternal(-1),
		tmpdir(DEFAULT_TMPDIR),
		dsodir(DEFAULT_DSODIR),
		raiipp(DEFAULT_RAIIPP),
                raiiroute(DEFAULT_RAIIROUTE),
		contextpath(DEFAULT_CONTEXTPATH),
		sqlconnection(DEFAULT_SQLCONNECTION),
		sqlpoolsize(-1),
		buildcmd(DEFAULT_BUILDCMD),
		parameter(new Map<String,String> ),
		mutex() {}
  ~RaiiConfig() {
	delete parameter;
  }

};

  RaiiConfig *get_dconfig(const request_rec *r);
  RaiiConfig *get_sconfig(const server_rec *s);
  RaiiConfig *get_rconfig(const request_rec *r);


}
/*
#include "Object.h"
#include "HttpServletRequest.h"
#include "ServletContext.h"
*/

#endif /* __RAIICONFIG_H__ */
