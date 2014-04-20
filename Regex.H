#ifndef __RAII_REGEX_H_
#define __RAII_REGEX_H_

#include <regex.h>

namespace raii {

class Regex : public Object {

	DECLARE_EXCEPTION(RegularExpressionException,LogicException);

	typedef enum {
		NO,
		MATCH,
		REPLACE
	} Compiled;

	String pattern;
	mutable regex_t re;
	bool m_extended;
	bool m_caseInsensitive;
	bool m_newLine;
	mutable Compiled compiledFor;

	void compile(Compiled compileFor) const;
	public:

	Regex(const String& pat,bool ext=false, bool icase=false, bool newline=false);

	//pour les chaines litterales
	Regex(const char* pat,bool ext=false, bool icase=false, bool newline=false);
	
	Regex(const Regex& r);
	Regex& operator=(const Regex& r);

	Regex& extended(bool e=true);
	Regex& caseInsensitive(bool i=true);
	Regex& newLine(bool nl=true);



	// ex: Regex("plop").matches(str);
	bool matches(const String& str, bool notbol=false, bool noteol=false) const;

	
	// ex: Regex("plop").replace("gr8k",str);
	String replace(const String& repl, const String& str) const;

	Vector<String> split(const String& str, bool notbol=false, bool noteol=false) const;
	Vector<String> substrs(const String& str, bool notbol=false, bool noteol=false) const;

	virtual ~Regex();
	
};

}

#endif
