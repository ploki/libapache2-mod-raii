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
