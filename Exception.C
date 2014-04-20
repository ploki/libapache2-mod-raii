#include "raii.H"
#include <iostream>
#include <cxxabi.h>
#include <execinfo.h>
#include <dlfcn.h>
namespace raii {
namespace error {

  Throwable::Throwable(const char *message)
      :  message_(message?strdup(message):NULL), level(0) {
    fillInStackTrace();
  }
  
  Throwable::Throwable(const String& message)
	  : message_((!message.isNull())?strdup(message.c_str()):NULL), level(0) {
	  fillInStackTrace();
  }

  Throwable::~Throwable() throw() {
    if ( message_ ) free(static_cast<void*>(message_));
  }

  String Throwable::getMessage() const {
    if ( message_ )
      return message_;
    return "";
  }
  
  void Throwable::setMessage(const String& message) {
        if ( message_ ) free(static_cast<void*>(message_));
        message_ = ! message.isNull()
                        ? strdup(message.c_str())
                        : NULL;
  }
  
  String Throwable::getLocalizedMessage() const {
    return getMessage();
  }
  
  const char *Throwable::what()  const throw()  {
    return getStackTrace().c_str();
  }
  
  void Throwable::printStackTrace() const {
    std::cerr << getStackTrace();
    fflush(stderr);
  }
  
  void Throwable::poorManPrintStackTrace(bool dem/*angle*/) const {
	  Dl_info info;

                using std::cerr;
	  cerr << "Exception: " << getName() << "\n";

        cerr << "stack trace size is " << depth << "\n";

	  for ( int i = start ; i < depth ; ++i ) {
	        cerr << "pos="<<i<<"\n";
		  //pose problème lorsqu'il ne s'agit pas d'une vrai exception
		  //TODO trouver un kludge...
		  // if ( i < level ) continue;
		  /*
				si inline est activé, level ne correspond pas à la profondeur de la pile utilisée par les exceptions
#ifdef NO_INLINE
		if ( i < level ) continue;
#endif
			*/
                        char cmd1[1024];
        			if ( dladdr(stackTrace[i],&info) ) {
				snprintf(cmd1,1024,"addr2line -e %s -fC %p",info.dli_fname,
				(void*)((long long int)stackTrace[i] - (long long int)info.dli_fbase));
			}
			else {
				bzero(&info,sizeof(Dl_info));
				snprintf(cmd1,1024,"dladdr failed");
                        }
			//String sname;
			const char *sname;
			bool end = false;
			try {
			        if ( info.dli_sname ) {
			                if  ( dem )
        				        sname=strdupa(demangle(info.dli_sname).c_str());
        				else
        				        sname=info.dli_sname;
        			}
        			else
        			        sname="???";
			} catch (...) {
				sname=info.dli_sname;
			}


//			if ( sname == "raii_launch" ) end=true;
                        if ( strcmp("raii_launch",sname) == 0 ) end=true;

                        cerr << "\tat " << sname << "\n";
	    		cerr << "\t\tbinary: " <<  ( info.dli_fname ? info.dli_fname : "(null)" ) << " @ [" << info.dli_fbase << "]\n"; 
                        cerr << "\t\tsrc : " << cmd1 << "\n";

			if ( end ) break;
    	}
  }
  
  void Throwable::setFaultPosition(void*pos) {
    stackTrace[1]=pos;
  }

  String Throwable::demangle(const String& sym)
  {
    char *demangled=NULL;
    int status;
    demangled=abi::__cxa_demangle(sym.c_str(),demangled,0,&status);
    String ret;

    switch (status)
      {
      case 0:
        if ( !demangled )
		return sym;
	ret=demangled;
	free(demangled);
	return ret;
      default:
	return sym;
      }
  }

  String Throwable::getName() const
  {
    //return demangle(typeid(self_).name());
    std::type_info *te=abi::__cxa_current_exception_type();
    if ( !te )
      {
	return demangle(typeid(this).name());
      }
    else
      {
	const char *name=te->name();
	return demangle(name);
      }
  }
  String Throwable::toString() const
  {
    //String ret=demangle(typeid(self_).name());
    std::type_info *te=abi::__cxa_current_exception_type();
    if ( !te )
    	return demangle(typeid(this).name());
    const char *name=te->name();
    if ( !name )
    	return demangle(typeid(this).name());
    return demangle(name);
  }

  void Throwable::fillInStackTrace()
  {
    /*
    void *p[1024];
    int depth=backtrace(p,1024);
    int i;
    //0 is Throwable::fillInStackTrace
    for ( i=1 ; i < depth ; i++ ) {
      //stackTrace_[i]=p[i];
      stackTrace_.push_back(p[i]);
    }
    */
    depth=backtrace(stackTrace,1024);
    start=1;
  }


  struct debug_t {
    char *label;
    char *file;
    char *lineno_s;
    int lineno;
  } ;
    
  struct debug_t *find_source(const char *sofile, void* fbase, void* p)
  {
    if ( !sofile || !strlen(sofile) )
      return NULL;
    struct debug_t *dbg;
    FILE *f;
    char func[1024],file[1024];
    char cmd[1024];
    char *s;
    
    snprintf(cmd,1024,"addr2line -e %s -fC %p",sofile,/*fbase+*/p);
    
    if ( !(f=popen(cmd,"r") ) )
      return NULL;
    fscanf(f,"%s %s",func,file);
    //FIXME: valgrind dit que cette allocation n'est pas libérée et je le crois
    //semble fixé
    dbg=(struct debug_t*)calloc(1,sizeof(struct debug_t));
    dbg->label=strdup(func);
    dbg->file=strdup(file);
    s=strchr(dbg->file,':');
    if ( s )
      {
	*s='\0';
	dbg->lineno_s=s+1;
	dbg->lineno=atoi(dbg->lineno_s);
      }
    
    if ( pclose(f) || ! strcmp(dbg->label,"??") )
      {
	snprintf(cmd,1024,"addr2line -e %s -fC %p",sofile,
                 (void*)((long long int)p-(long long int)fbase));
	if ( !(f=popen(cmd,"r") ) )
	  {
	    free(dbg->label);
	    free(dbg->file);
	    free(dbg);
	    return NULL;
	  }
	fgets(func,1024,f);
	fgets(file,1024,f);
	func[strlen(func)-1]='\0';
	file[strlen(file)-1]='\0';
	free(dbg->label);
	dbg->label=strdup(func);
	free(dbg->file);
	dbg->file=strdup(file);
	s=strchr(dbg->file,':');
	if ( s )
	  {
	    *s='\0';
	    dbg->lineno_s=s+1;
	    dbg->lineno=atoi(dbg->lineno_s);
	  }
	if ( pclose(f) )
	  {
	    free(dbg->label);
	    free(dbg->file);
	    free(dbg);
	    return NULL;
	  }
      }
    return dbg;
  };
  

  String Throwable::getStackTrace(bool prettyPrint) const {
	  Dl_info info;
	  String st="";

	  st+="Exception: ";

	  st+= getName() + "\n";

	  if ( prettyPrint )
		  st+="<br>";

	  for ( int i = start ; i < depth ; ++i ) {
		  //pose problème lorsqu'il ne s'agit pas d'une vrai exception
		  //TODO trouver un kludge...
		  // if ( i < level ) continue;
		  /*
				si inline est activé, level ne correspond pas à la profondeur de la pile utilisée par les exceptions
#ifdef NO_INLINE
		if ( i < level ) continue;
#endif
			*/

			struct debug_t *dbg=NULL;
			if ( dladdr(stackTrace[i],&info) ) {
				dbg=find_source(info.dli_fname,info.dli_fbase,stackTrace[i]);
			}
			else
				bzero(&info,sizeof(Dl_info));
	  
			if ( !dbg ) {
				dbg=(struct debug_t*)calloc(1,sizeof(struct debug_t));
				dbg->label=NULL;
				dbg->file=NULL;
				dbg->lineno=0;
				dbg->lineno_s=NULL;
			}
	    //\tat func(file:line)\n

			String sname;
			bool end = false;
			try {
				sname=String(demangle(info.dli_sname));
			} catch (...) {
			        const char* dli_sname = info.dli_sname;
			        if (dli_sname)
			                sname=dli_sname;
			}

			if ( prettyPrint ) {
				String tmp;
				for ( String::size_type pos=0 ; pos < sname.size() ; pos++ )
					if ( sname[pos] == '<' )
						tmp+="&lt;";
					else
						tmp+=sname[pos];
				sname=tmp;
			}

			if ( sname == "raii_launch" ) end=true;

			//segfault grave dans un sighandler... cf Regex.C
			//sname=sname.replace("Servlet_[0-9A-F]..............[0-9A-F]_","csp::");

			st+=String("\tat ");

			if ( prettyPrint )
				st+="<strong>";

			st+=String( ((!sname.empty())?sname:String(dbg->label?dbg->label:"gni") ) );
	    
			if ( prettyPrint )
				st+="</strong><br>";

			st+= String("\n");

			if ( prettyPrint )
				st+="&nbsp;&nbsp;&nbsp;\n";

			if ( dbg ) {
				if ( dbg->file && String(dbg->file) != String("??") ) {
					if ( prettyPrint ) {
						st+="<a href=\"gedit://"+String(dbg->file).escapeAttribute()+"&amp;line="+dbg->lineno_s+"\">";
					}
					st+= String("\t\tsource: ") + dbg->file;
					if ( prettyPrint ) {
						st+="</a>";
					}
				}
				else
					st+= String("\t\tbinary: ") + ( info.dli_fname ? info.dli_fname : "(null)" );

				if ( dbg->lineno)
					st+= String(":") + dbg->lineno_s + "\n";
				else
					st+= String("\n");
			
			        if ( dbg->file ) free(dbg->file);
			        if ( dbg->label) free(dbg->label);
			}
			else {
				st+="\t\tdebug structure empty!\n";
			}

			if ( prettyPrint )
				st+="<br>";
      
      
			if ( end ) break;
    	}
    
    	return st;
  };


}
}
namespace std {
  // Helper for exception objects in <except>
  void __throw_bad_exception() {
        throw raii::error::VeryBadException("from libstdc++");
  }

  // Helper for exception objects in <new>
  void __throw_bad_alloc() {
        throw raii::error::BadAllocException("from libstdc++");
  }
  
  // Helper for exception objects in <typeinfo>
  void __throw_bad_cast() {
        throw raii::error::BadCastException("from libstdc++");
  }
  
  void __throw_bad_typeid() {
        throw raii::error::BadTypeIdException("from libstdc++");
  }

  // Helpers for exception objects in <stdexcept>
  void __throw_logic_error(const char *str) {
        throw raii::error::LogicException(str);
  }

  void __throw_domain_error(const char *str) {
        throw raii::error::DomainException(str);
  }

  void __throw_invalid_argument(const char *str) {
        throw raii::error::IllegalArgumentException(str);
  }

  void __throw_length_error(const char *str) {
        throw raii::error::LengthException(str);
  }

  void __throw_out_of_range(const char *str) {
        throw raii::error::OutOfRangeException(str);
  }

  void __throw_runtime_error(const char *str) {
        throw raii::error::RuntimeException(str);
  }

  void __throw_range_error(const char* str)  {
        throw raii::error::RangeException(str);
  } 

  void __throw_overflow_error(const char *str)  {
        throw raii::error::OverflowException(str);
  }

  void __throw_underflow_error(const char *str) {
        throw raii::error::UnderflowException(str);
  }

  // Helpers for exception objects in basic_ios
  void __throw_ios_failure(const char *str)  {
        throw raii::error::IOSFailureException(str);
  }

}

void dumpst(FILE *f) {

	  Dl_info info;

        void *stackTrace[1024];
        int depth=0, start=1;
        depth=backtrace(stackTrace,1024);
       fprintf(f,"stack trace size is %d\n",depth);
       bool dem=false;

	  for ( int i = start ; i < depth ; ++i ) {
	        fprintf(f,"pos=%d\n",i);
		  //pose problème lorsqu'il ne s'agit pas d'une vrai exception
		  //TODO trouver un kludge...
		  // if ( i < level ) continue;
		  /*
				si inline est activé, level ne correspond pas à la profondeur de la pile utilisée par les exceptions
#ifdef NO_INLINE
		if ( i < level ) continue;
#endif
			*/
                        char cmd1[1024];
        			if ( dladdr(stackTrace[i],&info) ) {
				snprintf(cmd1,1024,"addr2line -e %s -fC %p",info.dli_fname,
				(void*)((long long int)stackTrace[i] - (long long int)info.dli_fbase));
			}
			else {
				bzero(&info,sizeof(Dl_info));
				snprintf(cmd1,1024,"dladdr failed");
                        }
			//String sname;
			const char *sname;
			bool end = false;
			try {
			        if ( info.dli_sname ) {
			                if  ( dem )
        				        sname=strdupa(raii::Throwable::demangle(info.dli_sname).c_str());
        				else
        				        sname=info.dli_sname;
        			}
        			else
        			        sname="???";
			} catch (...) {
				sname=info.dli_sname;
			}


//			if ( sname == "raii_launch" ) end=true;
                        if ( strcmp("raii_launch",sname) == 0 ) end=true;

                        fprintf(f,"\tat %s\n",sname);
	    		fprintf(f,"\t\tbinary: %s @ [%p]\n",( info.dli_fname ? info.dli_fname : "(null)" ),info.dli_fbase); 
                        fprintf(f,"\t\tsrc : %s\n",cmd1);

			if ( end ) break;
    	}

}






