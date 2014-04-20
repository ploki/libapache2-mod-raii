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
