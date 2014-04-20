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
