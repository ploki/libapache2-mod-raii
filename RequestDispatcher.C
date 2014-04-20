#include "raii.H"

namespace raii {

	RequestDispatcher::RequestDispatcher(const String& _contextPath, const String& _uri)

		: Object(), contextPath(_contextPath), uri(_uri) {}


	void RequestDispatcher::forward(HttpServletRequest& request, HttpServletResponse& response) {

		service(request,response);
		throw RequestOK("RequestDispatcher::forward");
	}


	void RequestDispatcher::include(HttpServletRequest& request, HttpServletResponse& response) {

		try {
			service(request,response);
		}
		catch (RequestOK& e) {
			//nop! si la page chargée forward sur une autre page, il ne faut pas propager l'exception
			if ( e.getMessage() != "RequestDispatcher::forward" )
				throw;
		}
	}

        String RequestDispatcher::dump(const String& str) const {

                return String("RequestDispatcher(")+str+")="+contextPath+uri;
        }

	void RequestDispatcher::service(HttpServletRequest& request,HttpServletResponse& response) {

		//request_rec *oldApacheRequest=apacheRequest;

		ptr<ServletContext> ctx=NULL;
		{
			Lock l1(servletContextMutex);
			ctx=(*servletContext)[contextPath];
		}

		String context_n_uri=contextPath+uri;
		ptr<HttpServlet> sv=NULL;

		{
			//changement de context
			ApacheSubRequest subreq(context_n_uri);
			sv=ctx -> getServlet();
		}

		if ( ! sv )
			throw UnavailableException("Pas de servlet");

		try {
			sv->preService(request, response);
			sv->service (request, response);
		}
		catch (...) {
			sv->postService(request, response,true);
			throw;
		}
		sv->postService(request, response,false);
	}



}
