
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
            
            constexpr static Placeholder<1> _1;
            constexpr static Placeholder<2> _2;
            constexpr static Placeholder<3> _3;
            constexpr static Placeholder<4> _4;
            constexpr static Placeholder<5> _5;
            constexpr static Placeholder<6> _6;
            constexpr static Placeholder<7> _7;

            /************
             * is_placeholder
             * is_placeholder_v
             ************/
            
            template<typename T>
            struct is_placeholder {
              constexpr static bool result = 
                std::is_same_v<T, decltype(_1)> ||
                std::is_same_v<T, decltype(_2)> ||
                std::is_same_v<T, decltype(_3)> ||
                std::is_same_v<T, decltype(_4)> ||
                std::is_same_v<T, decltype(_5)> ||
                std::is_same_v<T, decltype(_6)> ||
                std::is_same_v<T, decltype(_7)>;
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
            requires is_function_reference_v<Args&&> || is_array_reference_v<Args&&>
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
            
            /*******************
             * storage_size_impl
             * storage_size_impl
             *******************/
            
            template<size_t S1, size_t S2, bool Cond>
            struct storage_size_impl {
                constexpr static size_t result = S2;
            };
            
            template<size_t S1, size_t S2>
            struct storage_size_impl<S1,S2, true> {
                constexpr static size_t result = storage_size_impl<S1, S2*2, (S1 > S2*2)>::result;
            };
        }
        
        
        /**************
         * bind
         **************/
         
        // bind any closure type, but mfp.
        template<typename Functor, typename...Args>
        constexpr auto bind(Functor&& ftor, Args&&...args) requires (!MFP<Functor>) {
            using RawFunctor = decay_function_t<std::remove_reference_t<Functor>>;
            using InvokeType = std::conditional_t<is_function_reference_v<Functor&&>,RawFunctor, Functor&&>;
            
            return [m_ftor=__FORWARD(ftor), ...m_args=__FORWARD(args)] 
                   (auto&&...params) mutable -> decltype(auto) {
                       static_assert(!(sizeof...(params)>detail::bind_num<Args...>), "too many arguments to operator()");
                       static_assert(!(sizeof...(params)<detail::bind_num<Args...>), "too few arguments to operator()");
                       
                       return static_cast<InvokeType>(m_ftor) 
                              (detail::bind_arg<Args>(m_args, __FORWARD(params)...)...);
                   };
        }
        
        
        // bind 'member function pointer' with 'this'.
        template<MFP Functor, typename Class, typename...Args>
        constexpr auto bind(Functor&& mfp, Class&& pthis, Args&&...args) {
            using RequiredThis = this_type_t<std::remove_reference_t<Functor>>;
            using ThisType = std::conditional_t<
              std::is_abstract_v<std::remove_reference_t<RequiredThis> >,
              std::conditional_t<std::is_rvalue_reference_v<RequiredThis>, RequiredThis&&, RequiredThis&>,
              RequiredThis
            >;

            return [m_mfp=__FORWARD(mfp), m_this=__FORWARD(pthis), ...m_args=__FORWARD(args)]
                   (auto&&...params) mutable -> decltype(auto) {
                      static_assert(!(sizeof...(params)>detail::bind_num<Class,Args...>), "too many arguments to operator()");
                      static_assert(!(sizeof...(params)<detail::bind_num<Class,Args...>), "too few arguments to operator()");

                      return (detail::bind_this<ThisType>(
                                detail::bind_arg<Class>(m_this, __FORWARD(params)...)
                             ).*m_mfp) (detail::bind_arg<Args>(m_args, __FORWARD(params)...)...);
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
            
            template<typename...Args> requires (!MFP<T>)
            decltype(auto) operator()(Args&&...args) const {
                return (*m_ptr)(std::forward<Args>(args)...);
            }
            
            template<typename Class, typename...Args> requires MFP<T>
            decltype(auto) operator()(Class&& pthis, Args&&...args) const {
                using ThisType = this_type_t<T>;
                return (detail::bind_this<ThisType>(
                          std::forward<Class>(pthis)
                        ).**m_ptr) (std::forward<Args>(args)...);
            }
        };
        
        
        /*****************
         * ref
         *****************/
         
        template<typename T>
        constexpr auto ref(T& t) { 
            return reference_wrapper(
                const_cast<std::remove_const_t<T>&>(t) 
            ); 
        }
        
        template<typename T>
        constexpr void ref(T&&) = delete;
        
        
        /*****************
         * cref
         *****************/
         
        template<typename T>
        constexpr auto cref(T& t) {
            return reference_wrapper(
              const_cast<std::add_const_t<T>&>(t)
            );
        }
        
        template<typename T>
        constexpr void cref(T&&) = delete;
        
        
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
           template<size_t S1, size_t S2 = 64>
           constexpr static size_t storage_size = detail::storage_size_impl<S1,S2,(S1>S2)>::result;
           
           using InvokeType  = Ret(*)(function&, Args&&...);
           using ManagerType = void(*)(const function&, void*, Operation);
           using ClosureBuf  = std::byte[8];
           using ClosurePtr  = std::byte*;
           using ClosureSize = size_t;
           
           // allocate dynamic storage.
           template<size_t FunctorSize>
           void alloc() {
                if(m_bufptr==m_buf_local || m_capacity < FunctorSize) {
                    if(m_bufptr!=m_buf_local) delete m_bufptr;
                    constexpr size_t StorageSize = storage_size<FunctorSize>;
                    m_capacity = StorageSize;       // m_capacity's lifetime begins.
                    m_bufptr   = (new aligned_storage<StorageSize,
                                                      StorageSize>)->buf; // must be a power of 2, and at least 64.
                }
           }
           
           // invoke 'member function' with 'this'.
           template<MFP Functor, typename Class, typename...Params>
           static Ret invoke(function& fn, Class&& pthis, Params&&...params) {
               using ThisType = this_type_t<std::remove_reference_t<Functor>>;
               
               auto& mfp = *reinterpret_cast<Functor*>(fn.m_bufptr);
               return (detail::bind_this<ThisType>(pthis).*mfp) (std::forward<Params>(params)...);
           }
            
        
           // invoke 'any closure types'.
           template<typename RawFunctor, typename Functor>
           static Ret invoke(function& fn, Args&&...args) requires (!MFP<RawFunctor>) {
               using OriginalType = std::conditional_t<
                 is_function_reference_v<Functor>, RawFunctor, Functor
               >;
               return static_cast<OriginalType>(
                          *reinterpret_cast<RawFunctor*>(fn.m_bufptr)
                      )  (std::forward<Args>(args)...);
           }
           
           
           // closure manager.
           template<size_t FunctorSize, typename Functor>
           static void manager(const function& fn, void* out, Operation op) {
               function* fnout;
               switch(op) {
                 case Operation::TARGET_TYPE: {
                    *reinterpret_cast<const std::type_info**>(out) = &typeid(Functor); 
                    break;
                 }
                 case Operation::DESTRUCT: {
                    reinterpret_cast<Functor*>(fn.m_bufptr)->~Functor(); 
                    break;
                 }
                 case Operation::CONSTRUCT: {
                    fnout = static_cast<function*>(out);
                    if constexpr (FunctorSize > 8) {
                       fnout->alloc<FunctorSize>();
                    }
                    new(fnout->m_bufptr) 
                    Functor(*reinterpret_cast<Functor*>(fn.m_bufptr) ); 
                    break;
                 }
               };
           }
            
        private:
           ClosurePtr   m_bufptr;
           InvokeType   m_invoke;
           ManagerType  m_manager;
           union {
               alignas(8) 
               ClosureBuf  m_buf_local;
               ClosureSize m_capacity;
           };
           
        public:
           // default constructor
           function() : m_bufptr(m_buf_local), m_invoke(nullptr), m_manager(nullptr) {}
           
           // create an empty function
           function(std::nullptr_t) : m_bufptr(m_buf_local), m_invoke(nullptr), m_manager(nullptr) {}
           
           // copy constructor
           function(const function& other) { 
               m_bufptr  = m_buf_local;
               m_invoke  = other.m_invoke;
               m_manager = other.m_manager; 
               
               if(other.m_manager) {
                   other.m_manager(other, this, Operation::CONSTRUCT);
               }
           }    
           
           // move constructor
           function(function&& other) { 
               memcpy(this, &other, sizeof(function) );
               
               if(other.m_bufptr==other.m_buf_local) {
                   m_bufptr = m_buf_local;
               }
               other.m_bufptr  = other.m_buf_local;
               other.m_invoke  = nullptr;
               other.m_manager = nullptr;
           } 
           
                
           // destructor
           ~function() {  
               if(m_manager) m_manager(*this, nullptr, Operation::DESTRUCT);
               if(m_bufptr!=m_buf_local) delete m_bufptr;
            }
           
            // initialize the target with std::forward<Functor>(ftor)
            template<NotEqual<function> Functor>
            function(Functor&& ftor) requires Callable<Ret,Functor,Args...> {
                using RawFunctor = decay_function_t< // int(&)() => int() => int(*)()
                  std::remove_reference_t<Functor>
                >; 
                if constexpr (std::is_member_function_pointer_v<RawFunctor>) {
                    m_invoke = function::invoke<RawFunctor,Args...>; // invoke for member function pointer
                }
                else {
                    m_invoke = function::invoke<RawFunctor,Functor&&>; // general version of invoke
                }
                if constexpr (sizeof(RawFunctor)>8) {
                    constexpr size_t StorageSize = storage_size<sizeof(RawFunctor)>;
                    m_capacity = StorageSize;    // m_capacity's lifetime begins.
                    m_bufptr   = (new aligned_storage<StorageSize, StorageSize>)->buf;
                }
                else {
                    m_bufptr = m_buf_local;
                }
                m_manager = function::manager<sizeof(RawFunctor), RawFunctor>;
                new(m_bufptr) RawFunctor(std::forward<Functor>(ftor) );
            }
            
            
            // assign the new target with std;:forward<Functor>(ftor)
            template<NotEqual<function> Functor>
            function& operator=(Functor&& ftor) requires Callable<Ret,Functor,Args...> {
                
                using RawFunctor = decay_function_t< // int(&)() => int() => int(*)()
                  std::remove_reference_t<Functor>
                >; 
                if(m_manager) {
                    m_manager(*this, nullptr, Operation::DESTRUCT); // call ~ClosureType()
                }
                
                if constexpr (std::is_member_function_pointer_v<RawFunctor>) {
                    m_invoke = function::invoke<RawFunctor,Args...>; // invoke for member function pointer
                }
                else {
                    m_invoke = function::invoke<RawFunctor,Functor>; // general version of invoke
                }
                if constexpr (sizeof(RawFunctor)>8) {
                    alloc<sizeof(RawFunctor)>();
                }
                m_manager = function::manager<sizeof(RawFunctor), RawFunctor>;
                new(m_bufptr) RawFunctor(std::forward<Functor>(ftor) );
                return *this;
            }
           
           
            // assign a copy of target of other
            function& operator=(const function& other) {
                if(m_manager) {
                    m_manager(*this, nullptr, Operation::DESTRUCT); // call ~ClosureType()
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
                if(m_manager) {
                    m_manager(*this, nullptr, Operation::DESTRUCT); // call ~ClosureType()
                }
                memcpy(this, &other, sizeof(function) );
                if(other.m_bufptr==other.m_buf_local) {
                  m_bufptr = m_buf_local;
                }
                other.m_bufptr  = other.m_buf_local;
                other.m_invoke  = nullptr;
                other.m_manager = nullptr;
                return *this;
            }
            
            
            // drop the current target
            function& operator=(std::nullptr_t) {
                if(m_manager) m_manager(*this, nullptr, Operation::DESTRUCT); // call ~ClosureType()
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
              function temp = std::move(*this);
              *this = std::move(other);
              other = std::move(temp);
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
                if(m_invoke) {
                  return reinterpret_cast<decay_function_t<T>*>(m_bufptr);
                }
                return static_cast<decay_function_t<T>*>(nullptr);
            }
            
            // checks if the target is empty
            operator bool() const noexcept {
                return m_invoke;
            }
        };
    }
    
// }
#endif
