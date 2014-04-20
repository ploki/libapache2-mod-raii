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

const char *cfg_debug(cmd_parms *parms, void *d, int yes)
{
  CFG_FLAG( debug );
}

const char *cfg_segfault_handler(cmd_parms *parms, void *d, int yes)
{
  CFG_FLAG( segfault_handler );
}
const char *cfg_override_eternal(cmd_parms *parms, void *d, int yes)
{
  CFG_FLAG( override_eternal );
}

const char *cfg_tmpdir(cmd_parms *parms, void *d, const char *optstr)
{
  CFG_TAKE1( tmpdir );
}
const char *cfg_dsodir(cmd_parms *parms, void *d, const char *optstr)
{
  CFG_TAKE1( dsodir );
}

const char *cfg_raiipp(cmd_parms *parms, void *d, const char *optstr)
{
  CFG_TAKE1( raiipp );
}

const char *cfg_raii_route(cmd_parms *parms, void *d, const char *optstr)
{
  CFG_TAKE1( raiiroute );
}

const char *cfg_context_path(cmd_parms *parms, void *d, const char *optstr)
{
  CFG_TAKE1( contextpath );
}
const char *cfg_sqlconnection(cmd_parms *parms, void *d, const char *optstr)
{
  CFG_TAKE1( sqlconnection );
}

const char *cfg_sqlpoolsize(cmd_parms *parms, void *d, const char *optstr)
{
  static_cast<RaiiConfig *>(d)->sqlpoolsize  =atoi(static_cast<const char*>(optstr));
  return NULL;
}

const char *cfg_buildcmd(cmd_parms *parms, void *d, const char *optstr)
{
  CFG_TAKE1( buildcmd );
}

const char *cfg_raiiparameter(cmd_parms *cmd, void *d,
                        const char *optionname, const char *optionvalue)
{
        //FIXME: valgrind se plein beaucoup de cette ligne faut croire que d n'est pas libéré en temps et en heure
  (*(static_cast<RaiiConfig *>(d))->parameter)[optionname]=optionvalue;
  return NULL;
}

extern "C" apr_status_t cleanup_cfg(void *cfg) {
  //delete reinterpret_cast<Map<String, String> *>(themap);
  reinterpret_cast<RaiiConfig*>(cfg)->~RaiiConfig();
  return OK;
}
extern "C" apr_status_t cleanup_noop(void *arg) {
  //FIXME peut-être qu'il vaut mieux appeler cleanup_cfgmap
  return OK;
}

void *create_raii_config(apr_pool_t *p, char *dummy)
{
  void *buf_newcfg = apr_pcalloc(p, sizeof(RaiiConfig));
  apr_pool_cleanup_register(p,buf_newcfg,cleanup_cfg,cleanup_noop);
  RaiiConfig *newcfg = new(buf_newcfg) RaiiConfig;

  return newcfg;
}

void *merge_raii_configs(apr_pool_t *p, void *basev, void *addv)
{
  void *buf_newcfg = apr_pcalloc(p, sizeof(RaiiConfig));
  apr_pool_cleanup_register(p,buf_newcfg,cleanup_cfg,cleanup_noop);
  RaiiConfig *newcfg = new(buf_newcfg) RaiiConfig;
  RaiiConfig *base = static_cast<RaiiConfig *>(basev);
  RaiiConfig *add = static_cast<RaiiConfig *>(addv);

  Lock(add->mutex);
  
  MERGE_FLAG( debug );

  MERGE_FLAG( override_eternal );

  MERGE_TAKE1( tmpdir , DEFAULT_TMPDIR );
  MERGE_TAKE1( dsodir , DEFAULT_DSODIR );
  MERGE_TAKE1( raiipp , DEFAULT_RAIIPP );
  MERGE_TAKE1( raiiroute, DEFAULT_RAIIROUTE );
  MERGE_TAKE1( contextpath , DEFAULT_CONTEXTPATH );
  MERGE_TAKE1( buildcmd , DEFAULT_BUILDCMD );
  MERGE_TAKE1( sqlconnection , DEFAULT_SQLCONNECTION );

  if ( base ) {
    delete newcfg->parameter;
    newcfg->parameter=new Map<String, String>(*(base->parameter));
  }
  else {
    delete newcfg->parameter;
    newcfg->parameter=new Map<String, String>;
  }
  if ( add )
    for ( Map<String, String>::const_iterator it=add->parameter->begin() ;
          it != add->parameter->end() ;
          it++ ) {
            (*(newcfg->parameter))[it->first]=it->second;
    }
  // attention, c'est un kludge ( pas bool, mais bien int )
  MERGE_FLAG( sqlpoolsize );


  return newcfg;
}

}
