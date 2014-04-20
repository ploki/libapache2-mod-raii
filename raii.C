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
#include <iostream>
#include <signal.h>
#include <exception>


namespace raii {




//__thread request_rec *apacheRequest=NULL;


// pas d'init car tls_ptr<> initialise à NULL et __thread aussi
tls_ptr(request_rec) apacheRequest;
tls_ptr(SegfaultBuffer) segfaultBuffer;

apr_status_t raii_launch(request_rec *r)
{
  int requestError=OK;
  bool performReset=false;
  
  if ( !r )
    return DECLINED;

  String handler(r->handler);

  if ( handler != "raii_module" )
    return DECLINED;

  if ( r->finfo.filetype == 0 )
    return HTTP_NOT_FOUND;
  r->content_type = "text/html";
  r->no_cache=1;
  r->no_local_copy=1;

  SegfaultBuffer *oldSegfaultBuffer=segfaultBuffer;
  SegfaultBuffer *newSegfaultBuffer=new SegfaultBuffer;
  segfaultBuffer=NULL;

  request_rec *oldApacheRequest=apacheRequest;
  apacheRequest=r;

  

  
  HttpServletResponse response;


  try {

    ServletConfig servletConfig;

    ptr<ServletContext> servletContext=servletConfig.getServletContext();

    ptr<HttpServlet> httpServlet=servletContext->getServlet();

    if ( !httpServlet )
      throw UnavailableException("Pas de servlet");


    int sig;
    segfaultBuffer=newSegfaultBuffer;
    sig=setjmp(segfaultBuffer->env);

    ptr<HttpServletRequest> request = NULL;
    
    if ( sig ) {
      fprintf(stderr,"raii.C: LONGJUMPED TO LANDING PAD\n");fflush(stderr);
      //delete request;
    }
/*
    switch (sig)
    {
      case 0:
*/      if ( sig == 0 )
  	{
		request = new HttpServletRequest;


		{
			ptr<HttpSession> s = request->getSession(false);
			//pour ajouter l'utilisateur dans les logs d'apache...
			if ( s && s->isValid() ) {
				String user=s->getUser();
				if ( ! user.empty() && ! apacheRequest->user ) {
					apacheRequest->user=apr_pstrdup(apacheRequest->pool,user.c_str());
				}
				s->touch();
			}
		}

		try {
			httpServlet->preService(*request,response);
			httpServlet->service (*request,response);
		} catch (...) {
			httpServlet->postService(*request,response,true);
			throw;
		}
		httpServlet->postService(*request,response,false);

		//s'il n'est pas déjà initialisé
		if ( ! apacheRequest->user ) {
			ptr<HttpSession> s = request->getSession(false);
			if ( s && s->isValid() ) {
				String user=s->getUser();
				if ( ! user.empty() )
					apacheRequest->user=apr_pstrdup(apacheRequest->pool,user.c_str());
			}
		}
		
                //delete request;

	}
        else {
                Exception * e = segfaultBuffer->exception;
                if ( 0 ) {//déjà dumpé
                Logger log("raii");
                log.error(e->toString());
                std::cerr << e->getMessage() << "\n";
                e->poorManPrintStackTrace();
             }
                response.setStatus(HTTP_INTERNAL_SERVER_ERROR);
                response << "</script></select><h1>" << e->toString() << "</h1>"
                << "<hr/>"
                << "<pre>\n"
                << e->getMessage()
                << "</pre>"
                << "<hr/>"
                << e->getStackTrace(true);
              //<< "see logs\n";
              response.flushBuffer();
            delete e;
        }
/*
        break;
      case SIGILL:
        throw static_cast<IllegalInstruction*>(segfaultBuffer->exception);
      case SIGFPE:
        throw static_cast<FloatingPointException*>(segfaultBuffer->exception);
      case SIGSEGV:
        throw static_cast<SegmentationFault*>(segfaultBuffer->exception);
      case SIGBUS:
        throw static_cast<BusError*>(segfaultBuffer->exception);
      case SIGABRT:
          throw static_cast<Abort*>(segfaultBuffer->exception);
      default:
        throw (segfaultBuffer->exception);
    }
*/
	segfaultBuffer=NULL;
  } catch ( RequestOK & ) {
    requestError=OK;
  
  } catch ( RequestDECLINED & ) {
    requestError=DECLINED;
    
  } catch ( RequestDONE & ) {
    requestError=DONE;
    
  } catch ( RequestERROR & ) {
    requestError=r->status;
  
  } catch ( ResponseException & ) {

    //rien à faire
  
  } catch ( ResetRaii & ) {
    performReset=true;
    Logger log("raii");
    log("Reset requested, delaying...");

  } catch ( BuildException &e ) {

	Logger log("raii");
	log.error();
	log("Build failed");
    response.setStatus(HTTP_INTERNAL_SERVER_ERROR);
    response.setContentType("text/html");
    response << "</script></select><div style=\"z-index: 666; background: white; position: fixed; top: 0px; left: 0px; right: 0px; bottom: 0px; padding: 50px; overflow: auto; opacity: 0.95;\"><h1>" << e.toString() << "</h1>"
	<< "<hr/>"
	<< "<div style=\"text-align: left; font-size: 12px; font-family: Monospace, Mono, Andale Mono;\">";

	String buf = e.getMessage().escapeHTML();
	String text;
	for ( size_t i = 0 ; i < buf.size() ; ++i ) {
		if ( i !=  buf.size()-1 ) {
			if ( buf[i] == ':' && buf[i+1] == ':' ) {
				text+="&#58;&#58;";
				i++;
			}
			else
				text+=buf[i];
		}
		else {
			text+=buf[i];
			
		}
	}

	Vector<String> phrase = text.explode("\n");
	for ( size_t l=0 ; l < phrase.size() ; ++l ) {

		Vector<String> token = phrase[l].explode(":");
		size_t size=token.size();

		if ( size == 1 ) {
			response << "<strong>"<<token[0]<<"</strong><br/>";
			continue;
		}

		//response << "<span style=\"color: black;\">"<<token[0]<<"</span>";
		
		int lineno = token[1].toi();
		size_t pos = token[0].find('/',0);
		String  head= token[0].substr(0,pos);
		String  file;
		if ( pos != String::npos )
        		file = token[0].substr(pos);

                if ( token[0][0] == ' ' ) response << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";

                if ( file.empty() ) {
        		response << "<span style=\"color: black;\">"<<token[0]<<"</span>";
                }
                else {
              		response << "<span style=\"color: black;\">"<<head<<"<a href=\"gedit://"
              		         << file.escapeAttribute()<<"&amp;line="<<lineno<<"\">"<<file<<"</a></span>";
              	}
		
		if ( size >= 2 ) {
			response << ":<span style=\"color: green;\">"<<token[1]<<"</span>";
		}

		String col="#000000";

		if ( size >= 3 ) {
			if ( token[2] == " error" ) {
				response << ": <span style=\"color: red;\">error</span>";
				col="#aa0000";
			}
			else if ( token[2] == " warning" ) {
				response << ": <span style=\"color: blue;\">warning</span>";
				col="#ff00ff";
			}
			else if ( token[2] == " note" ) {
				response << ": <span style=\"color: seagreen;\">note</span>";
				col="seagreen";
			}
			else {
				response << ":<strong>" << token[2] << "</strong>";
				col="#770000";
			}
		}

		for ( size_t t=3 ; t < size ; ++t ) {

			response << ":<span style=\"color: "<<col<<";\">" << token[t] << "</span>";
		}
		response << "<br/>";
	}

//        << e.getMessage().escapeHTML()
	response << "</div></div>";
  

  } catch ( Throwable *e ) {
    
    response.setContentType("text/html");
    if ( !e ) {
      response << "received an illegal exception";
    }
    else {
    
    Logger log("raii");
    log.error(e->toString());
    std::cerr << e->getMessage() << "\n";
    e->poorManPrintStackTrace();
     
    response.setStatus(HTTP_INTERNAL_SERVER_ERROR);
    response << "</script></select><h1>" << e->toString() << "</h1>"
        << "<hr/>"
        << "<pre>\n"
        << e->getMessage()
        << "</pre>"
        << "<hr/>"
      /*  << e->getStackTrace(true); */
      << "see logs\n";
      response.flushBuffer();
    delete e;
    }
  

  } catch ( SQLException &e ) {

    Logger log("raii::sql");
    log.error(e.toString());
    std::cerr << e.getMessage() << "\n";
    e.printStackTrace();

    response.setContentType("text/html");
    response.setStatus(HTTP_INTERNAL_SERVER_ERROR);
    
    response << "</script></select><h1>" << e.toString() << "</h1>"
        << "<hr/>"
        << "<pre>\n"
//        << "le détail de cette erreur se trouve dans les logs"
        << e.getMessage() << "\n"
        << "</pre>"
	<< "<hr/>"
	<< e.getStackTrace(true);

     
  } catch ( Throwable &e ) {
    
    Logger log("raii");
    log.error(e.toString());
    std::cerr << e.getMessage() << "\n";
    e.printStackTrace();

    response.setContentType("text/html");
    response.setStatus(HTTP_INTERNAL_SERVER_ERROR);
    
    response << "</script></select><h1>" << e.toString() << "</h1>"
        << "<hr/>"
        << "<pre>\n"
        << e.getMessage()
        << "</pre>"
        << "<hr/>"
        << e.getStackTrace(true);
  
  
  } catch ( std::exception &e ) {
    
    requestError=HTTP_INTERNAL_SERVER_ERROR;
    
    const char *demwhat=abi::__cxa_demangle(e.what(),NULL,NULL,NULL);

    Logger log("raii");
    log.error(String("Uncaught non raii exception : ") + e.what());
    if ( demwhat )
      log.error(String("demangled is : ")+demwhat);
  

  } catch ( char const* str ) {

    requestError=HTTP_INTERNAL_SERVER_ERROR;
        
    Logger log("raii");
    log.error(String("Uncaught char const* exception : ") + str);
  

  } catch ( ... ) {

    requestError=HTTP_INTERNAL_SERVER_ERROR;

    Logger log("raii");
    log.error(String("Uncaught unknown exception : ")
        +abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(),
                             NULL,NULL,NULL));
  }
  
  if ( newSegfaultBuffer ) delete newSegfaultBuffer;
  segfaultBuffer=oldSegfaultBuffer;

  if ( performReset ) {
    Logger log("raii");
    log("resetting sessions");
    {
      ServletConfig cfg;
      ptr<ServletContext> ctx=cfg.getServletContext();
      Map<String, ptr<HttpSession> >& sessionMap = ctx->getSessionMap();
      sessionMap.clear();
    }
    log("resetting pools");
    raii::sql::Connection::clearPools();
    raii::sql::SQLDriverContainer::clear();
    log("resetting contexts");
    servletContext->clear();
    log("reset performed");
  }

  apacheRequest=oldApacheRequest;


  if ( requestError >= 100 )
    r->status=HTTP_OK;
  return requestError;
}
}
