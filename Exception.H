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
#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include "Object.H"


#include <cxxabi.h>
#include <typeinfo>
//#include <iostream>


namespace std {

  // Helper for exception objects in <except>
  void
  __throw_bad_exception(void) __attribute__((__noreturn__));

  // Helper for exception objects in <new>
  void
  __throw_bad_alloc(void) __attribute__((__noreturn__));

  // Helper for exception objects in <typeinfo>
  void
  __throw_bad_cast(void) __attribute__((__noreturn__));

  void
  __throw_bad_typeid(void) __attribute__((__noreturn__));

  // Helpers for exception objects in <stdexcept>
  void
  __throw_logic_error(const char*) __attribute__((__noreturn__));

  void
  __throw_domain_error(const char*) __attribute__((__noreturn__));

  void
  __throw_invalid_argument(const char*) __attribute__((__noreturn__));

  void
  __throw_length_error(const char*) __attribute__((__noreturn__));

  void
  __throw_out_of_range(const char*) __attribute__((__noreturn__));

  void
  __throw_runtime_error(const char*) __attribute__((__noreturn__));

  void
  __throw_range_error(const char*) __attribute__((__noreturn__));

  void
  __throw_overflow_error(const char*) __attribute__((__noreturn__));

  void
  __throw_underflow_error(const char*) __attribute__((__noreturn__));

  // Helpers for exception objects in basic_ios
  void
  __throw_ios_failure(const char*) __attribute__((__noreturn__));

}



namespace raii {
class String;
namespace error {

  class Throwable : public std::exception {
    char *message_;
  public:
    //Vector<void*> stackTrace_;
    void *stackTrace[1024];
    int depth;
    int start;
  private:
    virtual String getName() const;
  public:
    int level;
    explicit Throwable(const char *message=NULL);
    explicit Throwable(const String& message);
    virtual ~Throwable() throw();

    static String demangle(const String& sym);
    virtual String getMessage() const;
    virtual void setMessage(const String& message);
    virtual String getLocalizedMessage() const;
    virtual String toString() const;
    void fillInStackTrace();
    String getStackTrace(bool prettyPrint=false) const;
    virtual const char *what()  const throw();
    void poorManPrintStackTrace(bool dem=true) const;
    void printStackTrace() const;
    void setFaultPosition(void*pos);
  };

#define DECLARE_EXCEPTION(Derived,Base)															\
	class Derived  : public Base {																\
		public:																					\
		explicit Derived(const char *message=NULL)  : Base(message) { level++; }	\
		explicit Derived(const raii::String& message) : Base(message) { level++; }		\
		~Derived() throw() {};																	\
	};

  DECLARE_EXCEPTION(RequestHandling,Throwable);
    DECLARE_EXCEPTION(RequestOK,RequestHandling);
    DECLARE_EXCEPTION(RequestDONE,RequestHandling);
    DECLARE_EXCEPTION(RequestDECLINED,RequestHandling);
    DECLARE_EXCEPTION(RequestERROR,RequestHandling);
    DECLARE_EXCEPTION(ResetRaii,RequestHandling);
  DECLARE_EXCEPTION(Exception, Throwable);
    DECLARE_EXCEPTION(Signal,Exception);
      DECLARE_EXCEPTION(IllegalInstruction,Signal);
      DECLARE_EXCEPTION(FloatingPointException,Signal);
      DECLARE_EXCEPTION(SegmentationFault,Signal);
      DECLARE_EXCEPTION(BusError,Signal);
      DECLARE_EXCEPTION(Abort,Signal);
      DECLARE_EXCEPTION(DumpStack,Signal);
    DECLARE_EXCEPTION(LogicException,Exception);
      DECLARE_EXCEPTION(OverflowException,LogicException);
      DECLARE_EXCEPTION(UnderflowException,LogicException);
      DECLARE_EXCEPTION(NullPointerException,LogicException);
      DECLARE_EXCEPTION(ArithmeticException,LogicException);
      DECLARE_EXCEPTION(IllegalArgumentException,LogicException);
      DECLARE_EXCEPTION(NumberFormatException,LogicException);
      DECLARE_EXCEPTION(DomainException,LogicException);
      DECLARE_EXCEPTION(RangeException,LogicException);
      DECLARE_EXCEPTION(OutOfRangeException,LogicException);
      DECLARE_EXCEPTION(LengthException,LogicException);
    DECLARE_EXCEPTION(RuntimeException,Exception);
      DECLARE_EXCEPTION(CookieFormatException,RuntimeException);
      DECLARE_EXCEPTION(IOException,RuntimeException);
        DECLARE_EXCEPTION(ResponseException,IOException);
        DECLARE_EXCEPTION(IOSFailureException,IOException);
      DECLARE_EXCEPTION(IllegalStateException,RuntimeException);
      DECLARE_EXCEPTION(ServletException,RuntimeException);
      DECLARE_EXCEPTION(UnavailableException,RuntimeException);
      DECLARE_EXCEPTION(UnsupportedEncodingException,RuntimeException);
      DECLARE_EXCEPTION(MalformedURLException,RuntimeException);
      DECLARE_EXCEPTION(SQLException,RuntimeException);
      DECLARE_EXCEPTION(InvalidSessionException,RuntimeException);
      DECLARE_EXCEPTION(NotImplementedException,RuntimeException);
    DECLARE_EXCEPTION(RaiiException,Exception);
      DECLARE_EXCEPTION(DynamicLinkerException,RaiiException);
      DECLARE_EXCEPTION(BuildException,RaiiException);
      DECLARE_EXCEPTION(ApacheException,RaiiException);
      DECLARE_EXCEPTION(BadAllocException,RaiiException);
      DECLARE_EXCEPTION(BadCastException,RaiiException);
      DECLARE_EXCEPTION(BadTypeIdException,RaiiException);
      DECLARE_EXCEPTION(TerminateException,RaiiException);
      DECLARE_EXCEPTION(UnexpectedException,RaiiException);
      DECLARE_EXCEPTION(VeryBadException,RaiiException);




} //error
  using namespace error;
} //raii




#endif /* __EXCEPTION_H__ */
