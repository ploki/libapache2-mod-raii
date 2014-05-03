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
namespace raii {
template<typename _Tp>
        class ptr {
                public:
                void subRef();
                ptr_counter *addRef() const;
                /// The pointed-to type.
                ptr(_Tp* __p = 0, ptr_counter *ct = new ptr_counter);
                ptr(const ptr& __a);

                template<typename _Tp1>
                ptr(const ptr<_Tp1>& __a);

                ptr&
                operator=(const ptr& __a);

                template<typename _Tp1>
                ptr&
                operator=(const ptr<_Tp1>& __a);
                ~ptr();

                _Tp& operator*() const;

                _Tp* operator->() const;

                _Tp* getValue() const;
                _Tp* getValue();

                operator _Tp*();
                operator _Tp*() const;

                ptr(ptr_ref<_Tp> __ref);

                template<typename _Tp1>
                operator ptr_ref<_Tp1>();


                template<typename _Tp1>
                operator ptr<_Tp1>();

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
                operator const Tester*() const;
                _Tp* release();

                bool operator!() const;

                inline friend bool operator==(const ptr& lhs, const _Tp* rhs);
                inline friend bool operator==(const _Tp* lhs, const ptr& rhs);
                inline friend bool operator!=(const ptr& lhs, const _Tp* rhs);
                inline friend bool operator!=(const _Tp* lhs, const ptr& rhs);
                template <typename _Tp1>
                inline friend bool operator==(const ptr& lhs, const _Tp1* rhs);
                template <typename _Tp1>
                inline friend bool operator==(const _Tp1* lhs, const ptr& rhs);
                template <typename _Tp1>
                inline friend bool operator!=(const ptr& lhs, const _Tp1* rhs);
                template <typename _Tp1>
                inline friend bool operator!=(const _Tp1* lhs, const ptr& rhs);

                template <typename _Tp1> bool operator==(const ptr<_Tp1>& rhs) const;

                template <typename _Tp1> bool operator!=(const ptr<_Tp1>& rhs) const;
        };

}
