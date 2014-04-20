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
#include <vector>
#include "Object.H"

namespace raii {
template<typename T>
class Vector : public virtual Object, public std::vector<T> {
        public:
        Vector() : Object(), std::vector<T>() {}
        Vector(size_t n) : Object(), std::vector<T>(n) {}
        Vector(size_t n, const T& t) : Object(), std::vector<T>(n,t) {}
	Vector(const Vector& v) : Object(), std::vector<T>(v) {}

        template <class InputIterator>
        Vector(InputIterator first, InputIterator last)
                : Object(), std::vector<T>(first,last) {}

        //convert
        Vector(const std::vector<T>& v) : Object(), std::vector<T>(v) {}

        //duck typing : I'am a List :)
        void push_front(const T& elem) {
                std::vector<T>::insert(this->begin(),elem);
	}
	
        Vector<T>& append(const Vector<T>& vec) {
	        size_t s = vec.size();
	        for ( size_t i = 0 ; i < s ; ++i ) this->push_back(vec[i]);
	        return *this;
	}

/*
	Vector<T>& prepend(const Vector<T>& vec) {
	        size_t s = vec.size();
	        for ( int i = 0 ; i < s ; ++i ) this->push_front(vec[i]);
	        return *this;
	}
*/
        Vector<T>& prepend(/*copy*/ Vector<T> vec) {
	        size_t s = this->size();
                for ( size_t i = 0 ; i < s ; ++i ) vec.push_back((*this)[i]);
                this->clear();
                this->append(vec);
                return *this;
	}

        typename Vector<T>::iterator eraseAt(int i) {
	        return this->erase(this->begin()+i);
	}
	
	
	typename Vector<T>::iterator find(const T& val) {
        	typename Vector<T>::iterator it = this->begin();
	        for ( ; it != this->end() ; ++it )
	                if ( *it == val )
	                        break;
	        return it;
	}
};
}
