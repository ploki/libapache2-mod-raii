#include <list>
#include "Object.H"

namespace raii {
	template<typename T>
	class List : public virtual Object, public std::list<T> {
		public:
		List() : Object(), std::list<T>() {}
		List(size_t n)  : Object(), std::list<T>(n) {}
		List(size_t n, const T& t) : Object(), std::list<T>(n,t) {}


		template<class InputIterator>
		List(InputIterator f, InputIterator l) : Object(), std::list<T>(f,l) {}

		virtual String dump(const String& str) const {

			return String("List(")+str+")";
		}

		List<T>& append(const List<T>& l) {
			for ( typename List<T>::iterator it = l.begin() ; it != l.end() ; ++it )
				this->push_back(*it);
			return *this;
		}


		List<T>& prepend(const List<T>& l) {
			for ( typename List<T>::iterator it = l.begin() ; it != l.end() ; ++it )
				this->push_front(*it);
			return *this;
		}

	};
}
