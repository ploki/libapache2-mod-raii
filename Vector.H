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
