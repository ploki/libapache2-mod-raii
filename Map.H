#include <map>
#include "Object.H"

namespace raii {

	template<typename Key, typename Data>
	class Map : public virtual Object, public std::map<Key,Data> {

		public:
		Map() : Object(), std::map<Key,Data>() {}
		Map(const Map& m) : Object(), std::map<Key,Data>(m) {}
		Map(const std::map<Key,Data>& m) : Object(), std::map<Key,Data>(m) {}



		template<class InputIterator>
		Map(InputIterator f, InputIterator l) : Object(), std::map<Key,Data>(f,l) {}

		virtual String dump(const String& str) const {

			return String("Map(")+ str + ")";
		}

	};
}
