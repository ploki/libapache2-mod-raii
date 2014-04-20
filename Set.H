#include <set>
#include "Object.H"

namespace raii {

	template<typename Key>
	class Set : public virtual Object, public std::set<Key> {

		public:
		Set() : Object(), std::set<Key>() {}
		Set(const Set& s) : Object(), std::set<Key>(s) {}
		Set(const std::set<Key>& s) : Object(), std::set<Key>(s) {}

		template<class InputIterator>
		Set(InputIterator f, InputIterator l) : Object(), std::set<Key>(f,l) {}

                Set<Key>& operator<<(const Key& key) {
                        std::set<Key>::insert(key);
                        return *this;
                }

		virtual String dump(const String& str) const {

			return String("Set(")+ str + ")";
		}

	};
}
