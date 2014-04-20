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
