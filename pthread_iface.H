#ifndef __PTHREAD_IFACE_H__
#define __PTHREAD_IFACE_H__

#include <pthread.h>
#include "Object.H"


namespace raii {

	/*! The Mutual exclusion class.
	 * \sa raii::Lock
	 */
	class Mutex : public raii::Object {
		mutable pthread_mutex_t lock_;
		public:
		typedef enum {
			fastmutex=PTHREAD_MUTEX_NORMAL,
			recmutex=PTHREAD_MUTEX_RECURSIVE,
			errchkmutex=PTHREAD_MUTEX_ERRORCHECK
		} mutexType;

		/*! Create a new %Mutex
		 * \param t is the mutex type to create, default is fast mutex (which is a normal mutex)
		 *
		 */
		Mutex(mutexType t=fastmutex) : Object() {

			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr,t);

			pthread_mutex_init(&lock_,&attr);

			pthread_mutexattr_destroy(&attr);

		}

		/*! Destroy the mutex, weither it is locked or not
		 */
		~Mutex() {
			pthread_mutex_destroy(&lock_);
		}

		/*! lock this mutex
		 * \note the better is to use raii::Lock which follows RAII pattern
		 */
		void lock() const {
			pthread_mutex_lock(&lock_);
		}

		/*! unlock this mutex
		 * \note the better is to use raii::Lock which follows RAII pattern
		 */
		void unlock() const {
			pthread_mutex_unlock(&lock_);
		}
	};

	/*! %Lock is the object that manipulate Mutex for you.
	 * It uses RAII pattern to do the job.
	 * \code
	 * Mutex criticalSection;
	 * [...]
	 * {//enter critical section
	 *     Lock l1(criticalSection);
	 *     //here the critical things to do
	 * }//leave critical section
	 * \endcode
	 */
	class Lock : public raii::Object {
		const Mutex& mutex;
		public:
		/*!
		 * Hold the %Mutex locked
		 */
		Lock(const Mutex& tex) : Object(), mutex(tex) {
			mutex.lock();
		}
		/*!
		 * Release the locked %Mutex
		 */
		~Lock() {
			mutex.unlock();
		}
	};

}

#endif /* __PTHREAD_IFACE_H__ */
