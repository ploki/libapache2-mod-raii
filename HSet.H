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
#include <unordered_set>
#include "Object.H"

namespace raii {

	template<typename Key>
	class HSet : public virtual Object, public std::unordered_set<Key> {

		public:
		HSet() : Object(), std::unordered_set<Key>() {}
		HSet(const HSet& s) : Object(), std::unordered_set<Key>(s) {}
		HSet(const std::set<Key>& s) : Object(), std::unordered_set<Key>(s) {}

		template<class InputIterator>
		HSet(InputIterator f, InputIterator l) : Object(), std::unordered_set<Key>(f,l) {}

                HSet<Key>& operator<<(const Key& key) {
                        insert(key);
                        return *this;
                }

		virtual String dump(const String& str) const {

			return String("HSet(")+ str + ")";
		}

	};

}

namespace std {
template<>
        inline size_t hash<raii::String>::operator()(raii::String __val) const {
                //return hash<string>::hash()(static_cast<string&>(__val));
                size_t s = __val.size();
                if ( s == 0 ) return 0;
                return
#if 1
		  static_cast<unsigned char>(__val[0])     *  5
		  + static_cast<unsigned char>(__val[s-1])   * 13
		  + static_cast<unsigned char>(__val[(s>>1)])* 23
		  + s;
#else
		(((unsigned char)__val[0])<<24)
		  + (((unsigned char)__val[s-1])  <<16)
		  + (((unsigned char)__val[(s>>1)])<<8)
		  + (s % 256 );
#endif
        }

}
