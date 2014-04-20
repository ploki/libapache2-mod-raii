#ifndef __DEFAULT_TAGS_H__
#define __DEFAULT_TAGS_H__


namespace raii {

class HttpServletRequest;
class HttpServletResponse;

class include {
        String page;
        public:
        void setPage(const String& str);
        void doStart(HttpServletRequest& request, HttpServletResponse& response);
};

class forward {
	String page;
	public:
	void setPage(const String& str);
	void doStart(HttpServletRequest& request, HttpServletResponse& response);
};

class page {
	String type;
	String charset;
	public:
	void setType(const String& str);
	void setCharset(const String& str);
	void doStart(HttpServletRequest& request, HttpServletResponse& response);
};


}

#endif /* __DEFAULT_TAGS_H__ */
