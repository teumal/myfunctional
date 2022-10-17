#ifndef MYTYPE_TRAITS_HPP // {
    # include<type_traits>
    # include<cstring>
    # include<iostream>
    
    namespace soo {
      /**********************
       * always_false
       **********************/
       
       template<typename T>
       constexpr bool always_false = false;

      
      /***********************
       * decay_function
       * decay_function_t
       ***********************/
       
       template<typename T>
       struct decay_function {
          using type = T;  
       };
       
   
       template<typename T>
       requires std::is_function_v<T>
       struct decay_function<T> {
           using type = std::add_pointer_t<T>;
       };
      
       template<typename T>
       using decay_function_t = decay_function<T>::type;
      
       /*****************
        * print_type
        *****************/
        
        #if defined(__GUNC__) // {
          template<typename...Tn>
          constexpr void print_type(const char* endstr="") {
              const size_t sz = sizeof(__PRETTY_FUNCTION__);
              char out[sz];
              for(int i=0; i<sz; ++i) {
                  out[i] = __PRETTY_FUNCTION__[i];
              }
              out[sz-3] = '\0';
              std::cout << strstr(out,"Tn = {")+6 << endstr;
          }  
        // }
        
        #elif defined(_MSC_VER) // {
          template<typename...Tn>
          constexpr void print_type(const char* endstr="") {
            char out[] = __FUNCSIG__;
            out[sizeof(__FUNCSIG__) - 16] = '\0';
            std::cout << strstr(out,"<")+1 << endstr;
          }
        // }
        
        #elif defined(__clang__) // {
          template<typename...Tn>
          constexpr void print_type(const char* endstr="") {
              const size_t sz = sizeof(__PRETTY_FUNCTION__);
              char out[sz];
              for(int i=0; i<sz; ++i) {
                  out[i] = __PRETTY_FUNCTION__[i];
              }
              out[sz-3] = '\0';
              std::cout << strstr(out,"Tn = <")+6 << endstr;
          }  
        // }
        #endif
        
        
        
        /*********************
         * find_type
         * find_type_v
         *********************/
        
        template<typename T, typename...Tn>
        struct find_type {
           constexpr static bool result = false;   
        };
        
        
        template<typename T, typename T1, typename...Tn>
        struct find_type<T,T1,Tn...> {
            constexpr static bool result = find_type<T,Tn...>::result;
        };
        
        template<typename T>
        struct find_type<T> {
            constexpr static bool result = false;
        };
        
        template<typename T, typename...Tn>
        struct find_type<T,T,Tn...> {
            constexpr static bool result = true;
        };
        
        template<typename T, typename...Tn>
        constexpr bool find_type_v = find_type<T,Tn...>::result;
        
      
        /******************
         * remove_const_ref
         * remove_const_ref_t
         ******************/
    
        template<typename T>
        struct remove_const_ref {
          using type = std::remove_const_t<
            std::remove_reference_t<T>
          >;  
        };
        
        template<typename T>
        using remove_const_ref_t = remove_const_ref<T>::type;
    
    
        /********************
         * this_type
         * this_type_t
         *******************/
        
        // primary template.
        template<typename T>
        struct this_type {
            static_assert(always_false<T>,
                          "type \"T\" must be member function pointer!");
        };
        

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
        
       template<typename T>
       using this_type_t = typename this_type<T>::type;
       
       /****************
        * is_function_reference
        * is_function_reference_v
        ***************/

       template<typename T>
       struct is_function_reference {
         constexpr static bool result = false;  
       };
       
       template<typename T>
       requires std::is_function_v<T>
       struct is_function_reference<T&> {
           constexpr static bool result = true;  
       };
       
       template<typename T>
       requires std::is_function_v<T>
       struct is_function_reference<T&&> {
           constexpr static bool result = true;  
       };
       
       template<typename T>
       constexpr bool is_function_reference_v = is_function_reference<T>::result;


        /****************
        * is_array_reference
        * is_array_reference_v
        ***************/
        
        template<typename T>
        struct is_array_reference {
            constexpr static bool result = false;
        };
        
        template<typename T, size_t SIZE>
        struct is_array_reference<T(&)[SIZE]> {
            constexpr static bool result = true;
        };
        
        template<typename T, size_t SIZE>
        struct is_array_reference<T(&&)[SIZE]> {
            constexpr static bool result = true;
        };
        
        template<typename T>
        constexpr bool is_array_reference_v = is_array_reference<T>::result;
    };
    
// }
#endif
