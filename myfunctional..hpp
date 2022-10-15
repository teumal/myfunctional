#ifndef MYFUNCTIONAL_HPP // {
    # include<exception>
    # include<typeinfo>
    # include<cstddef>
    # include"mytype_traits.hpp"
    # define MYFUNCTIONAL_HPP
    
    namespace soo {
        
      /********************
       * bad_function_call
       ********************/
    
      class bad_function_call 
      : public std::exception {
        virtual const char* what() const noexcept {
            return "bad function call";
        }
      };
        

       /***************
        * aligned_storage
        ***************/
        
        template<size_t align, size_t len>
        struct aligned_storage {
          alignas(align) std::byte buf[len];
        };
        
        
        /******************
         * reference_wrapper
         ******************/
        
        template<typename T>
        class reference_wrapper {
          T* m_ptr = nullptr; 
          
        public:
           reference_wrapper(T&&) = delete;
           reference_wrapper(T& t) : m_ptr(&t) {}

           operator T&() const { return *m_ptr; }
           T& get() const { return *m_ptr; }
           
           template<typename...Args>
           auto operator()(Args&&...args) const
           -> std::invoke_result_t<T&,Args...> {
               return (*m_ptr)(std::forward<Args>(args)...);
           }
        };
        
        /*****************
         * ref
         ****************/
        
        template<typename T>
        auto ref(T& t) -> reference_wrapper<T> {
            return reference_wrapper(t);
        }
        
        template<typename T>
        auto ref(const T& t) ->reference_wrapper<std::remove_const_t<T> > {
            return reference_wrapper(const_cast<T&>(t) ); 
        }
        
        template<typename T>
        void ref(T&&) = delete; // not allowed temporary object.
        
        template<typename T>
        void ref(const T&&) = delete; // not allowed temporary object.
      
        /*****************
         * cref
         ****************/
        
        template<typename T>
        auto cref(T& t) -> reference_wrapper<const T> {
            return reference_wrapper<const T>(t); 
        }
        
        template<typename T>
        auto cref(const T& t) -> reference_wrapper<const T> {
            return reference_wrapper<const T>(t);
        }
        
        template<typename T>
        void cref(T&&) = delete; // not allowed temporary object.
        
        template<typename T>
        void cref(const T&&) = delete; // not allowed temporary object.
      
      
      /********************
       * function
       * function<Ret(Args...)>
       ********************/
       
       template<typename T>
       class function {
          static_assert(always_false<T>,
                        "wrong type of template parameter (1,should be \"function\")");  
       };
       
       
       /** specialization for non-variadic function */
       template<typename Ret, typename...Args>
       class alignas(16) function<Ret(Args...)> {
           enum struct Operation {
             TARGET, TARGET_TYPE, CONSTRUCT, DESTRUCT,
           };
           using InvokeType       = Ret(*)(function&, Args&&...);
           using ClosureOperation = void(*)(const function&, void*, Operation);
           using ClosureType1     = std::byte[16];
           using ClosureType2     = std::byte*;
           using ClosureSize      = size_t;


           /** invoke 'member function' with this pointer */
           template<typename Mfp, typename Class, typename...Params>
           static Ret invoke(function& fn, Class&& thisptr, Params&&...param)
           requires std::is_member_function_pointer_v<Mfp> && std::is_pointer_v<Class> {
               if constexpr (sizeof(Mfp) <= 16) {
                   auto& mfp = *reinterpret_cast<Mfp*>(fn.m_closure_local);
                   return (thisptr->*mfp) (std::forward<Params>(param)...);
               }
               else {
                  auto& mfp = *reinterpret_cast<Mfp*>(fn.m_closure_global);
                  return (thisptr->*mfp) (std::forward<Params>(param)...);
               }
           }
           
           
           /** invoke 'member function' with this reference */
           template<typename Mfp, typename Class, typename...Params>
           static Ret invoke(function& fn, Class&& thisref, Params&&...param)
           requires std::is_member_function_pointer_v<Mfp> && std::is_reference_v<Class> {
               if constexpr (sizeof(Mfp) <= 16) {
                   auto& mfp = *reinterpret_cast<Mfp*>(fn.m_closure_local);
                   return (std::forward<Class>(thisref).*mfp) (std::forward<Params>(param)...);
               }
               else {
                  auto& mfp = *reinterpret_cast<Mfp*>(fn.m_closure_global);
                  return (std::forward<Class>(thisref).*mfp) (std::forward<Params>(param)...);
               }
           }
           

           /** invoke any other closure */
           template<typename RawFunctor, typename Functor>
           static Ret invoke(function& fn, Args&&...args)
           requires std::is_invocable_v<Functor,Args...> && 
                    std::is_same_v<std::invoke_result_t<Functor,Args...>,Ret> {
               using OriginFunctor = std::conditional_t<
                  is_function_reference_v<Functor>,RawFunctor,Functor
               >;
               if constexpr (sizeof(RawFunctor) <= 16) {
                   return std::forward<OriginFunctor>(*reinterpret_cast<RawFunctor*>(fn.m_closure_local))  
                          (std::forward<Args>(args)...);
               }
               else {
                   return std::forward<OriginFunctor>(*reinterpret_cast<RawFunctor*>(fn.m_closure_global))  
                          (std::forward<Args>(args)...);
               }
           }
           
           
           /** do operation (closure size <=16) */
           template<size_t FunctorSize, typename Functor>
           static void operate(const function& fn, void* out, Operation op)
           requires (FunctorSize<=16) {
               switch(op) {
                case Operation::TARGET:      *reinterpret_cast<const std::byte**>(out) = fn.m_closure_local; break;
                case Operation::TARGET_TYPE: *reinterpret_cast<const std::type_info**>(out) = &typeid(Functor); break;
                case Operation::CONSTRUCT:   new(reinterpret_cast<function*>(out)->m_closure_local )
                                             Functor(*reinterpret_cast<const Functor*>(fn.m_closure_local) ); break;
                case Operation::DESTRUCT:    reinterpret_cast<const Functor*>(fn.m_closure_local)->~Functor(); break;
               };
           }
           
           
           /** do operation (closure size >16) */
           template<size_t FunctorSize, typename Functor>
           static void operate(const function& fn, void* out, Operation op)
           requires (FunctorSize>16) {
               function& fn2 = *reinterpret_cast<function*>(out);
               
               switch(op) {
                case Operation::TARGET:      *reinterpret_cast<const std::byte**>(out) = fn.m_closure_global; break;
                case Operation::TARGET_TYPE: *reinterpret_cast<const std::type_info**>(out) = &typeid(Functor); break;
                case Operation::CONSTRUCT:   if(fn2.m_closure_maxsz < FunctorSize) {
                                               if(fn2.m_closure_global) delete fn2.m_closure_global;
                                               fn2.m_closure_maxsz  = FunctorSize + 63 & -64;
                                               fn2.m_closure_global = (new aligned_storage<FunctorSize+63&-64,
                                                                                           FunctorSize+63&-64>)->buf;
                                             }
                                             new(fn2.m_closure_global) 
                                             Functor(*reinterpret_cast<const Functor*>(fn.m_closure_global) ); break;
                case Operation::DESTRUCT:    reinterpret_cast<const Functor*>(fn.m_closure_global)->~Functor(); break;
               };
           }
           
       private:
           ClosureType1     m_closure_local;
           ClosureType2     m_closure_global = nullptr;
           ClosureSize      m_closure_maxsz  = 16;
           InvokeType       m_invoke         = nullptr;
           ClosureOperation m_operate        = nullptr;
       
       public:
           /** default constructor */
           function() = default;
           
           /** copy constructor */
           function(const function& other) { *this = other; }
           
           
           /** move constructor */
           function(function&& other) { *this = std::move(other); }
           
           /** create an empty function */
           function(std::nullptr_t) {}
           
           /** destructor */
           ~function() noexcept {
               if(m_operate) m_operate(*this, nullptr, Operation::DESTRUCT);
               if(m_closure_global) delete[] m_closure_global;
           }
           
           
           
           /** initialize the target with std:;forward<Functor>(ftor) */
           template<typename Functor>
           function(Functor&& ftor) 
           requires !std::is_same_v<std::remove_cvref_t<function>,  // prevent to forwarding itself.
                                    std::remove_cvref_t<Functor> >{ 
              using RawFunctor = decay_function_t<std::remove_reference_t<Functor>>; // int(&)() => int() => int(*)()
              m_operate = function::operate<sizeof(RawFunctor), RawFunctor>; 
              
              if constexpr (std::is_member_function_pointer_v<RawFunctor>) {
                m_invoke  = function::invoke<RawFunctor, Args...>;
              }
              else {
                m_invoke = function::invoke<RawFunctor,Functor>;
              }
              
              if constexpr (sizeof(RawFunctor)<=16) {
                new(m_closure_local) RawFunctor(std::forward<Functor>(ftor) );
              }
              else {
                m_closure_maxsz  = sizeof(RawFunctor) + 63 & -64; // always allocate power of 64.
                m_closure_global = (new aligned_storage<sizeof(RawFunctor)+63&-64, 
                                                        sizeof(RawFunctor)+63&-64>)->buf; 
                new(m_closure_global) RawFunctor(std::forward<Functor>(ftor) );
              }
           }
           
           
           /** assign the new target with std::forward<Functor>(ftor) */
           template<typename Functor>
           function& operator=(Functor&& ftor)
           requires !std::is_same_v<std::remove_cvref_t<Functor>,    // prevent to forwarding itself.
                                    std::remove_cvref_t<function> > {
              using RawFunctor = decay_function_t<std::remove_reference_t<Functor>>; // int() => int(*)()
              if(m_operate) {
                m_operate(*this, nullptr, Operation::DESTRUCT); // call the current closure's destructor.
              }
              m_operate = function::operate<sizeof(RawFunctor), RawFunctor>; 
              
              if constexpr (std::is_member_function_pointer_v<RawFunctor>) {
                m_invoke  = function::invoke<RawFunctor, Args...>;
              }
              else {
                m_invoke = function::invoke<RawFunctor, Functor>;
              }
              
              if constexpr (sizeof(RawFunctor)<=16) {
                new(m_closure_local) RawFunctor(std::forward<Functor>(ftor) );
              }
              else {
                if(m_closure_maxsz < sizeof(RawFunctor) ) {
                    if(m_closure_global) delete m_closure_global;
                    m_closure_maxsz  = sizeof(RawFunctor) + 63 & -64; // always allocate power of 64.
                    m_closure_global = (new aligned_storage<sizeof(RawFunctor)+63&-64, 
                                                            sizeof(RawFunctor)+63&-64>)->buf; 
                }
                new(m_closure_global) RawFunctor(std::forward<Functor>(ftor) );
              }
              return *this;
           }
           
           
           /** assign a copy of target of other */
           function& operator=(const function& other) {
               if(m_operate) {
                  m_operate(*this, nullptr, Operation::DESTRUCT);
               }
               if(other.m_operate) {
                other.m_operate(other, this, Operation::CONSTRUCT);
               }
               m_invoke  = other.m_invoke;
               m_operate = other.m_operate;
               return *this;
           }
           
           /** move the target of other to this */
           function& operator=(function&& other) {
           	   memcpy(this, &other, sizeof(function) );
               other = nullptr;
               return *this;
           }
           
           /** drop the current target  */
           function& operator=(std::nullptr_t) {
               if(m_operate) m_operate(*this, nullptr, Operation::DESTRUCT);
               m_invoke  = nullptr;
               m_operate = nullptr;
               return *this;
           }
           
           /** invoke the wrapped function */
           Ret operator()(Args&&...args) const {
               if(m_invoke) {
                   return m_invoke(*const_cast<function*>(this), std::forward<Args>(args)...);
               }
               throw bad_function_call{};
           }
           
           /** swap the stored targets of *this and other */
           void swap(function& other) {
              alignas(16) char temp[sizeof(function) ];
              memcpy(temp, this, sizeof(function) );
              memcpy(this, &other, sizeof(function) );
              memcpy(&other, temp, sizeof(function) );
           }
           
           
           /** get type_info of target */
           const std::type_info& target_type() const {
               if(m_operate) {
                   const std::type_info* ret;
                   m_operate(*this, &ret, Operation::TARGET_TYPE);
                   return *ret;
               }
               else {
                   return typeid(void);
               }
           }
        
           /** get target pointer */
           template<typename T>
           constexpr auto target() const -> decay_function_t<T>* {
               char* ret = nullptr;
               if(m_operate) {
                   m_operate(*this, &ret, Operation::TARGET);
               }
               return reinterpret_cast<decay_function_t<T>*>(ret);
           }
           
           
           /** checks if the target is empty */
           operator bool() const noexcept  {
               return m_invoke;
           }
           
           /** trace this function object */
           void trace(const char* name) const {
               int64_t* p = (int64_t*) m_closure_local;
               
               printf("/////////////////////////\n"
                      "%s::\n"
                      "m_closure_local: %zx %zx\n"
                      "m_closure_global: %p\n"
                      "m_closure_maxsz: %zd\n",
                      name,p[1], p[0], m_closure_global, m_closure_maxsz);
               std::cout << "m_invoke: " << m_invoke << '\n'
                         << "m_operate: " << m_operate << '\n'
                         << "/////////////////////////\n\n";
           }
       };
       
       
       namespace placeholders {
           
           /***************
            * Placeholder
            ***************/
            
           template<size_t>
           struct Placeholder {};
           
           constexpr Placeholder<1> _1;
           constexpr Placeholder<2> _2;
           constexpr Placeholder<3> _3;
           constexpr Placeholder<4> _4;
           constexpr Placeholder<5> _5;
           constexpr Placeholder<6> _6;
           constexpr Placeholder<7> _7;
           
           /*******************
            * is_placeholder
            * is_placeholder_v
            *******************/
            
            template<typename T>
            struct is_placeholder {
              constexpr static bool result = 
                std::is_same_v<T,const placeholders::Placeholder<1>> ||  
                std::is_same_v<T,const placeholders::Placeholder<2>> || 
                std::is_same_v<T,const placeholders::Placeholder<3>> ||  
                std::is_same_v<T,const placeholders::Placeholder<4>> ||
                std::is_same_v<T,const placeholders::Placeholder<5>> ||  
                std::is_same_v<T,const placeholders::Placeholder<6>> ||
                std::is_same_v<T,const placeholders::Placeholder<7>>;
            };
            
            template<typename T>
            constexpr bool is_placeholder_v = is_placeholder<T>::result;
       };
       
       
       /******************
        * __IS_PLACEHOLDER
        * __IS_ARRAY_FUNCTION_TYPE
        * __FORWARD
        * __M_FTOR
        * __INVOKE_MEM_FN
        *****************/

       # define __IS_PLACEHOLDER(N) std::is_same_v<placeholders::Placeholder<N>,    \
                                                   std::remove_cvref_t<decltype(t0)> >  
       # define __IS_ARRAY_FUNCTION_TYPE is_function_reference_v<Args> || \
                                         is_array_reference_v<Args>
       # define __FORWARD(tn) (decltype(tn)) tn
       # define __M_FTOR      ((std::conditional_t<is_function_reference_v<Functor&&>, \
                                                   RawFunctor,                         \
                                                   Functor&&>) m_ftor)
       # define __INVOKE_MEM_FN(...) (bind_this<ThisType>(                                \
                                        bind_arg<Class>(__FORWARD(thisptr), __VA_ARGS__)  \
                                      ).*mfp) (bind_arg<Args>(__FORWARD(args), __VA_ARGS__)...)
                            
                                      
       /*******************
        * bind_arg
        *******************/
       
       template<typename Args> 
       constexpr decltype(auto) bind_arg(auto&& t0) {
           using RawArgs = std::remove_reference_t<Args>;

           if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       template<typename Args>
       constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1) {
           using RawArgs = std::remove_reference_t<Args>;

           if constexpr (__IS_PLACEHOLDER(1) ) return __FORWARD(t1);
           else if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       template<typename Args>
       constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2) {
           using RawArgs = std::remove_reference_t<Args>;
           
           if constexpr (__IS_PLACEHOLDER(1) ) return __FORWARD(t1);
           else if constexpr (__IS_PLACEHOLDER(2) ) return __FORWARD(t2);
           else if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       template<typename Args>
       constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3) {
           using RawArgs = std::remove_reference_t<Args>;
           
           if constexpr (__IS_PLACEHOLDER(1) ) return __FORWARD(t1);
           else if constexpr (__IS_PLACEHOLDER(2) ) return __FORWARD(t2);
           else if constexpr (__IS_PLACEHOLDER(3) ) return __FORWARD(t3);
           else if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       template<typename Args>
       constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, auto&& t4) {
           using RawArgs = std::remove_reference_t<Args>;
           
           if constexpr (__IS_PLACEHOLDER(1) ) return __FORWARD(t1);
           else if constexpr (__IS_PLACEHOLDER(2) ) return __FORWARD(t2);
           else if constexpr (__IS_PLACEHOLDER(3) ) return __FORWARD(t3);
           else if constexpr (__IS_PLACEHOLDER(4) ) return __FORWARD(t4);
           else if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       template<typename Args>
       constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                         auto&& t4, auto&& t5) {
           using RawArgs = std::remove_reference_t<Args>;
           
           if constexpr (__IS_PLACEHOLDER(1) ) return __FORWARD(t1);
           else if constexpr (__IS_PLACEHOLDER(2) ) return __FORWARD(t2);
           else if constexpr (__IS_PLACEHOLDER(3) ) return __FORWARD(t3);
           else if constexpr (__IS_PLACEHOLDER(4) ) return __FORWARD(t4);
           else if constexpr (__IS_PLACEHOLDER(5) ) return __FORWARD(t5);
           else if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       template<typename Args>
       constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                         auto&& t4, auto&& t5, auto&& t6) {
           using RawArgs = std::remove_reference_t<Args>;
           
           if constexpr (__IS_PLACEHOLDER(1) ) return __FORWARD(t1);
           else if constexpr (__IS_PLACEHOLDER(2) ) return __FORWARD(t2);
           else if constexpr (__IS_PLACEHOLDER(3) ) return __FORWARD(t3);
           else if constexpr (__IS_PLACEHOLDER(4) ) return __FORWARD(t4);
           else if constexpr (__IS_PLACEHOLDER(5) ) return __FORWARD(t5);
           else if constexpr (__IS_PLACEHOLDER(6) ) return __FORWARD(t6);
           else if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       template<typename Args>
       constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                         auto&& t4, auto&& t5, auto&& t6, auto&& t7) {
           using RawArgs = std::remove_reference_t<Args>;
           
           if constexpr (__IS_PLACEHOLDER(1) ) return __FORWARD(t1);
           else if constexpr (__IS_PLACEHOLDER(2) ) return __FORWARD(t2);
           else if constexpr (__IS_PLACEHOLDER(3) ) return __FORWARD(t3);
           else if constexpr (__IS_PLACEHOLDER(4) ) return __FORWARD(t4);
           else if constexpr (__IS_PLACEHOLDER(5) ) return __FORWARD(t5);
           else if constexpr (__IS_PLACEHOLDER(6) ) return __FORWARD(t6);
           else if constexpr (__IS_PLACEHOLDER(7) ) return __FORWARD(t7);
           else if constexpr (__IS_ARRAY_FUNCTION_TYPE) return (Args&&) (*(RawArgs*) t0);
           else return (Args&&) t0;
       }
       
       
       /*****************
        * bind_this
        *****************/
        
       template<typename ThisType>
       constexpr decltype(auto) bind_this(auto&& thisptr)
       requires std::is_convertible_v<decltype(thisptr), std::remove_reference_t<ThisType>*> {
           using Class = std::remove_reference_t<ThisType>;
           return (Class&) (*(Class*) thisptr);
       }
       
       template<typename ThisType>
       constexpr decltype(auto) bind_this(auto&& thisptr) 
       requires std::is_convertible_v<decltype(thisptr), ThisType> {
           return (ThisType&&) thisptr;
       }
       
       
       /********************
        * bind
        ********************/
       
       /** bind any closure type, but 'member function pointer' */
       template<typename Functor, typename...Args>
       constexpr auto bind(Functor&& ftor, Args&&...args) 
       requires !std::is_member_function_pointer_v<std::remove_reference_t<Functor>> {
         using RawFunctor = decay_function_t<std::remove_reference_t<Functor> >;

         /** using placeholders [_1, _7] */
         if constexpr (find_type_v<decltype(placeholders::_7), std::remove_reference_t<Args>...>) {
            return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)] 
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4, auto&& arg5, auto&& arg6, auto&& arg7) mutable {
                     return __M_FTOR(bind_arg<Args>(__FORWARD(args),__FORWARD(arg1),__FORWARD(arg2),
                                                    __FORWARD(arg3),__FORWARD(arg4),__FORWARD(arg5),
                                                    __FORWARD(arg6),__FORWARD(arg7) )...);
                   };
         }
         /** using placeholders [_1, _6] */
         else if constexpr (find_type_v<decltype(placeholders::_6), std::remove_reference_t<Args>...>) {
            return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)]  
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4, auto&& arg5, auto&& arg6) mutable {
                     return __M_FTOR(bind_arg<Args>(__FORWARD(args),__FORWARD(arg1),__FORWARD(arg2),
                                                    __FORWARD(arg3),__FORWARD(arg4),__FORWARD(arg5),
                                                    __FORWARD(arg6) )...);
                   };  
         }
         /** using placeholders [_1, _5] */
         else if constexpr (find_type_v<decltype(placeholders::_5), std::remove_reference_t<Args>...>) {
            return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)]  
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4, auto&& arg5) mutable {
                     return __M_FTOR(bind_arg<Args>(__FORWARD(args),__FORWARD(arg1),__FORWARD(arg2),
                                                    __FORWARD(arg3),__FORWARD(arg4),__FORWARD(arg5) )...);
                   };  
         }
         /** using placeholders [_1, _4] */
         else if constexpr (find_type_v<decltype(placeholders::_4), std::remove_reference_t<Args>...>) {
            return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)] 
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4) mutable {
                     return __M_FTOR(bind_arg<Args>(__FORWARD(args),__FORWARD(arg1),__FORWARD(arg2),
                                                    __FORWARD(arg3),__FORWARD(arg4) )...);
                   }; 
         }
         /** using placeholders [_1, _3] */
         else if constexpr (find_type_v<decltype(placeholders::_3), std::remove_reference_t<Args>...>) {
            return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)] 
                   (auto&& arg1, auto&& arg2, auto&& arg3) mutable {
                     return __M_FTOR(bind_arg<Args>(__FORWARD(args),__FORWARD(arg1),__FORWARD(arg2),
                                                    __FORWARD(arg3) )...);
                   };  
         }
         /** using placeholders [_1, _2] */
         else if constexpr (find_type_v<decltype(placeholders::_2), std::remove_reference_t<Args>...>) {
            return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)] 
                   (auto&& arg1, auto&& arg2) mutable {
                     return __M_FTOR(bind_arg<Args>(__FORWARD(args),__FORWARD(arg1),__FORWARD(arg2) )...);
                   }; 
         }
         /** using placeholders [_1] */
         else if constexpr (find_type_v<decltype(placeholders::_1), std::remove_reference_t<Args>...>) {
            return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)] 
                   (auto&& arg1) mutable {
                    return __M_FTOR(bind_arg<Args>(__FORWARD(args),__FORWARD(arg1) )...);
                   };
         }
         /** not using placeholders */
         else {
           return [m_ftor=__FORWARD(ftor), ...args=__FORWARD(args)] () mutable {
                    return __M_FTOR(bind_arg<Args>(__FORWARD(args) )...);
                  };
         }
       }
       
       
       /** bind 'member function pointer' */
       template<typename Functor, typename Class, typename...Args>
       auto bind(Functor&& mfp, Class&& thisptr, Args&&...args) 
       requires std::is_member_function_pointer_v<std::remove_reference_t<Functor>> {
         using ThisType = this_type_t<std::remove_reference_t<Functor> >;
           
         /** using placeholders [_1, _7] */
         if constexpr (find_type_v<decltype(placeholders::_7), std::remove_reference_t<Class>, std::remove_reference_t<Args>...>) {
            return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)]
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4, auto&& arg5, auto&& arg6, auto&& arg7) mutable {
                     return __INVOKE_MEM_FN(__FORWARD(arg1),__FORWARD(arg2),__FORWARD(arg3),
                                            __FORWARD(arg4),__FORWARD(arg5),__FORWARD(arg6),
                                            __FORWARD(arg7) );
                   };
         }
         /** using placeholders [_1, _6] */
         else if constexpr (find_type_v<decltype(placeholders::_6), std::remove_reference_t<Class>, std::remove_reference_t<Args>...>) {
            return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)]
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4, auto&& arg5, auto&& arg6) mutable {
                     return __INVOKE_MEM_FN(__FORWARD(arg1),__FORWARD(arg2),__FORWARD(arg3),
                                            __FORWARD(arg4),__FORWARD(arg5),__FORWARD(arg6) );
                   };  
         }
         /** using placeholders [_1, _5] */
         else if constexpr (find_type_v<decltype(placeholders::_5), std::remove_reference_t<Class>, std::remove_reference_t<Args>...>) {
            return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)]
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4, auto&& arg5) mutable {
                     return __INVOKE_MEM_FN(__FORWARD(arg1),__FORWARD(arg2),__FORWARD(arg3),
                                            __FORWARD(arg4),__FORWARD(arg5) );
                   };  
         }
         /** using placeholders [_1, _4] */
         else if constexpr (find_type_v<decltype(placeholders::_4), std::remove_reference_t<Class>, std::remove_reference_t<Args>...>) {
            return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)]
                   (auto&& arg1, auto&& arg2, auto&& arg3, auto&& arg4) mutable {
                     return __INVOKE_MEM_FN(__FORWARD(arg1),__FORWARD(arg2),__FORWARD(arg3),__FORWARD(arg4) );
                   }; 
         }
         /** using placeholders [_1, _3] */
         else if constexpr (find_type_v<decltype(placeholders::_3), std::remove_reference_t<Class>, std::remove_reference_t<Args>...>) {
            return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)] 
                   (auto&& arg1, auto&& arg2, auto&& arg3) mutable {
                     return __INVOKE_MEM_FN(__FORWARD(arg1),__FORWARD(arg2),__FORWARD(arg3) );
                   };  
         }
         /** using placeholders [_1, _2] */
         else if constexpr (find_type_v<decltype(placeholders::_2), std::remove_reference_t<Class>, std::remove_reference_t<Args>...>) {
            return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)]
                   (auto&& arg1, auto&& arg2) mutable {
                     return __INVOKE_MEM_FN(__FORWARD(arg1),__FORWARD(arg2) );
                   }; 
         }
         /** using placeholders [_1] */
         else if constexpr (find_type_v<decltype(placeholders::_1), std::remove_reference_t<Class>, std::remove_reference_t<Args>...>) {
            return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)]
                   (auto&& arg1) mutable {
                    return __INVOKE_MEM_FN(__FORWARD(arg1) );
                   };
         }
         /** not using placeholders */
         else {
           return [mfp, thisptr=__FORWARD(thisptr), ...args=__FORWARD(args)] () mutable {
                    return (bind_this<ThisType>(
                                bind_arg<Class>(__FORWARD(thisptr) )
                           ).*mfp)  (bind_arg<Args>(__FORWARD(args) )...);
                  };
         }
       }
     
    };
// }
#endif
