# myfunctional
Simple function, bind in c++20

<table><tr><td>

## soo::print_type
<sub> Defined in "mytype_traits.hpp"</sub>
```c++
template<typename...Tn>
inline void print_type(const char* endstr = "");
```
타입을 ``stdout`` 으로 출력합니다. 사용하고 있는 컴파일러에서, ``__GNUC__``, ``_MSC_VER``, ``__clang__`` 가 미리 정의가 되 있는 경우에만 사용 가능합니다. 이외의 컴파일러에서는 사용할 수 없습니다.

### Template parameter
**...Tn** - 출력하려는 타입들

### Parameter
**endstr** - 끝에 추가로 출력할 문자열

### Return value
(none)

### Example
``` c++
# include"mytype_traits.hpp"

int main()
{
    auto&& a = 10;
    
    soo::print_type<
      decltype(std::cout), int, double  // std::basic_ostream<char, std::char_traits<char> >, int, double
    >("\n");
    
    soo::print_type<
      decltype(a), std::remove_cvref_t<decltype(a)> // int&&, int
    >("\n");
}
```
</td></tr></table>


<table><tr><td>

## soo::always_false
<sub> Defined in "mytype_traits.hpp"</sub>
``` c++
template<typename T>
constexpr bool always_false = false;
```
항상 ``false`` 인 ``bool`` 변수입니다. 템플릿 매개변수 ``T`` 가 결정되야지만 인스턴스화가 되므로, ``static_assert`` 와 함께 사용하여 원치 않는 function overloading 을 컴파일 타임에 방지할 수 있습니다.

### Template parameter
**T** - ``always_false`` 를 인스턴스화 시키기 위한 임의의 타입 ``T``.

### Example
``` c++
# include"mytype_traits.hpp"

template<typename T>
class function {
    static_assert(soo::always_false<T>, "must be \"function\"");
};

template<typename Ret, typename...Args>
class function<Ret(Args...)> {
    
};

int main()
{
    // function<int> fn1; // error: static assertion failed: must be "function"
    function<int(int,int)> fn2; // OK.
}

```

</td></tr></table>

<table><tr><td>

## soo::decay_function
<sub> Defined in "mytype_traits.hpp"</sub>
``` c++
template<typename T> 
struct decay_function;
```
``T`` 가 ``int(int,int)`` 와 같은 오직 ``static function`` 타입일 때에만, ``int(*)(int,int)`` 와 같이 ``*`` 을 붙여줍니다. ``T`` 는 ``void() const volatile &&`` 와 같은 ``member function`` 일 수 없습니다(member function pointer 는 ``void(Class::*)() const volatile &&`` 처럼 해줘야 합니다). ``std::decay`` 은  템플릿 매개변수 ``T`` 가 ``function`` 또는 ``array`` 가 아닐 경우, **cv-qualifier** 와 **ref-qualifier** 를 없애버리지만, ``soo::decay_function`` 은 어떤 qualifier 도 없애지 않습니다.

### Template parameter
**T** - ``*`` 을 붙여줄 타입.

### Member type
**type** - ``T`` 가 ``function`` 이었다면 ``T*``. 이외의 경우는 ``T``.

### Helper type
``` c++
template<typename T>
using decay_function_t = typename decay_function<T>::type;
```


### Example
``` c++
# include"mytype_traits.hpp"

int main()
{
    soo::print_type<
      soo::decay_function_t<int(int,int)>,        // int (*) (int, int)
      soo::decay_function_t<size_t>,              // long unsigned int,
      soo::decay_function_t<int(const char*...)>, // int (*) (const char*, ...)
      soo::decay_function_t<char[20]>             // char [20]
    >("\n");
    
    soo::print_type<
      // error: forming pointer to qualified function type `void() const volatile &&`
      // soo::decay_function_t<void() const volatile &&>, 
      soo::decay_function_t<void()> // void (*) ()
    >();
    
    soo::print_type<
      std::decay_t<const volatile int&&>,         // int
      soo::decay_function_t<const volatile int&&> // const volatile int&&
    >();
}
```

</td></tr></table>


<table><tr><td>

## soo::this_type
<sub> Defined in "mytype_traits.hpp"</sub>
``` c++
template<typename T>
struct this_type;
```
member function pointer 를 invoke 시킬 때, 가장 적합한 타입의 ``this`` 의 타입을 줍니다. 예를 들어, ``T`` 가 ``int(Class::*) const volatile &&`` 라면 ``const volatile Class&&`` 을 결과로 얻을 수 있습니다. ``T`` 가 member function pointer 가 아닐 경우는 정의되어 있지 않습니다.

### Template parameter
**T** - member function pointer.

### Member type
**type** - 주어진 member function pointer 에 가장 적합한 ``this`` 의 타입.

### Helper type
``` c++
template<typename T>
using this_type_t = typename this_type<T>::type;
```

### Example
``` c++
# include"mytype_traits.hpp"
# include<string>

struct A {
  void foo() const volatile &&;
  void bar() &;
};


int main()
{
    soo::print_type<
       soo::this_type_t<decltype(&std::string::c_str)>,    // const std::__cxx11::basic_string<
                                                           //    char, std::char_traits<char>, std::allocator<char> 
                                                           // >
       soo::this_type_t<decltype(&A::foo)>,                // const volatile A&&
       soo::this_type_t<decltype(&A::bar)>,                // A&
       soo::this_type_t<int(A::*)(int,int) const noexcept> // const A
    >();
}
```

</td></tr></table>


<table><tr><td>

## soo::find_type
<sub> Defined in "mytype_traits.hpp"</sub>
``` c++
template<typename T, typename...Tn>
struct find_type;
```
주어진 template parameter pack ``...Tn`` 에 ``T`` 가 들어있는지 확인합니다. ``T`` 가 들어있다면 ``true``, 아니라면 ``false`` 를 돌려줍니다. ``...Tn`` 이 비어있다면, 결과는 항상 ``false`` 입니다.

### Template parameter
**T**     - template parameter pack 에서 찾고자 하는 타입. <br>
**...Tn** - ``T`` 가 들어있는지 확인할 template parameter pack.

### Member variable
**result** - ``...Tn`` 안에 ``T`` 가 있다면 ``true``. 이외의 경우라면 ``false``.

### Helper variable template
```c++
template<typename T, typename...Tn>
constexpr bool find_type_v = find_type<T,Tn...>::result;
```

### Example
``` c++
# include"mytype_traits.hpp"

int main()
{
    std::cout << std::boolalpha
              << soo::find_type_v<int, double,size_t, int**, const volatile int&&>           // false
              << soo::find_type_v<float>                                                     // false
              << soo::find_type_v<const int&, decltype(std::cout), int(), const int&, int&>; // true
}
```

</td></tr></table>


<table><tr><td>

## soo::Reference, soo::MFP, soo::NotEqual, soo::Callable
<sub> Defined in "mytype_traits.hpp"</sub>
``` c++
template<typename T>
concept Reference = std::is_reference_v<T>;  (1)
```
``` c++
template<typename T>
concept MFP = std::is_member_function_pointer_v<  (2)
  std::remove_reference_t<T>
>;
```
``` c++
template<typename T1, typename T2>
concept NotEqual = !std::is_same_v<  (3)
   std::remove_cvref_t<T1>, 
   std::remove_cvref_t<T2> 
>;
```
``` c++
template<typename Ret, typename Functor, typename...Args>
concept Callable = std::is_invocable_r_v<Ret,Functor,Args...>; (4)
```
1 ) ``concept Reference`` 는 ``T`` 가 reference 이여야 함을 명시합니다. <br> 
2 ) ``concept MFP`` 는 ``T`` 가 ref-qualifer 를 떼고 봤을 때,  member function pointer 임을 명시합니다. <br>
3 ) ``concept NotEqual`` 는 ``T1`` 과 ``T2`` 가 qualifer 를 다 떼고 봤을 때,  똑같은 타입이 **아님**을 명시합니다.   <br>
4 ) ``concept Callable`` 은 ``Functor`` 가 ``Args&&...`` 와 같은 인자를 가지고 invoke 가능해야 하며, return type 이 ``Ret`` 로 변환될 수 있어야함을 명시합니다.   <br>

### Example
```c++
# include"mytype_traits.hpp"
struct S;
int sum(int,int);

int main()
{
    std::cout << std::boolalpha;
    
    std::cout << soo::Reference<int&&>            // true
              << soo::Reference<int(*)(int,int)>; // false
              
    std::cout << soo::MFP<int(*)()>               // false
              << soo::MFP<int(S::*)()>;           // true
              
    std::cout << soo::NotEqual<const int, int>                            // false
              << soo::NotEqual<const volatile int, const volatile float>; // true
              
    std::cout << soo::Callable<float, decltype(sum), int,int> // true
              << soo::Callable<int*, decltype(sum), int,int> // false
              << soo::Callable<int, decltype(sum)>;          // false
}
```
</td></tr></table>



<table><tr><td>

## is_function_reference
<sub> Defined in "mytype_traits.hpp"</sub>
``` c++
template<typename T> 
struct is_function_reference;
```
템플릿 인자 ``T`` 가 ``int(&)()`` 와 같은 ``static function`` 의 레퍼런스라면 ``true``. 아니라면 ``false`` 를 돌려줍니다. ``int(&)()const volatile`` 과 같은 ``member function`` 의 레퍼런스는 올바른 형식이 아닙니다. 

### Template parameter
**T** - function reference 인지 검사할 타입.

### Member variable
**result** - ``T`` 가 static function reference 라면 ``true``. 이외의 경우라면 ``false``.

### Helper variable template
``` c++
template<typename T>
constexpr bool is_function_reference_v = is_function_reference<T>::result;
```

### Example
``` c++
# include"mytype_traits.hpp"

int main()
{
   std::cout << std::boolalpha
             << soo::is_function_reference_v<int(&&)(int,int)>      // true
             << soo::is_function_reference_v<void(&)()>             // true
             << soo::is_function_reference_v<void()>                // false
             << soo::is_function_reference_v<void()const volatile>; // false
}
```

</td></tr></table>


<table><tr><td>

## is_array_reference
<sub> Defined in "mytype_traits.hpp"</sub>
``` c++
template<typename T>
struct is_array_reference;
```
템플릿 인자 ``T`` 가 ``const char(&)[16]`` 과 같은 배열의 레퍼런스 타입이라면 ``true``, 아니라면 ``false`` 를 돌려줍니다. 

### Template parameter
**T** - array reference 인지 검사할 타입.

### Member variable
**result** - ``T`` 가 array reference 라면 ``true``. 이외의 경우라면 ``false``.

### Helper variable template
``` c++
template<typename T>
constexpr bool is_array_reference_v = is_array_reference<T>::result;
```

### Example
``` c++
# include"mytype_traits.hpp"

int main()
{
   std::cout << std::boolalpha
             << soo::is_array_reference_v<const char(&&)[16]>      // true
             << soo::is_array_reference_v<int(&)[]>                // true
             << soo::is_array_reference_v<int(*(&)[16])(int,int)>  // true
             << soo::is_array_reference_v<size_t[10]>;             // false
}
```

</td></tr></table>

<table><tr><td>

## soo::placeholders::_1, soo::placeholders::_2, ..., soo::placeholders::_7
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<size_t N> 
struct Placeholder { constexpr static size_t nth = N; };
            
constexpr static Placeholder<1> _1;
constexpr static Placeholder<2> _2;
constexpr static Placeholder<3> _3;
constexpr static Placeholder<4> _4;
constexpr static Placeholder<5> _5;
constexpr static Placeholder<6> _6;
constexpr static Placeholder<7> _7;
```
``soo::bind`` 함수의 인자로 사용됩니다. ``extern`` 으로 선언되어 있는 ``std::placeholders::_N`` 과는 다르게 ``constexpr static Placeholder<1> _N`` 으로 선언되어 있습니다. ``soo::bind`` 가 반환한 함수 객체에서 적어도 크기 1의 공간을 차지합니다. 

### Member variable
**nth** - ``Placeholder`` 의 번호를 나타내는 정수.

### Example
``` c++
# include"myfunctional.hpp"
# include<string>
# include<algorithm>
using namespace soo::placeholders;

void print_array(int(&arr)[6]) {
    for(auto i : arr) {
        std::cout << i << ' ';
    }
    std::cout << std::endl;
}

int main()
{
  using Comp = soo::function<bool(const int&, const int&)>;
  int arr[] = { 8, 7, 1, 5, 9, 4 };
  
  auto fn1 = soo::bind(
    soo::bind(printf,_2,_1,_1,_3), 2, "%d + %d = %d\n", 4
  );
  auto fn2 = soo::bind(
      printf, "%d\n", soo::bind(soo::bind(strcmp,_1,_1), "hello")
  );
  auto fmt = soo::bind(&std::string::c_str, _1);
  auto fn3 = soo::bind(
      printf, fmt(new std::string{"%d,%d,%d\n"} ), _3,_1,_2
  ); 
  auto fn4 = soo::bind(
      std::sort<int*,Comp>, arr, arr+6, _1
  );
  auto fn5 = soo::bind(print_array, arr);
 
  fn1(); // 2 + 2 = 4
  fn2(); // 0
  fn3(1,2,3); // hello world!
  
  fn5(); // 8 7 1 5 9 4
  fn4(std::greater<int>{} );
  
  fn5(); // 9 8 7 5 4 1
  fn4(std::less<int>{} );
  fn5(); // 1 4 5 7 8 9
}
```

</td></tr></table>

<table><tr><td>

## soo::placeholders::is_placeholder
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<typename T>
struct is_placeholder;
```
템플릿 인자 ``T`` 가 ``_1,_2,_3,_4,_5,_6,_7`` 중 하나라면 ``true``, 아니라면 ``false`` 입니다. 타입만 ``Placeholder<N>`` 인 것이 아니라, qualifier 까지 완전히 같아야 ``true`` 입니다. 임의로 ``constexpr static Placeholder<8> _8`` 를 정의하였어도 ``myfunctional.hpp`` 가 ``_7`` 까지만 지원하기에 이 경우 ``false`` 를 돌려줍니다.

### Template parameter
**T** - ``_1,_2,_3,_4,_5,_6,_7`` 중 하나 인지 검사할 타입.

### Member variable
**result** - ``T`` 가 ``decltype(_1), decltype(_2), ..., decltype(_7)`` 중 하나라면 ``true``, 이외의 경우는 ``false``.

### Helper variable template
``` c++
template<typename T>
constexpr bool is_placeholder_v = is_placeholder<T>::result;
```

### Example
``` c++
# include"myfunctional.hpp"
using namespace soo::placeholders;

int main()
{
    std::cout << std::boolalpha 
              << is_placeholder_v<decltype(_1)> // true
              << is_placeholder_v<int>          // false
              << is_placeholder_v<Placeholder<8>>; // false
}
```

</td></tr></table>

    
    
<table><tr><td>
    
## __FORWARD
<sub> Defined in "myfunctional.hpp"</sub>
```c++
# define __FORWARD(tn) 
```
``std::forward`` 의 역할을 수행하는 매크로입니다.  ``auto&& a`` 와 같은 forwarding reference 처럼,  template argument 의 타입을 알 수 없는 경우에 사용합니다. 
    
### Parameter
**tn** - value category 를 타입과 일치하도록 바꾸고자 하는 변수의 이름.
    
### Expanded value
``static_cast<decltype(tn)>(tn)``
    
### Example
``` c++
# include"myfunctional.hpp"

struct A {
    A() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    A(const A&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    A(A&&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
};

void foo(auto&& a) {
    A b(__FORWARD(a) );
}


int main()
{
   foo(A{} );    // A::A()
                 // A::A(A&&)
   foo(*new A);  // A::A()
                 // A::A(const A&)
} 
```
</td></tr></table>

    
<table><tr><td>

## soo::detail::is_nth_placeholder
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<size_t N, typename T>
struct is_nth_placeholder;
```
주어진 템플릿 인자 ``T`` 가 ``Placeholder<N>`` 이라면 ``true``, 아니라면 ``false`` 를 돌려줍니다. 
    
### Template Parameter
**N** -  
**T** - 
    
### Member value
**result** - ``T`` 가 ``Placeholder<N>`` 이라면
    
### Helper variable template
``` c++
template<size_t N, typename T>
constexpr bool is_nth_placeholder_v = is_nth_placeholder<N,T>::result; 
```
### 
</td></tr></table> 
    
