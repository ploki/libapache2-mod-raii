#include "raii.H"

#include <FlatProject/all.H>

using namespace raii;
using namespace raii::sql;

namespace htdocs {
class SERVLET(index) : public HttpServlet {

        void service( HttpServletRequest& request, HttpServletResponse& response) {

                String action=request.getParameter("action");

                if ( action.empty() || action == "index" ) {
                        request.getRequestDispatcher("/view/index.csp").forward(request,response);
                }
                else if ( action == "plop" ) {
                        //do something
                }
                else {
                        throw ServletException("Unknown action");
                }
        }

};

exportServlet(index);
}
