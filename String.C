#include "raii.H"

namespace raii {

	Vector<String> String::explode(const String& sep) const {

                Vector<String> v;
               	size_t lpos,pos=0;
               	do {
               	        lpos=pos;
               	        pos = this->find(sep,pos) ;
               	        v.push_back(this->substr(lpos,pos-lpos));
				if ( pos == String::npos )
				break;
			pos+=sep.size();
                	} while ( 1 );
		return v;
       	}

	Vector<String> String::split(const Regex& regex, bool notbol, bool noteol) const {
		return regex.split(*this,notbol,noteol);
	}

	Vector<String> String::substrs(const Regex& regex, bool notbol, bool noteol) const {
		return regex.substrs(*this,notbol,noteol);
	}

	bool String::matches(const Regex& regex) const {
		return regex.matches(*this);
	}

	String String::replace(const Regex& regex, const String& replace) const {
		return regex.replace(replace,*this);
	}

	String& String::implode(const Vector<String>& vec, const String& glue) {
		clear();
		for ( size_t i = 0 ; i < vec.size() ; ) {
			append(vec[i]);
			if ( ++i < vec.size() )
				append(glue);
		}
		return *this;
	}

	String& String::ltrim() {
		size_t i;
		for(i=0; (*this)[i] == ' ' && i < this->size() ; ++i );
		return *this=this->substr(i,String::npos);
	}
	String& String::rtrim() {
		int i;
		for ( i=this->size()-1 ; (*this)[i] == ' ' && i >= 0 ; --i );
		return *this=this->substr(0,i+1);
	}
	String& String::trim() {
		ltrim();
		rtrim();
		return *this;
	}
}
