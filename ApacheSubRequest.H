#include "Object.H"
#include "Exception.H"



namespace raii {

  class ApacheSubRequest : public Object {
  private:
    request_rec *rr;
    request_rec *oldApacheRequest;
    ap_filter_t *f;
  public:
    ApacheSubRequest(const String& uripath,
		     ap_filter_t *_f=NULL,
         request_rec *r=NULL);

    ~ApacheSubRequest();
    int run() const;

    request_rec *getApacheRequest() const;

    operator request_rec*() const;

    virtual String dump(const String& str) const;
  };
}
