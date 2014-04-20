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

#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <sys/utsname.h>


namespace raii {

  //il semble que les fonctions dl* ne soient pas réentrantes
  Mutex dlMutex;

  //vérifie si le dso est toujours d'actualité
  ptr<HttpServlet> ServletContainer::getServlet()
  {
    Lock l1(mutex);

    if ( !isServletBuilt() ) {
	     buildServlet();
	     if ( servletFactory )
        	     unloadServlet();
    }
    Map<pthread_t, ptr<HttpServlet> >::iterator it = servlet.find(pthread_self());
    if ( it == servlet.end() ) {

    	if ( !servletFactory )
    		loadServlet();
    	else
    		servlet[pthread_self()]=servletFactory();
    	return servlet[pthread_self()];
    }
    else {
            return it->second;
     }
  }

  static String getArch() {
	String arch;

#if !defined(__i386__) && !defined(__x86_64__)
    struct utsname uts;
    uname(&uts);
    arch= uts.machine;
#else
# if defined(__i386__)
    arch="i686";
# endif
# if defined(__x86_64__)
    arch="x86_64";
# endif
#endif
    return arch;
  }

  // crée le container et compilation
  ServletContainer::ServletContainer(request_rec* r) :
    Object(),
    file(r->filename),
    modification_time("0"),
    sofile(""),
    dlhandle(NULL),
    servletFactory(NULL),
    servlet(),
    errorbuffer("")
  {
    RaiiConfig *cfg=get_dconfig(r);
    sofile+=cfg->dsodir;
    sofile+=file;
#ifdef HAVE_TLS
    sofile+=".tls";
#else
    sofile+=".tsd";
#endif
    sofile+="."+getArch();
    sofile+=".so";
  }


  void ServletContainer::buildServlet()
  {
    RaiiConfig *cfg=get_dconfig(apacheRequest);
    char *cmd;

    errorbuffer="";

    cmd=apr_psprintf(apacheRequest->pool,"/usr/bin/RaiiBuild "
                     " \"%s\""
		     " \"%s\""
		     " \"%s\""
		     " \"%s\" 2>&1",getArch().c_str(),cfg->tmpdir,cfg->dsodir,file.c_str());
    Logger log("raii");
    log.debug();
    log(HERE)("Building servlet "+file);

    FILE *p=popen(cmd,"r");
    if ( ! p )
      {
	errorbuffer+=String("popen");
	throw BuildException(errorbuffer.c_str());
      }
    
    char buf[1028];
    int i;
    while ( (i=fread(buf,1,1024,p)) )
    {
      buf[i]='\0';
      errorbuffer+=buf;
      
      if ( i < 1024 )
	break;
    }
    
    if ( pclose (p) )
      {
	//echec de la compilation...
	//errorbuffer+="pclose";
	throw BuildException(errorbuffer.c_str());
      }
    

  }
  
  bool ServletContainer::isServletBuilt()
  {
    struct stat raiist,sost;

    if ( stat(file.c_str(),&raiist) != 0 )
      {
	throw ServletException((file+" not found").c_str());
      }
      modification_time=itostring(raiist.st_mtime);

    if ( stat((sofile+"."+modification_time).c_str(),&sost) == 0
	 && sost.st_mtime > raiist.st_mtime )
      {
	return true;
      }

    return false;
  }
  void ServletContainer::loadServlet()
  {
    Logger log("raii");
    log.debug();
    log(HERE)("Loading servlet "+sofile+"."+modification_time);
    errorbuffer="";

    {
      Lock l1(dlMutex);

      dlhandle=dlopen((sofile+"."+modification_time).c_str(),RTLD_NOW|RTLD_GLOBAL);
      if ( ! dlhandle )
        {
          errorbuffer+=dlerror();
	  throw DynamicLinkerException(errorbuffer.c_str());
        }

      servletFactory=reinterpret_cast<HttpServlet *(*)()>
          (dlsym(dlhandle,"servletFactory"));

      if ( !servletFactory )
      {
	errorbuffer+=dlerror();
	throw DynamicLinkerException(errorbuffer.c_str());
      }

      servlet[pthread_self()]=servletFactory();
      if ( ! servlet[pthread_self()] )
      {
	errorbuffer+=dlerror();
	throw DynamicLinkerException(errorbuffer.c_str());
      }
    }
  }
  void ServletContainer::unloadServlet()
  {
    Logger log("raii");
    log.debug();
    log(HERE)("Unloading servlet "+sofile+"."+modification_time);
    servlet.clear();
    servletFactory=NULL;
    //delete servlet;
    if ( dlhandle ) {
      Lock l1(dlMutex);
      dlclose(dlhandle);
    }
    dlhandle=NULL;
    //servlet=NULL;
  }

  ServletContainer::~ServletContainer()
  {
    Logger log("raii");
    log.debug();
    log("cleaning container for "+file);
    Lock l1(mutex);
    unloadServlet();
  }
}
