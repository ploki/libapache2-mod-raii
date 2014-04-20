#include "raii.H"

namespace raii {

	/*
		class include
	*/

        void include::setPage(const String& str) {

		page=str;
	}


        void include::doStart(HttpServletRequest& request, HttpServletResponse& response) {

		RequestDispatcher rd=request.getRequestDispatcher(page);
		rd.include(request,response);
        }


	/*
		class forward
	*/

	void forward::setPage(const String& str) { page=str; }
	void forward::doStart(HttpServletRequest& request, HttpServletResponse& response) {
	
		RequestDispatcher rd=request.getRequestDispatcher(page);
		rd.forward(request,response);
	}

	/*
		class page
	*/

	void page::setType(const String& str) {

		type=str;
	}


	void page::setCharset(const String& str) {

		charset=str;
	}


void page::doStart(HttpServletRequest& request, HttpServletResponse& response) {
		if ( ! type.empty() ) response.setContentType(type);
		if ( ! charset.empty() ) response.setCharacterEncoding(charset);
	}
}

