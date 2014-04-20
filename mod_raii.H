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
#ifndef __MOD_RAII_H__
#define __MOD_RAII_H__

// Apache stuff
//  Its important to use "" instead of <> and to have the -I flags in
//  the right order in the Makefile because there is an Apache alloc.h
//  that is completely different from the system alloc.h.


#ifdef DEBUG_ALLOC
extern long long int total_mem;
extern int total_objects;

inline void * operator new(size_t size) {
	void *p = malloc ( size + sizeof(long long int) );
	if ( !p )
		throw std::bad_alloc();
	long long int * i = (long long int *)p;
	*i = size;
	total_objects++;
        total_mem+=size;
	return (void*)(i+1);
}

inline void operator delete(void *p) {
	long long int *i = (long long int*)p;
	if ( !p ) return;
	i--;
	total_mem-=*i;
	total_objects--;
	free(i);
}
#endif

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_request.h"
#include "http_protocol.h"
#include "http_log.h"
#include "http_main.h"
//#include "util_script.h"
//#include "ap_config.h"
#include "apr.h"
#include "ap_compat.h"
#include "apr_strings.h"
#include "apr_base64.h"



// Apache's compatibility warnings are of no concern to us.
#undef strtoul



namespace raii {
/************************
 * Forward Declarations *
 ************************/
//raii.C
extern "C" apr_status_t raii_launch(request_rec *r);

//config.C
extern "C" {
  void *create_raii_config(apr_pool_t *p, char *dummy);
  void *merge_raii_configs(apr_pool_t *p, void *basev, void *addv);
  const char *cfg_debug(cmd_parms *parms, void *d, int yes);
  const char *cfg_segfault_handler(cmd_parms *parms, void *d, int yes);
  const char *cfg_override_eternal(cmd_parms *parms, void *d, int yes);
  const char *cfg_tmpdir(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_dsodir(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_raiipp(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_raii_route(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_context_path(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_sqlconnection(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_sqlpoolsize(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_buildcmd(cmd_parms *parms, void *d, const char *optstr);
  const char *cfg_raiiparameter(cmd_parms *cmd, void *d, const char *optionname, const char *optionvalue);

  extern const command_rec raii_cmds[];

};

}


#define RAII_HEADER_STRING "RAII/2.0"

/*
 *Configuration default values
 */
#define DEFAULT_DSODIR "/tmp/raii/dso"
#define DEFAULT_TMPDIR "/tmp/raii/tmp"
#define DEFAULT_RAIIPP "/usr/bin/raiipp"
#define DEFAULT_RAIIROUTE ""
#define DEFAULT_CONTEXTPATH ""
#define DEFAULT_BUILDCMD "/usr/bin/RaiiBuild"
#define DEFAULT_SQLCONNECTION "sqlite:///tmp/raii/default.db"

#define DEFAULT_DEBUG 0
#define DEFAULT_SEGFAULT_HANDLER 0
#define DEFAULT_SQLPOOLSIZE 5
#define DEFAULT_OVERRIDE_ETERNAL 0


//don't work ?
#ifdef AP_INIT_TAKE1
#undef AP_INIT_TAKE1
#undef AP_INIT_TAKE2
#undef AP_INIT_FLAG
#endif

#define AP_INIT_FLAG(directive, func, mconfig, where, help)	\
  { directive, cmd_func(func), mconfig, where, FLAG, help }

#define AP_INIT_TAKE1(directive, func, mconfig, where, help)	\
  { directive, cmd_func(func), mconfig, where, TAKE1, help }

#define AP_INIT_TAKE2(directive, func, mconfig, where, help) \
  { directive, cmd_func(func), mconfig, where, TAKE2, help }

#define AP_FINITO { NULL, cmd_func(0), NULL, 0, cmd_how(0), NULL }



#define CFG_FLAG(name)		    \
  RaiiConfig *cfg= static_cast<RaiiConfig *>(d);	\
  cfg-> name =yes;		    \
  return NULL;

#define CFG_TAKE1( name )           \
  static_cast<RaiiConfig *>(d)-> name  =static_cast<const char*>(optstr); \
  return NULL;

#define MERGE_FLAG( name )	  \
  if ( add-> name != -1 )	  \
    newcfg-> name = add-> name;      \
  else				  \
    newcfg-> name = base-> name;     \
  
#define MERGE_TAKE1( name , default )		\
  if ( strcmp( add-> name , default ) )		\
    newcfg-> name = apr_pstrdup(p,add-> name );	\
  else						\
    newcfg-> name = apr_pstrdup(p,base-> name );


#endif /* __MOD_RAII_H__ */
