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
