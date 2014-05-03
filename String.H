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
 * ARE DISCLAIMED. IN NO EVENT SHALL G.GIMENEZ BE LIABLE FOR ANY DIRECT,
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
#include <string>
#include "Object.H"

namespace raii {
namespace error {
class NullPointerException;
}

class String;

String url_decode(const String& str);
String url_encode(const String& str);
String escapeHTML(const String& str);
String escapeAttribute(const String& str);

template<class T> class Vector;
class Regex;

/*! The String class of mod_raii
 * \sa inline String ftostring(double d)
 * \sa inline String itostring(int i)
 * \sa inline String lltostring(long long int lli )
 * */
class String : public virtual Object, public std::string {

        bool null;

        /*! check for Null pointer
         * \param s the string pointer to test
         * \return s if not null or a pointer to an empty string
         */
        static const char* checkPtr(const char *s) {
	        if ( !s ) {
	                return "";
	                //throw error::NullPointerException("string is null");
	        }
	        return s;
	}
	public:

    /*! Default constructor */
	String() : Object(), std::string(), null(false) {};

    /*! Copy constructor
     * \param s the string to copy
     * \param pos from that position
     * \param n number of characters to copy
     */
	String(const String& s, size_type pos = 0, size_type n = std::string::npos)
		: Object(), std::string(s,pos,n), null(s.isNull()) {}


	/*! Copy from iterator constructor
	 * \param first iterator pointing to first element to copy from a String
	 * \param last element (excluded) to be copied from the same String
	 */
        template <class InputIterator>
        String(InputIterator first, InputIterator last)
                : Object(), std::string(first,last), null(false) {}

    /*! Construct a raii::String from an std::string */
	String(const std::string& str) : Object(), std::string(str), null(false) {}

	/*! Construct a raii::String from a const char*
	 * \param s the string to copy which should be NULL
	 */
	String(const char* s) : Object(), std::string(checkPtr(s)), null(!s) {};

	/*! Construct a raii::String from a const char* limiting size to n characters
	 * \param s the string to copy which should be NULL
	 */
        String(const char* s, std::string::size_type n) : Object(), std::string(checkPtr(s),n), null(!s) {}

        /*! construct a String made of n characters c
         * \param n the number of characters
         * \param c the character to put in the String
         */
        String(std::string::size_type n, char c) : Object(), std::string(n,c), null(false) {}

        /*! Assigning to this String another String
         * \param str the string to copy
         */
        String& operator=(const String& str) {
                null=str.null;
                assign(str);
                return *this;
        }

        /*! Assign to this String a string literal or a C string
         * \param s is the string and may be NULL
         */
        String& operator=(const char* s) {
                null= !s;
                assign(checkPtr(s));
                return *this;
        }
        /*! Replace content of this string by a character */
        String& operator=(char c) {
                null=false;
                clear();
                push_back(c);
                return *this;
        }
        
        /*! swap this string content with another string */
        void swap(String& str) {
                String tmp(*this);
                null=str.null;
                assign(str);
                str.null=tmp.null;
                str.assign(tmp);
        }

    /*! return substring of n characters starting at pos */
	String substr(size_type pos = 0, size_type n = npos ) const {
		return std::string::substr(pos,n);
	}

	/*! return a pointer to datum contened in this string*/
        const char * c_str() const {
                if ( null ) return NULL;
                return this->std::string::c_str();
        }

    /*! convert this string to an int */
	int toi() const {
		return atoi(this->std::string::c_str());
	}

	/*! convert this string to a long note : sizeof long == architecture word size
	 * use this function for size_t, time_t, ino_t, off_t etc... 
	*/
	long tol() const {
		return atol(this->std::string::c_str());
	}

	/*! convert this string to a long long */
	long long toll() const {
		return atoll(this->std::string::c_str());
	}

	/*! convert this string to a double */
	double tof() const {
		return atof(this->std::string::c_str());
	}

	/*! convert this string to a double */
	double tod() const {
		return tof();
	}

	/*! return true if this string is null
	 * \note a String is null if and only if null is set to true and not empty
	 */
        bool isNull() const {
                if ( ! this->empty() )
                        return false;
                else
                        return null;
        }

       /*! set String to null */
        void setNull() {
                this->clear();
                null=true;
        }

    /*! return the url_decoded value of this string */
	String url_decode() const {
		return raii::url_decode(*this);
	}
	/*! return the url_encoded value of this string */
	String url_encode() const {
		return raii::url_encode(*this);
	}

	/*! return this string with all characters having a meaning in html escaped */
        String escapeHTML() const {
		return raii::escapeHTML(*this);
	}

        /*! return this string with all characters having a meaning in attributes escaped */
        String escapeAttribute() const {
		return raii::escapeAttribute(*this);
	}

        /*! explode this string with separator sep
         * \param sep is the separator to use and may content multiples characters
         */
	Vector<String>	explode(const String& sep) const;

	/*! split this string using a regular expression as separator
	 * \param regex the regular expression to use as separator
	 * \param notbol
	 * \param noteol
	 */
	Vector<String>	split(const Regex& regex, bool notbol=false, bool noteol=false) const;

	/*! return substring matching regex
	 * \param regex
	 * \param notbol
	 * \param noteol
	 */
	Vector<String>	substrs(const Regex& regex, bool notbol=false, bool noteol=false) const;

	/*! return true if regex matches this string */
	bool		matches(const Regex& regex) const;

	/*! return a string with part maching regex replaced by replace */
	String		replace(const Regex& regex, const String& replace) const;


	/*! Assign to this string content of vec assembled with glue */
	String& implode(const Vector<String>& vec, const String& glue);

	/*! remove heading white spaces of this string
	 * \return a reference to this
	 */
	String& ltrim();

	/*! remove trailing white spaces of this string
	 * \return a reference to this
	 */
	String& rtrim();

	/*! remove heading and trailing white spaces of this string
	 * \return a reference to this
	 */
	String& trim();

	virtual String dump(const String& str) const {
		return str+": "+*this;
	}
};

	/*! literal string concatenation
	 * Permit constructs like
	 * \code
	 * String str = "a="+request.getParameter("a");
	 * \endcode
	 */
	static inline String operator+(const char *s1, const String& s2) {
		return String(s1)+s2;
	}
	/*! literal string equals
	 * Permit constructs like
	 * \code
	 * String str="a";
	 * if ( "a" = str ) throw Plop("gr8k");
	 * \endcode
	 */
	static inline bool operator==(const char *s1, const String& s2) {
		return s2==s1;
	}


}
