#ifndef __PTR_H__
#define __PTR_H__

#include <Exception.H>
#include <exception>
#include <pthread_iface.H>

#include <cstdlib>

#if 0
#define PTRDEBUG(msg) fprintf(stderr,"ptr[%p,%p]: %s\n",this,_M_ptr,msg); fflush(stderr);
#else
#define PTRDEBUG(msg)
#endif


#ifdef PTR_IDE_KLUDGE
#include <ptr_ide_kludge.H>
#else
namespace raii {

	class ptr_counter {
		public:
		Mutex mutex;
		int count;
		ptr_counter() :
			mutex(), count(1) {
		}
		ptr_counter* addRef() {
			Lock l(mutex);
			++count;
			return this;
		}
		int subRef() {
			Lock l(mutex);
			return --count;
		}
	};

	/**
	 *  A wrapper class to provide ptr with reference semantics.
	 *  For example, a %ptr can be assigned (or constructed from)
	 *  the result of a function which returns a ptr by value.
	 *
	 *  All the ptr_ref stuff should happen behind the scenes.
	 */

	template<typename _Tp1>
	struct ptr_ref {
		_Tp1* _M_ptr;

		explicit ptr_ref(_Tp1* __p) :
			_M_ptr(__p) {
		}
	};

	template<typename _Tp>
	class ptr {
		private:
		_Tp* _M_ptr;
		mutable ptr_counter * count;
		Mutex mutex;
		public:
		void subRef() {
		        PTRDEBUG("void subRef()");
		        Lock l1(mutex);
		        if ( !count ) {
                                //si c'est nul, c'est que c'est déjà propre
		                return;
		        }


			if (count->subRef() == 0) {
				if (_M_ptr) {
					delete _M_ptr;
					_M_ptr = NULL;
				}
				delete count;
				count = NULL;
			}
		}
		ptr_counter *addRef() const {
        		PTRDEBUG("void addRef()");
			Lock l1(mutex);
			if ( !count && _M_ptr ) throw raii::error::IllegalStateException("ref_count is null and _M_ptr is set");
			return count->addRef();
		}
		/// The pointed-to type.
		typedef _Tp element_type;

		/*! An %ptr is usually constructed from a raw pointer.
		 *  \param  p  A pointer (defaults to NULL).
		 *  This constructor permits constructs like
		 *  \code
		 *  ptr<Object> obj = new Object;
		 *  //and
		 *  String *str = new String;
		 *  ptr<String> pstr = str; // you no longer own str
		 *  \endcode
		 *  This constructor is not explicit, so you no longer have to construct things like
		 *  \code
		 *  ptr<String> str(new String);
		 *  \endcode
		 *  But remember that in all cases if constructor throw an exception, memory leak occurs.
		 *  \note You should'nt delete p after that
		 */
		//explicit
		ptr(element_type* __p = 0, ptr_counter *ct = new ptr_counter) :
			_M_ptr(__p), count(ct), mutex(Mutex::recmutex) {
			PTRDEBUG("ptr(element_type* __p = 0, ptr_counter *ct = new ptr_counter)");
		}

		/*! An %ptr can be constructed from another %ptr.
		 *  \param a  Another %ptr of the same type.
		 */
		ptr(const ptr& __a) :
			_M_ptr(__a.getValue()), count(NULL), mutex(Mutex::recmutex) {
			PTRDEBUG("ptr(const ptr& __a)");
			if (_M_ptr )
			        count = __a.addRef();
			else
			        count = new ptr_counter;
		}

		/*! An %ptr can be constructed from another %ptr.
		 *  \param  a  Another %ptr of a different but related type.
		 *
		 *  A pointer-to-Tp1 must be convertible to a
		 *  pointer-to-Tp/element_type.
		 *
		 */
		template<typename _Tp1>
		ptr(const ptr<_Tp1>& __a) :
			_M_ptr(NULL), count(NULL), mutex(Mutex::recmutex) {

			PTRDEBUG("template<typename _Tp1> ptr(const ptr<_Tp1>& __a)");

			_M_ptr = dynamic_cast<_Tp*> (__a.getValue());
			if (__a.getValue() != NULL && _M_ptr == NULL) {
				throw BadCastException(typeid(__a.getValue()).name());
			}
			if ( _M_ptr )
        			count = __a.addRef();
        		else
        		        count = new ptr_counter;
		}

		/*! %ptr assignment operator.
		 *  \param  a  Another %ptr of the same type.
		 *
		 */
		ptr&
		operator=(const ptr& __a) {
	                Lock l1(mutex);
			PTRDEBUG("ptr& operator=(const ptr& __a)");
			subRef();
			_M_ptr = __a.getValue();
			if ( _M_ptr )
        			count = __a.addRef();
        		else
        		        count = new ptr_counter;
			return *this;
		}

		/*! %ptr assignment operator.
		 * \param a  Another %ptr of a different but related type.
		 *
		 *  A pointer-to-Tp1 must be convertible to a pointer-to-Tp/element_type.
		 *
		 */
		template<typename _Tp1>
		ptr&
		operator=(const ptr<_Tp1>& __a) {
		        Lock l1(mutex);
			PTRDEBUG("template<typename _Tp1> ptr operator=(const ptr<_Tp1>& __a)");
			subRef();
			count = NULL;
			_M_ptr = dynamic_cast<_Tp*> (__a.getValue());
			if (__a.getValue() != NULL && _M_ptr == NULL) {
			        count = new ptr_counter;
				throw BadCastException(typeid(__a.getValue()).name());
			}
			if ( _M_ptr )
        			count = __a.addRef();
        		else
        		        count = new ptr_counter;
			return *this;
		}

		/*! When the %ptr goes out of scope, the object it owns is
		 *  deleted if refcount is 0.  If it no longer owns anything (i.e., @c get() is
		 *  @c NULL), then this has no effect.
		 */
		/*virtual*/ ~ptr() {
		        PTRDEBUG("virtual ~ptr()");
			subRef();

		}

		/*! Smart pointer dereferencing.
		 *
		 *  If this %ptr no longer owns anything, then this
		 *  operation will throw NullPointerException.  (For a smart pointer,
		 *  "no longer owns anything" is the same as being a null pointer,
		 *  and you know what happens when you dereference one of those...)
		 */
		element_type&
		operator*() const {
			PTRDEBUG("element_type& operator*() const");
			if ( _M_ptr == 0 ) throw NullPointerException("Dereferencing bad Pointer: *NULL");
			return *_M_ptr;
		}

		/*! Smart pointer dereferencing.
		 *
		 *  This returns the pointer itself, which the language then will
		 *  automatically cause to be dereferenced.
		 *  If this %ptr no longer owns anything, then this operation will throw
		 *  a NullPointerException.
		 */
		element_type*
		operator->() const {
			PTRDEBUG("element_type* operator->()");
			if (_M_ptr == 0) throw NullPointerException(
					"Dereferencing bad Pointer: NULL->");
			return _M_ptr;
		}

		/*!  Bypassing the smart pointer.
		 *  \return  The raw pointer being managed.
		 *
		 *  You can get a copy of the pointer that this object owns, for
		 *  situations such as passing to a function which only accepts
		 *  a raw pointer.
		 *
		 *  \note  This %ptr still owns the memory.
		 */
		element_type*
		getValue() const {
			PTRDEBUG("element_type* getValue() const");
			return _M_ptr;
		}

		/*! brief  Bypassing the smart pointer.
		 *  \ return  The raw pointer being managed.
		 *
		 *  You can get a copy of the pointer that this object owns, for
		 *  situations such as passing to a function which only accepts
		 *  a raw pointer.
		 *
		 *  \note  This %ptr still owns the memory.
		 */
		element_type*
		getValue() {
			PTRDEBUG("element_type* getValue()");
			return _M_ptr;
		}

		/*! Automatic conversions
		 *
		 *  These operations convert a %ptr into and from a ptr_ref
		 *  automatically as needed.  This allows constructs such as
		 *  \code
		 *    ptr<Derived>  func_returning_ptr(.....);
		 *    ...
		 *    ptr<Base> ptr = func_returning_ptr(.....);
		 *  \endcode
		 */
		ptr(ptr_ref<element_type> __ref) :
			_M_ptr(__ref._M_ptr), count(new ptr_counter), mutex(Mutex::recmutex) {
			PTRDEBUG("ptr(ptr_ref<element_type> __ref)");
			throw raii::error::RaiiException("is used");
		}

		template<typename _Tp1>
		operator ptr_ref<_Tp1>() {
			PTRDEBUG("template<typename _Tp1> operator ptr_ref<_Tp1>()");
			throw raii::error::RaiiException("is used");
			_Tp1* p = dynamic_cast<_Tp1*> (_M_ptr);

			if (_M_ptr != 0 && p == 0) {
				throw raii::error::BadCastException(typeid(p).name());
			}
			return ptr_ref<_Tp1> (p);
		}

		/*! Automatic conversions
		 *
		 *  These operations convert a %ptr into and from a ptr_ref
		 *  automatically as needed.  This allows constructs such as
		 *  \code
		 *    ptr<Derived>  func_returning_ptr(.....);
		 *    ...
		 *    ptr<Base> ptr = func_returning_ptr(.....);
		 *  \endcode
		 */
		template<typename _Tp1>
		operator ptr<_Tp1>() {
			PTRDEBUG("template<typename _Tp1> operator ptr<_Tp1>()");
			if ( _M_ptr ) {
				_Tp1* p = dynamic_cast<_Tp1*> (this->getValue());
				if (_M_ptr != 0 && p == 0) {
					throw raii::error::BadCastException(typeid(_M_ptr).name());
				}
				return ptr<_Tp1> (p, count->addRef());
			}
			else
				return ptr<_Tp1> ();

		}

		/*
		 * Ce n'est pas à operator element_type*() de vérifier la nullité de _M_ptr
		 * c'est au code appelé dans le cas suivant
		 * void legacy(void *T);
		 * ptr<T> p = NULL;
		 * legacy(p);
		 * de plus c'est cette méthode qui est appelée lors de
		 * if ( p ) ou if ( !p )
		 */
		/*! Convert this %ptr to a bare pointer that can be passed to legacy code or "if" condition
		 *  \code
		 *  void legacy(void *T);
		 *  ptr<T> p = NULL;
		 *  legacy(p);
		 *  //or
		 *  if ( p ) ...;
		 *  //or
		 *  if ( !p ) ...;
		 *  \endcode
		 *
		 *  \note  This %ptr still owns the memory.
		 */
/*~~~
		operator element_type*() {
			PTRDEBUG("operator element_type*()");
			return _M_ptr;
		}
*/
/*
		operator const element_type*() const {
			PTRDEBUG("operator element_type*()");
			return _M_ptr;
		}
*/
                class Tester {
                        void operator delete(void*);
                };
                operator const Tester*() const {
                        static Tester t;
                        if ( _M_ptr )
                                return &t;
                        return 0;
                }
		/*! release the resource owned by this %ptr
		 * if this %ptr isn't the only one to handle this resource IllegalStateException
		 *
		 * \note This %ptr no longer owns the memory;
		 */
		element_type* release() {
		        Lock l1(mutex);
			PTRDEBUG("element_type* release()");
			element_type* ptr = _M_ptr;
			if (count->count != 1) throw raii::error::IllegalStateException(
					"I'm not alone with this");
			_M_ptr = 0;
			return ptr;

		}

		bool operator!() const {
		        return _M_ptr == 0;
		}

		inline friend bool operator==(const ptr& lhs, const _Tp* rhs) {
		        return lhs._M_ptr == rhs;
		}
		inline friend bool operator==(const _Tp* lhs, const ptr& rhs) {
		        return rhs._M_ptr == lhs;
		}
		inline friend bool operator!=(const ptr& lhs, const _Tp* rhs) {
		        return lhs._M_ptr != rhs;
		}
		inline friend bool operator!=(const _Tp* lhs, const ptr& rhs) {
		        return rhs._M_ptr != lhs;
		}
                template <typename _Tp1>
		inline friend bool operator==(const ptr& lhs, const _Tp1* rhs) {
		        return lhs._M_ptr == rhs;
		}
                template <typename _Tp1>
		inline friend bool operator==(const _Tp1* lhs, const ptr& rhs) {
		        return rhs._M_ptr == lhs;
		}
                template <typename _Tp1>
		inline friend bool operator!=(const ptr& lhs, const _Tp1* rhs) {
		        return lhs._M_ptr != rhs;
		}
                template <typename _Tp1>
		inline friend bool operator!=(const _Tp1* lhs, const ptr& rhs) {
		        return rhs._M_ptr != lhs;
		}

		template <typename _Tp1>
		bool operator==(const ptr<_Tp1>& rhs) const {
		        return _M_ptr == rhs._M_ptr;
		}

		template <typename _Tp1>
		bool operator!=(const ptr<_Tp1>& rhs) const {
		        return _M_ptr != rhs._M_ptr;
		}
	};
}
#endif /* PTR_IDE_KLUDGE */
#endif /* __PTR_H__ */

