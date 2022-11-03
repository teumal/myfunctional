
#ifndef MYTYPE_TRAITS_HPP // {
    
    # include<type_traits>
    # include<cstring>
    # include<iostream>
    # define MYTYPE_TRAITS_HPP
    
    namespace soo {
        
        /**************
         * print_type
         **************/
         
        #if defined(__GNUC__) || defined(__clang__) // {
         
           template<typename...Tn>
           inline void print_type(const char* endstr = "") {
               constexpr size_t sz = sizeof(__PRETTY_FUNCTION__);
               char out[sz];
               for(size_t i=0; i<sz; ++i) {
                   out[i] = __PRETTY_FUNCTION__[i];
               }
               out[sz-3] = '\0';
               std::cout << out+46 << endstr;
           }
        // }
           
        #elif defined(_MSC_VER) // {
            
            template<typename...Tn>
            inline void print_type(const char* endstr = "") {
                char out[] = __FUNCSIG__;
                out[sizeof(__FUNCSIG__)-16] = '\0';
                std::cout << out+24 << endstr;
            }
            
        // }
        #endif
        
        
        /*******************
         * always_false
         *******************/
         
        template<typename T>
        constexpr bool always_false = false;
        
        
        /*******************
         * decay_function
         * decay_function_t
         *******************/
        
        // primary template.
        template<typename T> 
        struct decay_function { using type = T; };
        
        // specialization
        template<typename T> requires std::is_function_v<T>
        struct decay_function<T> { using type = T*; };
        
        // helper type
        template<typename T>
        using decay_function_t = typename decay_function<T>::type;
        

        /******************
         * this_type
         * this_type_t
         ******************/
         
        // primary template.
        template<typename T>
        struct this_type;
        
        // specialization for regular functions
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...)> { using type = Class; };  

        // specialization for variadic functions such as std::printf
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......)> { using type = Class; }; 

        // specialization for function types that have cv-qualifiers
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const> { using type = const Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) volatile> { using type = volatile Class; };
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const volatile> { using type = const volatile Class; };
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const> { using type = const Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) volatile> { using type = volatile Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const volatile> { using type = const volatile Class; };  

        // specialization for function types that have ref-qualifiers
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) &> { using type = Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const &> { using type = const Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) volatile &> { using type = volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const volatile &> { using type = const volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) &> { using type = Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const &> { using type = const Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) volatile &> { using type = volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const volatile &> { using type = const volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) &&> { using type = Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const &&> { using type = const Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) volatile &&> { using type = volatile Class&&; };
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const volatile &&> { using type = const volatile Class&&; };
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) &&> { using type = Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const &&> { using type = const Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) volatile &&> { using type = volatile Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const volatile &&> { using type = const volatile Class&&; };  


        // specializations for noexcept versions of all the above (C++17 and later)
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) noexcept> { using type = Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) noexcept> { using type = Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const noexcept> { using type = const Class; };
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) volatile noexcept> { using type = volatile Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const volatile noexcept> { using type = const volatile Class; };
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const noexcept> { using type = const Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) volatile noexcept> { using type = volatile Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const volatile noexcept> { using type = const volatile Class; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) & noexcept> { using type = Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const & noexcept> { using type = const Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) volatile & noexcept> { using type = volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const volatile & noexcept> { using type = const volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) & noexcept> { using type = Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const & noexcept> { using type = const Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) volatile & noexcept> { using type = volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const volatile & noexcept> { using type = const volatile Class&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) && noexcept> { using type = Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const && noexcept> { using type = const Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) volatile && noexcept> { using type = volatile Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args...) const volatile && noexcept> { using type = const volatile Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) && noexcept> { using type = Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const && noexcept> { using type = const Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) volatile && noexcept> { using type = volatile Class&&; }; 
        template<typename Ret, typename Class, typename...Args>
        struct this_type<Ret(Class::*)(Args......) const volatile && noexcept> { using type = const volatile Class&&; }; 
        
        // helper type.
        template<typename T>
        using this_type_t = typename this_type<T>::type;
        
        
        /*****************
         * find_type
         * find_type_v
         *****************/
        
        // primary template. 
        template<typename T, typename...Tn>
        struct find_type { constexpr static bool result = false; };
        
        
        // specializations.
        template<typename T, typename T1, typename...Tn>
        struct find_type<T,T1,Tn...> {
          constexpr static bool result = find_type<T,Tn...>::result;  
        };
        
        template<typename T, typename...Tn>
        struct find_type<T,T,Tn...> { constexpr static bool result = true; };
        
        // helper variable.
        template<typename T, typename...Tn>
        constexpr bool find_type_v = find_type<T,Tn...>::result;
        
        
        /*****************
         * concept Reference
         *****************/
        
        template<typename T>
        concept Reference = std::is_reference_v<T>;
        
        
        /***************
         * concept MFP
         ***************/
         
        template<typename T>
        concept MFP = std::is_member_function_pointer_v<
          std::remove_reference_t<T>
        >;
        
        
        /**************
         * concept NotEqual
         **************/
        
        template<typename T1, typename T2>
        concept NotEqual = !std::is_same_v<
          std::remove_cvref_t<T1>, 
          std::remove_cvref_t<T2> 
        >;
        
        
        /****************
         * concept Callable
         ****************/
         
        template<typename Ret, typename Functor, typename...Args>
        concept Callable = std::is_invocable_r_v<Ret,Functor,Args...>;
        
        
        /****************
         * is_function_reference
         * is_function_reference_v
         ****************/
        
        // primary template. 
        template<typename T> 
        struct is_function_reference { constexpr static bool result = false; };
        
        // specialization.
        template<Reference T> requires std::is_function_v<std::remove_reference_t<T> >
        struct is_function_reference<T> {
            constexpr static bool result = true;
        };
        
        // helper variable.
        template<typename T>
        constexpr bool is_function_reference_v = is_function_reference<T>::result;
        
        
        /***************
         * is_array_reference
         * is_array_reference_v
         ***************/
         
        // primary template.
        template<typename T>
        struct is_array_reference { constexpr static bool result = false; };
        
        // specialization.
        template<Reference T> requires std::is_array_v<std::remove_reference_t<T> >
        struct is_array_reference<T> {
            constexpr static bool result = true;
        };
        
        // helper variable.
        template<typename T>
        constexpr bool is_array_reference_v = is_array_reference<T>::result;
    }
    
// }
#endif
