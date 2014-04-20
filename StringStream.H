#include <sstream>
#include "Object.H"

namespace raii {

class StringStream : public virtual Object, public std::stringstream {
	public:

       StringStream (const String& __str, std::ios_base::openmode __m=std::ios_base::out|std::ios_base::in)
		: Object(), std::stringstream(__str, __m) {}
       StringStream(std::ios_base::openmode __m=std::ios_base::out|std::ios_base::in)
		: Object(), std::stringstream(__m) {}

	virtual String dump(const String& str) const {
		return str+": "+this->str();
	}
	//convert
	//StringStream(const std::stringstream& str) : Object(), std::stringstream(str) {}
};
}
