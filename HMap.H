#include <unordered_map>
#include "Object.H"

namespace raii {

	template<typename Key, typename Data>
	class HMap : public virtual Object, public std::unordered_map<Key,Data> {

		public:
		HMap() : Object(), std::unordered_map<Key,Data>() {}
		HMap(const HMap& m) : Object(), std::unordered_map<Key,Data>(m) {}
		HMap(const std::unordered_map<Key,Data>& m) : Object(), std::unordered_map<Key,Data>(m) {}



		template<class InputIterator>
		HMap(InputIterator f, InputIterator l) : Object(), std::unordered_map<Key,Data>(f,l) {}

		virtual String dump(const String& str) const {

			return String("HMap(")+ str + ")";
		}

	};
}
