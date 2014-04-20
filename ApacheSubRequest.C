#include "raii.H"

namespace raii {
    ApacheSubRequest::ApacheSubRequest(const String& uripath,
                     ap_filter_t *_f,
                     request_rec *r) : Object(), rr(NULL), oldApacheRequest(apacheRequest), f(_f)  {
      rr=ap_sub_req_lookup_uri(uripath.c_str(),
                               r?r:apacheRequest,
                               f);
      if ( !rr )
        throw ApacheException("Unable to initialize sub request");
      apacheRequest=rr;
    }

    ApacheSubRequest::~ApacheSubRequest() {
      apacheRequest=oldApacheRequest;
      ap_destroy_sub_req(rr);
    }

    int ApacheSubRequest::run() const {
      return ap_run_sub_req(rr);
    };

    request_rec *ApacheSubRequest::getApacheRequest() const {
      return  rr;
    };

    ApacheSubRequest::operator request_rec*() const {
      return rr;
    }

    String ApacheSubRequest::dump(const String& str) const {
      return String("ApacheSubRequest(")+str+"): "+apr_uri_unparse(rr->pool,&rr->parsed_uri,0);
    }

}
