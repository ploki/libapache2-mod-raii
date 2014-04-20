#include "raii.H"
#include <new>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

namespace raii {
#ifdef DEBUG_ALLOC
long long int total_mem=0;
int total_objects=0;
#endif

extern "C" {
  static void register_hooks(apr_pool_t *p);
  static int raii_init(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s) __attribute__((unused));
  static void raii_child_init(apr_pool_t *p, server_rec *s) __attribute__((unused));
};


/************************
 * Global Dispatch List *
 ************************/

// We have to use C style linkage for the API functions that will be
// linked to apache.
extern "C" {
  module AP_MODULE_DECLARE_DATA raii_module =
    { STANDARD20_MODULE_STUFF,
      create_raii_config,          /* create per-directory config structure */
      merge_raii_configs,          /* merge per-directory config structures */
      NULL,//create_raii_config,     /* create per-server config structure */
      NULL,//merge_raii_configs,     /* merge per-server config structures */
      raii_cmds,                   /* command apr_table_t */
      register_hooks    /* register hooks */
    };
};



extern "C" {
  const command_rec raii_cmds[] =
    {
      AP_INIT_FLAG("Debug",cfg_debug,NULL,ACCESS_CONF|OR_OPTIONS,
		   "Weither to print debug informations"),
      AP_INIT_FLAG("OverrideEternal",cfg_override_eternal,NULL,ACCESS_CONF|OR_OPTIONS,
		   "Weither to override eternal attribute of servlets"),
      AP_INIT_FLAG("SegfaultHandler",cfg_segfault_handler,NULL,RSRC_CONF,
				"Weither to install a Segfault Handler"),
      AP_INIT_TAKE1("TmpDir",cfg_tmpdir,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
		    "Where to put preprocessed files"),
      AP_INIT_TAKE1("DsoDir",cfg_dsodir,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
		    "Where to put compiled files"),
      AP_INIT_TAKE1("RaiippPath",cfg_raiipp,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
		    "Where to find raii preprocessor"),
      AP_INIT_TAKE1("RaiiRoute",cfg_raii_route,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
		    "To set a particular route for load balancing"),
      AP_INIT_TAKE1("ContextPath",cfg_context_path,NULL,ACCESS_CONF|OR_OPTIONS,
		    "Where to find raii preprocessor"),
      AP_INIT_TAKE1("SqlConnection",cfg_sqlconnection,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
		    "Define a sqlrelay connection url"),
      AP_INIT_TAKE1("SqlPoolSize",cfg_sqlpoolsize,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
		    "Define the number of connection"),
      AP_INIT_TAKE1("BuildCmd",cfg_buildcmd,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
		    "Where to find Build Command"),

      AP_INIT_TAKE2("RaiiParameter",cfg_raiiparameter,NULL,RSRC_CONF|ACCESS_CONF|OR_OPTIONS,
        "Define custom parameters"),
      /*AP_INIT_RAW("Cflags",cfg_cflags,NULL,ACCESS_CONF|RSRC_CONF,
	"Additional cflags to compile with"), */
      AP_FINITO
    };
};




static void register_hooks(apr_pool_t *p)
{
        /* for directories, set the mod_autoindex to be called after us */
  //static const char * const after[] = { "mod_include.c", NULL };
  ap_hook_handler(raii_launch, NULL, NULL, APR_HOOK_MIDDLE);
  ap_hook_post_config(raii_init, NULL, NULL, APR_HOOK_LAST);
  ap_hook_child_init(raii_child_init, NULL, NULL, APR_HOOK_LAST);
}

//cannot throw in these handlers
static void raii_new_handler() {
	raise(SIGABRT);
}
static void raii_terminate_handler() {
	raise(SIGABRT);
}

static void raii_unexpected_handler() {
	raise(SIGABRT);
}


extern "C" apr_status_t cleanup_sctx(void *sctx) {

	Logger log("raii("+itostring(getpid())+")");
	log.debug();
        log("cleaning ServletContexts");

  reinterpret_cast<
	   Map<  String, ptr<ServletContext> >* >(sctx)
	->~Map<  String, ptr<ServletContext> >  ();
  return OK;
}

extern "C" void sig_nop(int sig,siginfo_t *siginfo, void *secret) {
	return;
}


extern "C" void segfaulth(int sig,siginfo_t *siginfo, void *secret);

extern bool dumpStackFlag;

extern "C" void dispatch_signal(int sig,siginfo_t *siginfo, void *secret) {

	pid_t pid=getpid();
	pid_t tid=syscall(SYS_gettid);

	Logger log("dispatcher");
	log ("pid is "+ itostring(pid));
	log ("tid is "+ itostring(tid));

	if ( pid != tid ) {
		log(itostring(tid)+" requests tasktrace");
		segfaulth(sig,siginfo,secret);
		return;
	}
	char buf[256];

	log("dispatching");

	snprintf(buf,256,"/proc/%d/task/",pid);
	struct dirent **entry;
	int m = scandir(buf,&entry,0,versionsort);
	for (int n = 0 ; n < m ; ++n ) {
		int task = String(entry[n]->d_name).toi();
		free(entry[n]);
		if ( !task || task == tid )
			continue;
		log(itostring(tid)+" sends signal to "+itostring(task));
		dumpStackFlag=true;
		kill(task,sig);
		//pthread_cond... plutôt, non ?
		time_t now = time(NULL);
		while ( dumpStackFlag && now + 1 >= time(NULL) )
			sched_yield();
		dumpStackFlag=false;
	}
	if ( m > 0 ) free(entry);

	if ( pid == tid ){
		log(itostring(tid)+" requests tasktrace");
		segfaulth(sig,siginfo,secret);
	}
	return;
}



static int raii_init(apr_pool_t *p, apr_pool_t *plog, apr_pool_t *ptemp, server_rec *s)
{

  {
    struct sigaction act;
    act.sa_handler=NULL;
    act.sa_sigaction=sig_nop;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO | SA_RESTART;
    if (sigaction(SIGUSR2,&act,NULL) )
      perror("sigaction");
  }

  std::set_new_handler(raii_new_handler);
  std::set_terminate(raii_terminate_handler);
  std::set_unexpected(raii_unexpected_handler);
  ap_add_version_component(p, RAII_HEADER_STRING);

  return OK;
}


static void raii_child_init(apr_pool_t *p, server_rec *s)
{
  if(1){
    struct sigaction act;
    act.sa_handler=NULL;
    act.sa_sigaction=segfaulth;
    sigemptyset(&act.sa_mask);
    act.sa_flags=SA_SIGINFO | SA_RESTART;
    if (sigaction(SIGSEGV,&act,NULL) )
      perror("sigaction");
    if (sigaction(SIGILL,&act,NULL) )
      perror("sigaction");
    if (sigaction(SIGFPE,&act,NULL) )
      perror("sigaction");
    if (sigaction(SIGBUS,&act,NULL) )
      perror("sigaction");
    if (sigaction(SIGABRT,&act,NULL) )
      perror("sigaction");

    act.sa_sigaction=dispatch_signal;
    if (sigaction(SIGUSR2,&act,NULL) )
      perror("sigaction");
  }

	Logger log("raii("+itostring(getpid())+")");
	log.debug();
	log("child init");
  Lock l(servletContextMutex);

  void* servletContext_buf = apr_pcalloc(p, sizeof(Map<String, ptr<ServletContext> >));
  apr_pool_cleanup_register(p,servletContext_buf,cleanup_sctx,/*cleanup_sctx*/apr_pool_cleanup_null);
  servletContext= new(servletContext_buf) Map <String, ptr<ServletContext> >;

  //dans cet ordre dans le but de voir les container désalloués après les pools
  raii::sql::SQLDriverContainer::clear();
  raii::sql::Connection::clearPools();

}




  RaiiConfig *get_dconfig(const request_rec *r)
  {
    if ( !r ) return NULL;
    RaiiConfig *plop=(RaiiConfig *)ap_get_module_config(r->per_dir_config, &raii_module);
    return plop;
  }
  // current server config
  RaiiConfig *get_sconfig(const server_rec *s)
  {
    if ( !s ) return NULL;
    return (RaiiConfig *) ap_get_module_config(s->module_config, &raii_module);
  }
  //current req config
  RaiiConfig *get_rconfig(const request_rec *r)
  {
    if ( !r ) return NULL;
    return (RaiiConfig *)ap_get_module_config(r->request_config, &raii_module);
  }

}
