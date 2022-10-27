
#ifndef MYFUNCTIONAL_HPP // {
    
    # include<exception>
    # include<typeinfo>
    # include<cstddef>
    # include"mytype_traits.hpp"
    # define MYFUNCTIONAL_HPP
    
    namespace soo {
        
        namespace placeholders {
            
            /*************
             * Placeholder
             *************/
            
            template<size_t N> 
            struct Placeholder { constexpr static size_t nth = N; };
            
            constexpr Placeholder<1> _1;
            constexpr Placeholder<2> _2;
            constexpr Placeholder<3> _3;
            constexpr Placeholder<4> _4;
            constexpr Placeholder<5> _5;
            constexpr Placeholder<6> _6;
            constexpr Placeholder<7> _7;

            /************
             * is_placeholder
             * is_placeholder_v
             ************/
            
            template<typename T>
            struct is_placeholder {
              constexpr static bool result = 
                std::is_same_v<T, const Placeholder<1>> ||
                std::is_same_v<T, const Placeholder<2>> ||
                std::is_same_v<T, const Placeholder<3>> ||
                std::is_same_v<T, const Placeholder<4>> ||
                std::is_same_v<T, const Placeholder<5>> ||
                std::is_same_v<T, const Placeholder<6>> ||
                std::is_same_v<T, const Placeholder<7>>;
            };
            
            // helper variable.
            template<typename T>
            constexpr bool is_placeholder_v = is_placeholder<T>::result;
        }
        
        
        namespace detail {
            
            /**************
             * __FORWARD
             **************/
            
            # define __FORWARD(tn) static_cast<decltype(tn)>(tn) 
            
            
            /**************
             * is_nth_placeholder
             * is_nth_placeholder_v
             **************/
             
            template<size_t N, typename T>
            struct is_nth_placeholder {
              constexpr static bool result = std::is_same_v<
                placeholders::Placeholder<N>, 
                std::remove_cvref_t<T>
              >;
            };
            
            // helper variable.
            template<size_t N, typename T>
            constexpr bool is_nth_placeholder_v = is_nth_placeholder<N,T>::result;
            
            
            /***************
             * bind_num
             ***************/
             
            template<typename...Args>
            constexpr size_t bind_num = 
             find_type_v<placeholders::Placeholder<7>,std::remove_cvref_t<Args>...> ? 7 :
             find_type_v<placeholders::Placeholder<6>,std::remove_cvref_t<Args>...> ? 6 :
             find_type_v<placeholders::Placeholder<5>,std::remove_cvref_t<Args>...> ? 5 :
             find_type_v<placeholders::Placeholder<4>,std::remove_cvref_t<Args>...> ? 4 :
             find_type_v<placeholders::Placeholder<3>,std::remove_cvref_t<Args>...> ? 3 :
             find_type_v<placeholders::Placeholder<2>,std::remove_cvref_t<Args>...> ? 2 :
             find_type_v<placeholders::Placeholder<1>,std::remove_cvref_t<Args>...> ? 1 : 0;
              
            
            /*****************
             * bind_member
             *****************/
             
            template<typename Args>
            constexpr decltype(auto) bind_member(auto&& t0) {
                return static_cast<Args&&>(t0);
            }
            
            template<typename Args>
            requires is_function_reference_v<Args> || is_array_reference_v<Args>
            constexpr decltype(auto) bind_member(auto&& t0) {
                using RawArgs = std::remove_reference_t<Args>;
                return std::forward<Args>(
                    *reinterpret_cast<RawArgs*>(t0)
                );
            }
            
            /******************
             * bind_arg
             ******************/
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0) {
                return bind_member<Args>(t0);
            }
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1) {
                using t0_t = decltype(t0);
                if constexpr (is_nth_placeholder_v<1,t0_t>) return __FORWARD(t1);
                else return bind_member<Args>(t0);
            }
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2) {
                using t0_t = decltype(t0);
                if constexpr (is_nth_placeholder_v<1,t0_t>)      return __FORWARD(t1);
                else if constexpr (is_nth_placeholder_v<2,t0_t>) return __FORWARD(t2);
                else return bind_member<Args>(t0);
            }
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3) {
                using t0_t = decltype(t0);
                if constexpr (is_nth_placeholder_v<1,t0_t>)      return __FORWARD(t1);
                else if constexpr (is_nth_placeholder_v<2,t0_t>) return __FORWARD(t2);
                else if constexpr (is_nth_placeholder_v<3,t0_t>) return __FORWARD(t3);
                else return bind_member<Args>(t0);
            }
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, auto&& t4) {
                using t0_t = decltype(t0);
                if constexpr (is_nth_placeholder_v<1,t0_t>)      return __FORWARD(t1);
                else if constexpr (is_nth_placeholder_v<2,t0_t>) return __FORWARD(t2);
                else if constexpr (is_nth_placeholder_v<3,t0_t>) return __FORWARD(t3);
                else if constexpr (is_nth_placeholder_v<4,t0_t>) return __FORWARD(t4);
                else return bind_member<Args>(t0);
            }
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                              auto&& t4, auto&& t5) {
                using t0_t = decltype(t0);
                if constexpr (is_nth_placeholder_v<1,t0_t>)      return __FORWARD(t1);
                else if constexpr (is_nth_placeholder_v<2,t0_t>) return __FORWARD(t2);
                else if constexpr (is_nth_placeholder_v<3,t0_t>) return __FORWARD(t3);
                else if constexpr (is_nth_placeholder_v<4,t0_t>) return __FORWARD(t4);
                else if constexpr (is_nth_placeholder_v<5,t0_t>) return __FORWARD(t5);
                else return bind_member<Args>(t0);
            }
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                              auto&& t4, auto&& t5, auto&& t6) {
                using t0_t = decltype(t0);
                if constexpr (is_nth_placeholder_v<1,t0_t>)      return __FORWARD(t1);
                else if constexpr (is_nth_placeholder_v<2,t0_t>) return __FORWARD(t2);
                else if constexpr (is_nth_placeholder_v<3,t0_t>) return __FORWARD(t3);
                else if constexpr (is_nth_placeholder_v<4,t0_t>) return __FORWARD(t4);
                else if constexpr (is_nth_placeholder_v<5,t0_t>) return __FORWARD(t5);
                else if constexpr (is_nth_placeholder_v<6,t0_t>) return __FORWARD(t6);
                else return bind_member<Args>(t0);
            }
            
            template<typename Args>
            constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                              auto&& t4, auto&& t5, auto&& t6, auto&& t7) {
                using t0_t = decltype(t0);
                if constexpr (is_nth_placeholder_v<1,t0_t>)      return __FORWARD(t1);
                else if constexpr (is_nth_placeholder_v<2,t0_t>) return __FORWARD(t2);
                else if constexpr (is_nth_placeholder_v<3,t0_t>) return __FORWARD(t3);
                else if constexpr (is_nth_placeholder_v<4,t0_t>) return __FORWARD(t4);
                else if constexpr (is_nth_placeholder_v<5,t0_t>) return __FORWARD(t5);
                else if constexpr (is_nth_placeholder_v<6,t0_t>) return __FORWARD(t6);
                else if constexpr (is_nth_placeholder_v<7,t0_t>) return __FORWARD(t7);
                else return bind_member<Args>(t0);
            }
            
            
            /***************
             * bind_this
             ***************/
            
            template<typename ThisType>
            constexpr decltype(auto) bind_this(auto&& thisptr) 
            requires std::is_convertible_v<decltype(thisptr), std::remove_reference_t<ThisType>*> {
                using RawThis = std::remove_reference_t<ThisType>;
                return static_cast<RawThis&>(
                  *static_cast<RawThis*>(thisptr)
                );
            }
            
            template<typename ThisType>
            constexpr decltype(auto) bind_this(auto&& thisref)
            requires std::is_convertible_v<decltype(thisref), ThisType> {
                using RawThis = std::remove_reference_t<ThisType>;
                return static_cast<ThisType&&>(
                    static_cast<RawThis&>(thisref)
                );
            }
        }
        
        
        /**************
         * bind
         **************/
         
        // bind any closure type, but mfp.
        template<typename Functor, typename...Args>
        constexpr auto bind(Functor&& ftor, Args&&...args) {
            using RawFunctor = decay_function_t<std::remove_reference_t<Functor>>;
            using InvokeType = std::conditional_t<is_function_reference_v<Functor>,RawFunctor, Functor&&>;
            
            return [m_ftor=__FORWARD(ftor), ...m_args=__FORWARD(args)] 
                   (auto&&...args) mutable -> decltype(auto) {
                       static_assert(sizeof...(args)==detail::bind_num<Args...>);
                       
                       return static_cast<InvokeType>(m_ftor) 
                              (detail::bind_arg<Args>(__FORWARD(m_args), __FORWARD(args)...)...);
                   };
        }
        
        
        // bind 'member function pointer' with 'this'.
        template<MFP Functor, typename Class, typename...Args>
        constexpr auto bind(Functor&& mfp, Class&& pthis, Args&&...args) {
            using ThisType = this_type_t<std::remove_reference_t<Functor>>;
            
            return [m_mfp=__FORWARD(mfp), m_this=__FORWARD(pthis), ...m_args=__FORWARD(args)]
                   (auto&&...args) mutable -> decltype(auto) {
                      static_assert(sizeof...(args)==detail::bind_num<Class, Args...>);
                      
                      return (detail::bind_this<ThisType>(
                                detail::bind_arg<Class>(__FORWARD(m_this), __FORWARD(args)...)
                             ).*m_mfp) (detail::bind_arg<Args>(__FORWARD(m_args), __FORWARD(args)...)...);
                   };
        }
        
        
        /*********************
         * bad_function_call
         *********************/
        
        class bad_function_call
        : public std::exception {
            virtual const char* what() const noexcept {
                return "bad function call";
            }
        };
        
        
        /***********************
         * aligned_storage
         ***********************/
         
        template<size_t align, size_t len>
        struct aligned_storage {
          alignas(align) std::byte buf[len];  
        };
        
        
        /**********************
         * reference_wrapper
         **********************/
         
        template<typename T>
        class reference_wrapper {
            T* m_ptr = nullptr;
        public:
            reference_wrapper(T&&) = delete;
            reference_wrapper(T& t) : m_ptr(&t) {}
            operator T&() const { return *m_ptr; }
            T& get() const { return *m_ptr; }
            
            template<typename...Args>
            decltype(auto) operator()(Args&&...args) const {
                return (*m_ptr)(std::forward<Args>(args)...);
            }
        };
        
        
        /*****************
         * ref
         *****************/
         
        template<typename T>
        constexpr auto ref(const T& t) { 
            return reference_wrapper(const_cast<T&>(t) ); 
        }
        
        template<typename T>
        constexpr void ref(const T&&) = delete;
        
        
        /*****************
         * cref
         *****************/
         
        template<typename T>
        constexpr auto cref(const T& t) {
            return reference_wrapper(t);
        }
        
        template<typename T>
        constexpr void cref(const T&&) = delete;
        
        
        /******************
         * function
         * function<Ret(Args...)>
         ******************/
         
        template<typename T>
        class function {
          static_assert(always_false<T>,
                        "wrong type of template parameter (1, must be \"non-variadic, non-member function\")");
        };
        
        
        // specialization for non-variadic function
        template<typename Ret, typename...Args>
        class function<Ret(Args...)> {
           enum struct Operation {
              TARGET_TYPE, DESTRUCT, CONSTRUCT  
           };
           using InvokeType  = Ret(*)(function&, Args&&...);
           using ManagerType = void(*)(const function&, void*, Operation);
           using ClosureBuf  = std::byte[8];
           using ClosurePtr  = std::byte*;
           using ClosureSize = size_t;
           
           // invoke 'member function' with 'this'.
           template<MFP Functor, typename Class, typename...Params>
           static Ret invoke(function& fn, Class&& pthis, Params&&...params) {
               using ThisType = this_type_t<std::remove_reference_t<Functor>>;
               
               auto& mfp = *reinterpret_cast<Functor*>(fn.m_bufptr);
               return (detail::bind_this<ThisType>(pthis).*mfp) (std::forward<Params>(params)...);
           }
            
        
           // invoke 'any closure types'.
           template<typename RawFunctor, typename Functor>
           static Ret invoke(function& fn, Args&&...args) {
               using OriginalType = std::conditional_t<
                 is_function_reference_v<Functor>, RawFunctor, Functor
               >;
               return std::forward<OriginalType>(*reinterpret_cast<RawFunctor*>(fn.m_bufptr) )
                      (std::forward<Args>(args)...);
           }
           
           
           // closure manager.
           template<size_t FunctorSize, typename Functor>
           static void manager(const function& fn, void* out, Operation op) {
               function* fnout;
               switch(op) {
                 case Operation::TARGET_TYPE: *reinterpret_cast<const std::type_info**>(out) = &typeid(Functor); break;
                 case Operation::DESTRUCT:    reinterpret_cast<Functor*>(fn.m_bufptr)->~Functor(); break;
                 case Operation::CONSTRUCT:   fnout = reinterpret_cast<function*>(out);
                                              if constexpr (FunctorSize>8) {
                                                if(fnout->m_bufptr!=fnout->m_buf_local )
                                                    if(fnout->m_capacity<FunctorSize ) {
                                                      delete fnout->m_bufptr; 
                                                      fnout->m_capacity = FunctorSize+63&-64;
                                                      fnout->m_bufptr = (new aligned_storage<FunctorSize+63&-64,
                                                                                             FunctorSize+63&-64>)->buf;
                                                    }
                                              }
                                              new(fnout->m_bufptr) 
                                              Functor(*reinterpret_cast<const Functor*>(fnout->m_bufptr) ); break;
               };
           }
            
        private:
           ClosurePtr   m_bufptr  = m_buf_local;
           InvokeType   m_invoke  = nullptr;
           ManagerType  m_manager = nullptr;
           union {
               alignas(8) 
               ClosureBuf  m_buf_local;
               ClosureSize m_capacity;
           };
           
        public:
           function() = default;                                    // default constructor
           function(const function& other) { *this = other; }       // copy constructor
           function(function&& other) { *this = std::move(other); } // move constructor
           function(std::nullptr_t) {}                              // create an empty function
           
           // destructor
           ~function() {  
               if(m_manager) m_manager(*this, nullptr, Operation::DESTRUCT);
               if(m_bufptr!=m_buf_local) delete m_bufptr;
            }
           
            // initialize the target with std::forward<Functor>(ftor)
            template<typename Functor>
            function(Functor&& ftor) {
                using RawFunctor = decay_function_t< // int(&)() => int() => int(*)()
                  std::remove_reference_t<Functor>
                >; 
                if constexpr (std::is_member_function_pointer_v<RawFunctor>) {
                    m_invoke = function::invoke<RawFunctor,Args...>; // invoke for member function pointer
                }
                else {
                    m_invoke = function::invoke<RawFunctor,Functor>; // general version invoke
                }
                if constexpr (sizeof(RawFunctor)>8) {
                    m_capacity = sizeof(RawFunctor) + 63 & -64;
                    m_bufptr   = (new aligned_storage<sizeof(RawFunctor)+63&-64,
                                                      sizeof(RawFunctor)+63&-64>)->buf;
                }
                m_manager = function::manager<sizeof(RawFunctor), RawFunctor>;
                new(m_bufptr) RawFunctor(std::forward<Functor>(ftor) );
            }
            
            
            // assign the new target with std;:forward<Functor>(ftor)
            template<typename Functor>
            function& operator=(Functor&& ftor) {
                using RawFunctor = decay_function_t< // int(&)() => int() => int(*)()
                  std::remove_reference_t<Functor>
                >; 
                if(m_manager) {
                    m_manager(*this, nullptr, Operation::DESTRUCT); // call the current closure's destructor.
                }
                
                if constexpr (std::is_member_function_pointer_v<RawFunctor>) {
                    m_invoke = function::invoke<RawFunctor,Args...>; // invoke for member function pointer
                }
                else {
                    m_invoke = function::invoke<RawFunctor,Functor>; // general version invoke
                }
                if constexpr (sizeof(RawFunctor)>8) {
                    if(m_bufptr!=m_buf_local )
                        if(m_capacity<sizeof(RawFunctor) ) {
                          delete m_bufptr;
                          m_capacity = sizeof(RawFunctor) + 63 & -64;
                          m_bufptr = (new aligned_storage<sizeof(RawFunctor)+63&-64,
                                                          sizeof(RawFunctor)+63&-64>)->buf;
                        }
                }
                m_manager = function::manager<sizeof(RawFunctor), RawFunctor>;
                new(m_bufptr) RawFunctor(std::forward<Functor>(ftor) );
                return *this;
            }
           
           
            // assign a copy of target of other
            function& operator=(const function& other) {
                if(m_manager) {
                    m_manager(*this, nullptr, Operation::DESTRUCT);
                }
                if(other.m_manager) {
                    other.m_manager(other, this, Operation::CONSTRUCT);
                }
                m_invoke  = other.m_invoke;
                m_manager = other.m_manager;
                return *this;
            }
            
            
            // move the target of other to this
            function& operator=(function&& other) {
                memcpy(this, &other, sizeof(function) );
                other.m_invoke  = nullptr;
                other.m_manager = nullptr;
                return *this;
            }
            
            
            // drop the current target
            function& operator=(std::nullptr_t) {
                if(m_manager) m_manager(*this, nullptr, Operation::DESTRUCT);
                m_invoke  = nullptr;
                m_manager = nullptr;
                return *this;
            }
            
            
            // invoke the current closure.
            Ret operator()(Args&&...args) const {
                if(m_invoke) {
                    return m_invoke(*const_cast<function*>(this), std::forward<Args>(args)...);
                }
                throw bad_function_call{};
            }
            
            
            // swap the stored targets of *this and other
            void swap(function& other) {
                alignas(alignof(function)) std::byte temp[sizeof(function) ];
                memcpy(temp, this, sizeof(function) );
                memcpy(this, &other, sizeof(function) );
                memcpy(&other, temp, sizeof(function) );
            }
            
            
            // get type_info of target
            const std::type_info& target_type() const {
                if(m_manager) {
                    const std::type_info* ret;
                    m_manager(*this, &ret, Operation::TARGET_TYPE);
                    return *ret;
                }
                return typeid(void);
            }
            
            
            // get target pointer
            template<typename T>
            constexpr auto target() const {
                return reinterpret_cast<decay_function_t<T>*>(m_bufptr);
            }
            
            // checks if the target is empty
            operator bool() const noexcept {
                return m_invoke;
            }
        };
    }
    
// }
#endif
