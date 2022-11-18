# myfunctional
Simple function, bind in c++20 <br>
``std::function`` 과 ``std::bind`` 를 간단하게 구핸해본 라이브러리입니다. <br>
자세한 것은 [std::function, std::bind 를 만들어보자](https://blog.naver.com/zmsdkemf8703/222894357301) 강좌에서 볼 수 있습니다. <br>
아래에는 ``soo::function`` 와 ``soo::bind`` 을 구현하는데 사용한 함수들과 클래스들의 레퍼런스를 정리해두었습니다. <br>
예시에 있는 코드들은 ``g++ 12.20``, ``msvc 19.34``, ``clang 15.0.0`` 에서 테스트 되었습니다.


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
      printf, "%d\n", soo::bind(strcmp,_1,_1) ("hello")
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
  fn3(1,2,3); // 3,1,2
  
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
``std::forward`` 의 역할을 수행하는 매크로입니다.  ``auto&& a`` 와 같은 forwarding reference 처럼,  template argument 의 타입을 알 수 없는 경우에 사용합니다. ``tn``이 ``int`` 과 같은 non-reference 타입이라면 ``static_cast<int>(tn)``가 되어 복사가 발생할 수 있음에 유의해야합니다. 
    
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
주어진 템플릿 인자 ``T`` 가 qualifier 를 모두 떼고 봤을 때, ``Placeholder<N>`` 와 같다면 ``true``, 아니라면 ``false`` 를 돌려줍니다. ``soo::detail::bind_arg`` 의 내부 구현에 사용됩니다. 
    
### Template Parameter
**N** - ``Placeholder<N>`` 의 번호 N. <br>
**T** - ``Placeholder<N>`` 인지 확인할 타입.
    
### Member value
**result** - ``T`` 가 ``Placeholder<N>`` 이라면 ``true``, 이외의 경우에는 ``false``.
    
### Helper variable template
``` c++
template<size_t N, typename T>
constexpr bool is_nth_placeholder_v = is_nth_placeholder<N,T>::result; 
```
### Example
``` c++
# include"myfunctional.hpp"
using namespace soo::detail;
using namespace soo::placeholders;

int main()
{
    std::cout << std::boolalpha
              << is_nth_placeholder_v<1,decltype(_1)>         // true
              << is_nth_placeholder_v<2,decltype(_1)>         // true
              << is_nth_placeholder_v<3,decltype(std::cout)>; // false
}
```
</td></tr></table> 
    
<table><tr><td>

## soo::detail::bind_num
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<typename...Args>
constexpr size_t bind_num = 
 find_type_v<placeholders::Placeholder<7>,std::remove_cvref_t<Args>...> ? 7 :
 find_type_v<placeholders::Placeholder<6>,std::remove_cvref_t<Args>...> ? 6 :
 find_type_v<placeholders::Placeholder<5>,std::remove_cvref_t<Args>...> ? 5 :
 find_type_v<placeholders::Placeholder<4>,std::remove_cvref_t<Args>...> ? 4 :
 find_type_v<placeholders::Placeholder<3>,std::remove_cvref_t<Args>...> ? 3 :
 find_type_v<placeholders::Placeholder<2>,std::remove_cvref_t<Args>...> ? 2 :
 find_type_v<placeholders::Placeholder<1>,std::remove_cvref_t<Args>...> ? 1 : 0;
```
``soo::bind`` 함수가 반환한 함수 객체를 invoke 할 시, 필요한 인자의 수를 가지고 있는 ``size_t`` 타입의 입니다. ``...Args`` 에 ``decltype(_1)`` 이 들어있다면 받아야할 인자는 1개이며, ``Args...`` 에 ``decltype(_7)`` 이 들어있다면 받아야할 인자는 7개가 됩니다. ``soo::bind`` 에서는 이 variable template 을 이용해서, ``operator()`` 호출에 필요한 인자가 너무 많거나 적으면, ``static_assert(false)`` 로 컴파일 에러를 내보냅니다.

### Example
``` c++
# include"myfunctional.hpp"
using namespace soo::detail;
using namespace soo::placeholders;

int main()
{
    std::cout << std::boolalpha
              << bind_num<decltype(_1), decltype(_3), decltype(_7)> // 7
              << bind_num<int,double, decltype(_4)>                 // 4
              << bind_num<int&, int(&)[10]>;                        // 0
}
```
</td></tr></table> 


<table><tr><td>

## soo::detail::bind_member
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<typename Args>
constexpr decltype(auto) bind_member(auto&& t0);  (1)
```
``` c++
template<typename Args>
requires is_function_reference_v<Args&&> || is_array_reference_v<Args&&>
constexpr decltype(auto) bind_member(auto&& t0);  (2)
```
``soo::detail::bind_arg`` 의 내부 구현에 사용됩니다. 항상 **reference** 타입을 반환하며, 이 과정에서 어떠한 복사도 일어나지 않습니다. 결과적으로 ``std::forward`` 와 똑같은 역할을 하는 함수입니다.

1 ) 단순히 ``std::forward<Args>(t0)`` 을 반환합니다.  <br>
2 ) ``Args&&`` 가 ``int(&&)(int,int)`` 또는 ``int(&)(int,int)`` 와 같은 function reference 이거나, ``const char(&&)[6]`` 또는 ``const char(&)[6]`` 와 같은 array reference 인 경우, ``t0`` 이 ``int(*)(int,int)`` 또는 ``const char*`` 으로 decay 되었다는 의미이므로 특별한 처리가 필요합니다. 먼저, ``*reinterpret_cast<std::remove_reference_t<Args>*>(t0)`` 으로 ``t0`` 이 담고 있는 포인터 값을  ``const char(*)[6]`` 와 같은 pointer to array 또는 ``int(*)(int,int)`` 와 같은 pointer to function 타입으로 캐스팅해줍니다(배열의 경우, ``const char(*)[6]`` 라는 배열의 주소값과 배열의 첫 번째 원소의 주소 값인 ``const char*`` 은 서로 같은 곳을 가리킵니다).  그 후, ``std::forward<Args>()`` 을 적용하여 원래 레퍼런스 타입을 반환합니다.

### Template parameter
**Args** - ``t0`` 의 원래 타입.

### Parameter
**t0** - perfect forwarding 이 필요한 객체의 레퍼런스.

### Return value
1 ) ``std::forward<Args>(t0)`` <br>
2 ) ``std::forward<Args>(*reinterpret_cast<std::remove_referennce_t<Args>*>(t0) )``

### Example
``` c++
# include"myfunctional.hpp"
using namespace soo::detail;

int sum(int a, int b) {
    return a + b;
}

void print_array(const char(&arr)[7] ) {
    for(auto i : arr) {
        std::cout << i;
    }
}

void invoke_sum(int(&fn)(int,int) ) {
    std::cout << fn(1,2);
}

void print_int(int&& a) {
    std::cout << a;
}

int main()
{
   const char* s = "hello\n";
   int(*fn)(int,int) = sum;
   int&& a = 10;
   
   soo::print_type<
     decltype(bind_member<const char(&)[7]>(s) ),  // const char (&) [7]
     decltype(bind_member<int(&)(int,int)>(sum) ), // int (&) (int,int)
     decltype(bind_member<int>(a) )                // int&&
   >();
   
   // print_array(s); // error: invalid initialization of reference of type `const char (&)[7]` from expression of type `const char*`
   print_array(bind_member<const char(&)[7]>(s) ); // hello
   
   // invoke_sum(fn); // error: invalid initialization of reference of type ‘int (&)(int, int)’ from expression of type ‘int (*)(int, int)’
   invoke_sum(bind_member<int(&)(int,int)>(fn) ); // 3
   
   //print_int(a); // error: cannot bind rvalue reference of type ‘int&&’ to lvalue of type ‘int’
   print_int(bind_member<int>(a) ); // 10
}
```

</td></tr></table> 



<table><tr><td>

## soo::detail::bind_arg
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0);  (1)
```
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1);  (2)
```
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2);  (3)
```
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3);  (4)
```
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, auto&& t4);  (5)
```
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                  auto&& t4, auto&& t5);    (6)
```
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                  auto&& t4, auto&& t5, auto&& t6);     (7)
```
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                  auto&& t4, auto&& t5, auto&& t6, auto&& t7);  (8)
```
``soo::bind`` 의 내부 구현에 사용됩니다. 항상 **reference** 타입을 반환하며, 이 과정에서 어떠한 복사도 일어나지 않습니다. 결과적으로 ``std::forward`` 와 똑같은 역할을 하는 함수입니다. ``t0`` 이 ``_N`` 과 같은 ``Placeholder<N>`` 인 경우 ``t1``,``t2``,..., ``t7`` 중 하나로 바인딩합니다. 이외의 경우, ``t0`` 으로 바인딩합니다.

1 ) ``t0`` 으로 바인딩합니다.  <br>
2 ) ``t0`` 의 타입에 따라, ``t0``,``t1`` 중 하나로 바인딩합니다. <br>
3 ) ``t0`` 의 타입에 따라, ``t0``,``t1``,``t2`` 중 하나로 바인딩합니다. <br>
4 ) ``t0`` 의 타입에 따라, ``t0``,``t1``,..., ``t3`` 중 하나로 바인딩합니다. <br>
5 ) ``t0`` 의 타입에 따라, ``t0``,``t1``,..., ``t4`` 중 하나로 바인딩합니다. <br>
6 ) ``t0`` 의 타입에 따라, ``t0``,``t1``,..., ``t5`` 중 하나로 바인딩합니다. <br>
7 ) ``t0`` 의 타입에 따라, ``t0``,``t1``,..., ``t6`` 중 하나로 바인딩합니다. <br>
8 ) ``t0`` 의 타입에 따라, ``t0``,``t1``,..., ``t7`` 중 하나로 바인딩합니다. <br>

### Template parameter
**Args** - ``t0`` 의 원래 타입.

### Parameters
**t0** - ``soo::bind`` 로 묶어준 인자 중 하나.   <br>
**t1** - ``_1`` 에 묶어줄 ``soo::bind::<lambda()>::operator()`` 의 인자. <br>
**t2** - ``_2`` 에 묶어줄 ``soo::bind::<lambda()>::operator()`` 의 인자.<br>
**t3** - ``_3`` 에 묶어줄 ``soo::bind::<lambda()>::operator()`` 의 인자.<br>
**t4** - ``_4`` 에 묶어줄 ``soo::bind::<lambda()>::operator()`` 의 인자.<br>
**t5** - ``_5`` 에 묶어줄 ``soo::bind::<lambda()>::operator()`` 의 인자.<br>
**t6** - ``_6`` 에 묶어줄 ``soo::bind::<lambda()>::operator()`` 의 인자.<br>
**t7** - ``_7`` 에 묶어줄 ``soo::bind::<lambda()>::operator()`` 의 인자.<br>

### Return value
1 ) ``bind_member<Args>(t0)`` <br>
2 ) ``bind_member<Args>(t0)``, ``__FORWARD(t1)`` 중 하나. <br>
3 ) ``bind_member<Args>(t0)``, ``__FORWARD(t1)``, ``__FORWARD(t2)`` 중 하나.<br>
4 ) ``bind_member<Args>(t0)``, ``__FORWARD(t1)``, ``__FORWARD(t2)``, ``__FORWARD(t3)`` 중 하나.<br>
5 ) ``bind_member<Args>(t0)``, ``__FORWARD(t1)``, ``__FORWARD(t2)``, ..., ``__FORWARD(t4)`` 중 하나.<br>
6 ) ``bind_member<Args>(t0)``, ``__FORWARD(t1)``, ``__FORWARD(t2)``, ..., ``__FORWARD(t5)`` 중 하나.<br>
7 ) ``bind_member<Args>(t0)``, ``__FORWARD(t1)``, ``__FORWARD(t2)``, ..., ``__FORWARD(t6)`` 중 하나.<br>
8 ) ``bind_member<Args>(t0)``, ``__FORWARD(t1)``, ``__FORWARD(t2)``, ..., ``__FORWARD(t7)`` 중 하나.<br>

### Example
``` c++
# include"myfunctional.hpp"
using namespace soo::detail;
using namespace soo::placeholders;
# define BINDING_ARGS __FORWARD(arg1),__FORWARD(arg2),__FORWARD(arg3),__FORWARD(arg4),__FORWARD(arg5),__FORWARD(arg6),__FORWARD(arg7)

int sum(int a, int b) {
    return a + b;
}

void print_array(const char(&arr)[2] ) {
    for(auto c : arr) {
        std::cout << c;
    }
    std::cout << std::endl;
}


int main()
{
    using func_t  = int(int,int);
    using cstr_t  = const char[6]; 
    using cvint_t = const volatile int;
    
    int         arg1    = 10;
    float       arg2    = 2.0f;
    double      arg3    = 3.14;
    cstr_t&     arg4    = "hello";
    func_t&     arg5    = sum;
    int&&       arg6    = 15;
    cvint_t&    arg7    = *new cvint_t(20);
    
    
    std::cout << bind_arg<decltype(_1)>(_1, BINDING_ARGS)        // 10
              << bind_arg<decltype(_4)>(_4, BINDING_ARGS)        // hello
              << bind_arg<std::nullptr_t>(nullptr, BINDING_ARGS) // nullptr
              << std::endl;
    
    soo::print_type<
      decltype(  bind_arg<decltype(_1)>(_1,BINDING_ARGS) ), // int&&
      decltype(  bind_arg<decltype(_2)>(_2,BINDING_ARGS) ), // float&&
      decltype(  bind_arg<decltype(_3)>(_3,BINDING_ARGS) ), // double&&
      decltype(  bind_arg<decltype(_4)>(_4,BINDING_ARGS) ), // const char (&) [6]
      decltype(  bind_arg<decltype(_5)>(_5,BINDING_ARGS) ), // int (&) (int, int)
      decltype(  bind_arg<decltype(_6)>(_6,BINDING_ARGS) ), // int&&
      decltype(  bind_arg<decltype(_7)>(_7,BINDING_ARGS) ), // const volatile int&
      decltype(  bind_arg<int>(10,BINDING_ARGS) )           // int&&
    >();
    
    const char* a = "hi";
    // print_array(a); // error: invalid initialization of reference of type ‘const char (&)[2]’ from expression of type ‘const char*’
    print_array(bind_arg<const char(&)[2]>(a) ); // hi
}
```
</td></tr></table> 


<table><tr><td>

## soo::detail::bind_this
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<typename ThisType>
constexpr decltype(auto) bind_this(auto&& thisptr) 
requires std::is_convertible_v<decltype(thisptr), std::remove_reference_t<ThisType>*>;  (1)
```
``` c++
template<typename ThisType>
constexpr decltype(auto) bind_this(auto&& thisref)
requires std::is_convertible_v<decltype(thisref), ThisType>;  (2)
```
``soo::bind``, ``soo::function`` 의 내부 구현에 사용됩니다. 항상 **reference** 타입을 반환하며, 이 과정에서 어떠한 복사도 일어나지 않습니다. member function pointer 를 invoke 하는 데 필요한 ``'this'`` 피연산자를 항상 ``.*`` 연산자를 사용하도록 레퍼런스 타입으로 만들어줍니다. 

1 ) ``thisptr`` 이 ``std::remove_reference_t<ThisType>*`` 으로 변환될 수 있어야 하며, 결과는 항상 lvalue-reference 입니다(포인터는 항상 lvalue 를 가리키고 있다고 가정합니다). 즉, rvalue-reference 의 ``'this'`` 를 받는 member function 은 호출할 수 없습니다. <br>
2 ) ``thisref`` 가 ``ThisType`` 으로 변환될 수 있어야 하며, 결과는 ``ThisType&&`` 입니다. <br>

### Template parameter
**ThisType** - member function pointer 를 호출하기에 가장 적합한 ``'this'`` 의 타입. 보통 ``soo::this_type_t`` 를 사용하여 얻어온 타입이 전달됩니다.

### Parameter
**thisptr** - ``std::remove_reference_t<ThisType>*`` 으로 변환될 수 있는 Class 타입. <br>
**thisref** - ``ThisType`` 으로 변환될 수 있는 Class 타입. <br>

### Return value
1 ) ``static_cast<RawThis&>(*static_cast<RawThis*>(thisptr) )``. ``RawThis`` 는 ``std::remove_reference_t<ThisType>`` 입니다. <br>
2 ) ``static_cast<ThisType&&>(static_cast<RawThis&>(thisref) )``. ``RawThis`` 는 ``std::remove_reference_t<ThisType>`` 입니다.

### Example
``` c++
# include"myfunctional.hpp"
# include<string>
using namespace soo::detail;

struct A {
    auto foo() const volatile && { return std::string(__PRETTY_FUNCTION__) + "\n"; }
    auto bar() & { return std::string(__PRETTY_FUNCTION__) + "\n"; }
};

int main()
{
    auto mfp1 = &std::string::c_str;
    auto mfp2 = &A::foo;
    auto mfp3 = &A::bar;
    
    A* a;
    const volatile A a2;
    
    using ThisType1 = soo::this_type_t<decltype(mfp1)>;
    using ThisType2 = soo::this_type_t<decltype(mfp2)>;
    using ThisType3 = soo::this_type_t<decltype(mfp3)>;
    
    soo::print_type<
      ThisType1, // const std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >
      ThisType2, // const volatile A&&
      ThisType3  // A&
    >("\n\n");
    
    std::cout << (bind_this<ThisType1>(new std::string("hello\n") ).*mfp1) () // hello
              << (bind_this<ThisType2>(A{} ).*mfp2) ()                        // auto A::foo() const volatile &&
              << (bind_this<ThisType3>(soo::ref(*new A) ).*mfp3) ()  // auto A::bar() &
              << (bind_this<ThisType3>(soo::ref(a) ).*mfp3) ()      // auto A::bar() &
              
              // no matching function for call to `bind_this(const volatile A&)`
              // << (bind_this<ThisType3>(a2).*mfp3) ();  
              
              // error: pointer-to-member-function type ‘std::__cxx11::basic_string (A::*)() const volatile &&’ requires an rvalue
              // << (bind_this<ThisType2>(soo::ref(a) ).*mfp2) (); 
              
              << (bind_this<ThisType2>(std::move(a2) ).*mfp2) ();  // auto A::foo() const volatile &&
}
```

</td></tr></table> 



<table><tr><td>

## soo::detail::storage_size_impl
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<size_t S1, size_t S2, bool Cond>
struct storage_size_impl;
```
``S1`` 보다 크거나 같은 2의 제곱수 ``result`` 를 정의하며, ``constexpr static size_t soo::function<Ret(Args...)>::storage_size`` variable template 을 초기화하는데 사용됩니다. 단순히 정적 변수 하나를 초기화하는데 사용되므로, helper variable 은 없습니다.

### Template parameters
``S1`` - 2의 제곱으로 만들어줄 크기. <br>
``S2`` - ``result`` 의 최솟값. 항상 2의 제곱이어야 합니다. <br>
``Cond`` - ``S1 > S2`` 인지 여부입니다. ``S1`` 가 ``S2`` 보다 크지 않다는 것은, ``S2`` 가 ``S1`` 와 같거나 작은 2의 제곱수라는 의미입니다.

### Member variable
``result`` - ``S1`` 보다 크거나 같은 2의 제곱 수 ``S2``

### Example
``` c++
# include"myfunctional.hpp"

int main()
{
  std::cout << soo::detail::storage_size_impl<24,64, (24>64)>::result; // 64
  std::cout << soo::detail::storage_size_impl<48,1024,(48>1024)>::result; // 1024
}
```

</td></tr></table> 

<table><tr><td>

## soo::bind
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<typename Functor, typename...Args>
constexpr auto bind(Functor&& ftor, Args&&...args) requires (!MFP<Functor>);   (1)
```
``` c++
template<MFP Functor, typename Class, typename...Args>
constexpr auto bind(Functor&& mfp, Class&& pthis, Args&&...args);   (2)
```
함수 객체와 호출에 필요한 인자들을 묶어주는 함수입니다. ``soo::bind`` 함수는 단순히 `ftor` 와 ``pthis``, ``...args`` 를 복사한 ``std::bind::<lambda()>``를 반환하기만 합니다. 즉, ``pthis`` 와 ``...args`` 의 타입이 올바른지, 넘겨줄 인자가 너무 많은지 적은지는 ``std::bind::<lambda()>::operator()`` 를 호출하기 전까지 검사하지 않습니다. 잘못된 값으로 바인딩했다면, ``operator()`` 를 호출하는 시점에서 항상 에러가 발생하게 됩니다. 함수에 전달할 값의 타입은 ``soo::bind`` 에 전달했던 그대로 전달을 해줍니다. 즉, ``int&&`` 으로 전달했다면 항상 ``int&&`` 타입으로 전달합니다. 

1 ) ``Functor`` 가 member function pointer 가 아닌 이외의 functoin object 이여야 합니다. ``...args`` 는 ``ftor`` 와 함께 묶어줄 함수의 인자들입니다. ``soo::bind`` 를 통해 바인딩한 함수 객체의 호출 인자의 갯수는 ``soo::detail::bind_num<Args...>`` 와 같습니다.    <br><br>
2 ) ``Functor`` 가 member function pointer 이어야 합니다. ``pthis``, ``...args`` 는 ``ftor`` 와 함께 묶어줄 함수의 인자들입니다. ``soo::bind`` 를 통해 바인딩한 함수 객체의 호출 인자의 갯수는 ``soo::detail::bind_num<Args...>`` 와 같습니다. 
### Template parameter
**Functor** - any function objects. <br>
**Class** - member function pointer 가 가리키는 함수가 소속되어 있는 class 의 타입. <br>
**...Args** - ``Functor`` 호출 시에 전달해줄 인자들의 타입. 

### Parameters
**ftor** - member function pointer 가 아닌 이외의 function object. <br>
**mfp** - member function pointer <br>
**pthis** - ``mfp`` 호출 시에 넘겨줄 ``this`` 로 변환될 수 있는 class object. <br>
**...args** - ``ftor`` 와 바인딩하고자 하는 인자들.  

### Return value
1 ) ``ftor``, ``...args`` 의 값을 복사한 ``m_ftor``, ``...m_args`` 를 data member 로 갖고, template parameter pack 을 가진 ``operator()`` 를 member function 으로 갖는  ``soo::bind::<lambda()>`` 객체. <br>
2 ) ``ftor``, ``pthis``, ``...args`` 의 값을 복사한 ``m_mfp``, ``m_this``, ``...m_args`` 를 data member 로 갖고, template parameter pack 을 가진 ``operator()`` 를 member function 으로 갖는 ``soo::bind::<lambda()>`` 객체.

### Example
``` c++
# include"myfunctional.hpp"
# include<string>
using namespace soo::placeholders;

int sum(int a, int b) {
    return a + b;
}

struct Adder {
    int operator()(int a, int b) {
        return a + b;
    }
};

int main()
{
    auto println = [](auto&& a){ std::cout << a << std::endl; };
    
    auto fn1 = soo::bind(
        [](int a, int b) { return a + b; }, _1, _2  // lambda expression.
    );
    auto fn2 = soo::bind(sum, _1,_2);   // static function.
    auto fn3 = soo::bind(Adder{}, _1,_2);    // object which have `operator()`.
    auto fn4 = soo::bind(&Adder::operator(), Adder{}, _1,_2); // member function.
    
    println(fn1(1,2) ); // 3
    println(fn2(3,4) ); // 7
    println(fn3(5,6) ); // 11
    println(fn4(7,8) ); // 15
    
    // fn1(1,2,3); // error: static assertion failed: too many arguments to operator()
    // fn2(3);     // error: static assertion failed: too few arguments to operator()
    
    soo::function<int()> fns[4] = {
        soo::bind(fn1,1,2),
        soo::bind(fn2,3,4),
        soo::bind(fn3,5,6),
        soo::bind(fn4,7,8)
    };
    
    for(auto& f : fns) {
        println(f() ); // 3 7 11 15
    }
    
    
    auto fn5 = soo::bind(&std::string::c_str, std::string("hello") ); // std::string(const std::string&) is called.
    println(fn5() );  // hello
    
    auto fn6 = soo::bind(&std::string::c_str, 10); // OK. fn6::operator()().
                                                   // but `10` is not std::string. 
    // fn6(); // error: `10` is not convertible to `std::string`. 
    // fn6(std::string{"hello"} ); // error: static assertion failed: too many arguments to operator().
                                   // sizeof...(fn6::operator()::...args) must be 0.
    
    auto fn7 = soo::bind(&std::string::c_str, _3); // OK.
    // println(fn7(1,2,3) ); // error: `3` is not convertible to std::string
    println(
        fn7(1,2,std::string{"world!"} ) // world! 
    );
}
```
</td></tr></table> 



<table><tr><td>

## soo::function
<sub> Defined in "myfunctional.hpp"</sub>
``` c++
template<typename T>
class function;
```
``` c++
template<typename Ret, typename...Args>
class function<Ret(Args...)>;
```
``Static function``, ``Member function``, ``Lambda expression``, ``An objects which have operator()`` 와 같이 타입이 달라도 <br>
호출 방법이 ``Ret(Args...)`` 와 같다면, 모두 담을 수 있는 general-purpose polymorphic function object 입니다. <br>
사용 방법은 ``std::function`` 과 같습니다. <br>
1 ) ``T`` 가 **가변 인자를 가진 function**, **member function type**, **non-function** 타입일 경우에 컴파일 에러를 일으킵니다. <br>
    반드시 return type 과 호출에 필요한 인자들의 타입만을 명시해주어야 합니다. <br>
2 ) 호출 방식이 ``Ret(Args...)`` 인 함수 객체를 담는 컨테이너 class 입니다. 

### Template parameter
**Ret** - return type. <br>
**...Args>** - function argument types. 

### Member type
``Operation`` - 

### Data members
모두 ``private`` 입니다.
- ``m_bufptr`` - 현재 target 이 담겨 있는 storage 의 시작주소를 가리키는 포인터입니다. <br>
- ``m_invoke`` - ``function<Ret(Args...)>::invoke`` 함수의 주소를 가리키는 포인터입니다. <br>
- ``m_manager`` - ``function<Ret(Args...)>::m_manager`` 함수의 주소를 가리키는 포인터입니다. <br>
- ``m_buf_local`` - 크기가 8과 같거나 작은 target 을 담아두는 local storage 입니다. <br>
- ``m_capacity`` - dynamic storage 를 사용 중이라면 활성화됩니다. 할당한 dynamic storage 의 용량을 저장합니다. <br>

### Member functions
- ``alloc (private)`` - dynamic storage 를  할당합니다. 
- ``(constructor)`` - 생성자.
- ``(destructor)`` - 소멸자.
- ``operator=`` - 새로운 target 을 넣어줍니다.
- ``operator()`` - target 을 invoke 시킵니다.
- ``swap`` - 두 ``function<Ret(Args...)>`` 의 target 을 교환합니다.
- ``target_type`` - 현재 담고 있는 target 의 ``std::type_info`` 를 얻습니다.
- ``target`` - 현재 담고 있는 target 을 가리키는 포인터입니다.
- ``operator bool()`` - ``*this`` 의 target 이 있는지를 확인합니다.

<table><tr><td>

## soo::function<Ret(Args...)>::alloc
``` c++
template<size_t FunctorSize>
void alloc();   
```
dynamic storage 를 할당합니다. 할당하는 버퍼의 크기는 항상 64 의 배수이며, 할당한 크기만큼 정렬되어 있습니다. <br>
한번도 dynamic storage 를 할당한 적이 없거나, ``m_capacity`` 가 ``FunctorSize`` 보다 작은 경우에만 할당이 이루어 집니다. <br>
그 이외의 경우, 이미 충분한 크기의 dynamic storage 를 가지고 있다고 판단합니다. 

### Template parameter
**FunctorSize** - 할당할 객체의 크기.

### Parameters
(none)

### Return value
(none)
</td></tr></table> 
<table><tr><td>

## soo::function<Ret(Args...)>::function
``` c++
function(); (1)
```
``` c++
function(std::nullptr_t); (2)
```
``` c++
function(const function& other); (3)
```
``` c++
function(function&& other); (4)
```
``` c++
template<NotEqual<function> Functor>
function(Functor&& ftor) requires Callable<Ret,Functor,Args...>  (5)
```
1,2 ) default constructor. 비어있는 function object 를 만듭니다. 이 상태에서 ``operator()`` 를 호출한다면, <br>
      ``soo::bad_functoin_call`` exception 이 발생합니다. <br><br>
3 ) copy constructor. 깊은 복사를 통해 target 을 복사해옵니다. <br><br>
4 ) move constructor. ``other`` target 의 memory block 의 소유권을 넘겨받습니다. <br>
    즉, ``other`` 의 dynamic storage 은 해당 공간을 가리키는 포인터만 복사가 이루어집니다. <br> 
    이후, ``other`` 은 비어있는 functon objects 가 되며, dynamic storage 를 사용하기 전 상태가 됩니다. <br>
    이 과정에서 target 의 move constructor 가 호출되지 않습니다. <br><br>
5 ) 함수 객체 ``ftor`` 로 직접 target 을 넣어주는 것으로 초기화합니다. ``ftor`` 가 자기 자신일 수는 없습니다. <br>
    ``NotEqual<function>`` 제약이 없다면, 구현 상 무한 재귀가 일어나게 됩니다.<br>
    ``ftor`` 는 ``Args...`` 를 인자로 갖고 호출이 가능해야 하며, 호출 결과가 ``Ret`` 로 변환될 수 있어야 합니다. <br>
    ``std::remove_reference_t<Functor> <= 8`` 이라면 ``function<Ret(Args...)`` 안에 있는 local storage 를 사용하게 되지만, <br>
    ``std::remove_reference_t<Functor> > 8`` 이라면 dynamic storage 를 사용하게 됩니다. <br>

### Parameters
**other** - 초기화하는데 사용할 ``function<Ret(Args...)>`` 객체. <br> 
**ftor** - invoke 방법이 ``Ret(Args...)`` 인 function object.

### Example
``` c++
# include"myfunctional.hpp"
# include<string>
using namespace soo::placeholders;

struct Adder {
    int a, b;
    
    // default constructor
    Adder() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    
    // move constructor
    Adder(Adder&& other) : a(other.a), b(other.b) { 
        std::cout << __PRETTY_FUNCTION__ << std::endl; 
        other.a = other.b = 0;
    }
    
    // copy constructor
    Adder(const Adder& other) : a(other.a), b(other.b) { 
        std::cout << __PRETTY_FUNCTION__ << std::endl; 
    }
    
    // other constructor
    Adder(int a, int b) : a(a), b(b) {
        std::cout << __PRETTY_FUNCTION__ << std::endl; 
    }
    
    // destructor
    ~Adder() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    
    // function call operator
    int operator()() const {
        return a + b;
    }
};

template<typename F>
void var_dump(const F& f) {
    alignas(alignof(F)) uint64_t buf[sizeof(F)/8];
    static_assert(sizeof(buf)==sizeof(F) );
    
    memcpy(buf,&f, sizeof(F) );
    std::cout << std::hex << &f << ": \n";
    for(int i=0; i<sizeof(F)/8; ++i) {
        std::cout << buf[i] << ' ';
    }
    std::cout << "\n\n" << std::dec;
}


int main()
{
    using Delegate = soo::function<const char*()>;
    using Delegate2 = soo::function<int()>;
    std::cout << std::boolalpha;
    
    auto println = [](auto&&...args) { ((std::cout << args << ' '),...) << std::endl; };
    Delegate fn1 = soo::bind(&std::string::c_str, std::string("hello world,") ), // function(Functor&&)
             fn2 = fn1,     // function(const function&)
             fn3 = nullptr; // function(std::nullptr)
             
                   //    address        m_bufptr     m_invoke     m_manager  m_capacity
    var_dump(fn1); // 0x7ffcbd182800: 555555e91940 7f2dcab36500 7f2dcab36580 40
    var_dump(fn2); // 0x7ffcbd182820: 555555e91a00 7f2dcab36500 7f2dcab36580 40
    var_dump(fn3); // 0x7ffcbd182840: 7ffcbd182858 0            0            2c646c72
    println(fn1(), fn2(), fn3); // hello world hello world false
    
    //fn3(); // terminate called afther throwing an instance of `soo::bad_function_call`
             //   what():  bad function call
    
    Delegate fn4 = std::move(fn2); // function(function&&)
    
                   //    address        m_bufptr     m_invoke     m_manager  m_capacity
    var_dump(fn2); // 0x7ffcbd182820: 7ffcbd182838   0            0          40 
    var_dump(fn4); // 0x7ffcbd182860: 555555e91a00 7f2dcab36500 7f2dcab36580 40
    println(fn4() ); // hello world,
    
    Adder* adder = new Adder{1,2};
    Delegate2 fn5 = *adder; // Adder::Adder(int, int)  
                            // Adder::Adder(const Adder&)  in function(Functor&&)
    Delegate2 fn6 = Adder{3,4}; // Adder::Adder(int,int)
                                // Adder::Adder(Adder&&) in function(Functor&&)
                                // Adder::~Adder()
    Delegate2 fn7 = soo::ref(*new Adder{5,6} ); // Adder::Adder(int,int)
    
                                    
    Delegate2 fn8 = std::move(fn7); // function(function&&)
    
                   //    address        m_bufptr     m_invoke     m_manager  m_buf_local
    var_dump(fn5); // 0x7ffc19492350: 7ffc19492368 7f4c0d198b60 7f4c0d198d70 200000001
    var_dump(fn6); // 0x7ffc19492370: 7ffc19492388 7f4c0d198b70 7f4c0d198d70 400000003
    var_dump(fn7); // 0x7ffc19492390: 7ffc194923a8 0            0            555555a0fac0
    var_dump(fn8); // 0x7ffc194923b0: 7ffc194923c8 7f4c0d198b80 7f4c0d198b90 555555a0fac0
    println(fn5(), fn6(), fn7, fn8() ); // 3 7 false 11
    
    // Adder::~Adder()
    // Adder::~Adder()
    delete adder; // Adder::~Adder()
    delete &fn8.target<
      soo::reference_wrapper<Adder>
    >()->get(); // Adder::~Adder()
}
```
</td></tr></table> 
<table><tr><td>

## soo::function<Ret(Args...)>::~function
``` c++
function<Ret(Args...)>::~function();
```
target 이 존재한다면, target 의 소멸자를 호출해줍니다. 만약, dynamic storage 를 할당했었다면 이후 이 공간을 해제합니다.

</td></tr></table> 

<table><tr><td>

## soo::function<Ret(Args...)>::operator=
``` c++
template<NotEqual<function> Functor>
function& operator=(Functor&& ftor) requires Callable<Ret,Functor,Args...>;  (1)
```
``` c++
function& operator=(const function& other);  (2)
```
``` c++
function& operator=(function&& other);  (3)
```
``` c++
function& operator=(std::nullptr_t);  (4)
```
``function<Ret(Args...)>`` 객체에 새로운 target 을 넣어줍니다. 만약 ``function<Ret(Args...)>`` 객체의 target 이 비어있는 상태가 아니라면, <br>
먼저 이전 target 의 소멸자를 호출해줍니다. 

1 ) ``ftor`` 으로 현재 target 을 변경합니다. ``NotEqual<function>`` 제약이 없다면 구현 상 무한 재귀가 일어나게 됩니다. <br>
    ``ftor`` 은 ``Args...`` 으로 호출이 가능해야 하며, 호출 결과가 ``Ret`` 로 변환이 가능해야 합니다. <br><br>
2 ) copy constructor. ``other`` 에 있는 내용을 깊은 복사해옵니다. <br><br>
3 ) move constructor. ``other`` 의 memory block 의 소유권을 넘겨받습니다. 이 과정에서 target 의 move constructor 가 호출되지 않습니다.  <br><br>
4 ) ``*this`` 의 현재 target 을 비웁니다. 이 과정이 dynamic storage 를 해제하는 것을 의미하지 않습니다.  <br><br>
### Parameters
**other** - 초기화하는데 사용할 ``function<Ret(Args...)>`` 객체. <br> 
**ftor** - invoke 방법이 ``Ret(Args...)`` 인 function object.

### Return values
``*this``. 

</td></tr></table> 

<table><tr><td>

## soo::function<Ret(Args...)>::operator()
``` c++
Ret operator()(Args...args) const;
```
현재 target 을 ``...args`` 를 넘겨주는 것으로 invoke 시킵니다. 만약, target 이 비어있다면 ``soo::bad_function_call`` 예외가 throw 됩니다.

### Parameters
**...args** - 현재 target 을 invoke 하는 데 필요한 인자들.

### Return value
``Ret``

### Example
``` c++
# include"myfunctional.hpp"

int sum(int a, int b) {
    return a + b;
}

int main()
{
    soo::function<int(int,int)> fn = nullptr;
    
    // fn(1,2); // terminate called after throwing an instance of 'soo::bad_function_call'
                //   what():  bad function call
                
    fn = sum;
    std::cout << fn(1,2); // 3
}
```
</td></tr></table> 

<table><tr><td>

## soo::function<Ret(Args...)>::swap
``` c++
void swap(function& other);
```
``*this`` 와 ``other`` 의 target 을 교환합니다. 단순히, 임시 객체 ``temp`` 와 ``function(function&&)``, ``operator(function&&)`` 을 이용하여 교환합니다.

### Parameter
**other** - target 을 교환할 ``function<Ret(Args...)>`` 객체.

### Return value
(none)

### Example
``` c++
# include"myfunctional.hpp"
# include<string>
using namespace soo::placeholders;

struct Adder {
    int a, b;
    int operator()() const {
        return a + b;
    }
};

template<typename F>
void var_dump(const F& f) {
    alignas(alignof(F)) uint64_t buf[sizeof(F)/8];
    static_assert(sizeof(buf)==sizeof(F) );
    
    memcpy(buf,&f, sizeof(F) );
    std::cout << std::hex << &f << ": \n";
    for(int i=0; i<sizeof(F)/8; ++i) {
        std::cout << buf[i] << ' ';
    }
    std::cout << "\n\n" << std::dec;
}


int main()
{
    auto println = [](auto&&...args) { ((std::cout << args << ' '),...) << std::endl; };
    soo::function<const char*()> fn1 = soo::bind(&std::string::c_str, std::string("hello") ),
                                 fn2 = soo::bind(&std::string::c_str, std::string("world") );
                   //    address         m_bufptr    m_invoke     m_manager  m_capacity
    var_dump(fn1); // 0x7ffea1770f20: 5555567f9940 7fe2050a7520 7fe2050a75a0 40
    var_dump(fn2); // 0x7ffea1770f40: 5555567f9a00 7fe2050a7520 7fe2050a75a0 40
    println(fn1(), fn2() ); // hello world
    
    fn1.swap(fn2); 
    
                   //    address         m_bufptr    m_invoke     m_manager  m_capacity
    var_dump(fn1); // 0x7ffea1770f20: 5555567f9a00 7fe2050a7520 7fe2050a75a0 40
    var_dump(fn2); // 0x7ffea1770f40: 5555567f9940 7fe2050a7520 7fe2050a75a0 40
    println(fn1(), fn2() ); // world hello
    
    
    soo::function<int()> fn3 = Adder{1,2},
                         fn4 = Adder{3,4};
                         
                   //    address         m_bufptr    m_invoke     m_manager  m_buf_local
    var_dump(fn3); // 0x7fffcf135a20: 7fffcf135a38 7f86b70d3840 7f86b70d3850 200000001
    var_dump(fn4); // 0x7fffcf135a40: 7fffcf135a58 7f86b70d3840 7f86b70d3850 400000003
    println(fn3(), fn4() ); // 3 7
    
    fn3.swap(fn4);
    
                   //    address         m_bufptr    m_invoke     m_manager  m_buf_local
    var_dump(fn3); // 0x7fffcf135a20: 7fffcf135a38 7f86b70d3840 7f86b70d3850 400000003
    var_dump(fn4); // 0x7fffcf135a40: 7fffcf135a58 7f86b70d3840 7f86b70d3850 200000001
    println(fn3(), fn4() ); // 7 3
}
```
</td></tr></table> 

<table><tr><td>

## soo::function<Ret(Args...)>::target_type
``` c++
const std::type_info& target_type() const;
```
현재 ``*this`` 가 담고 있는 target 의 타입 정보를 반환합니다. 

### Parameter
(none)

### Return value
target 이 있다면 ``typeid(target)``. 만약, target 이 비어있다면 ``typeid(void)``.

### Example
``` c++

# include"myfunctional.hpp"
# include<string>
using namespace soo::placeholders;

struct Adder {
    int a, b;
    
    // destructor
    ~Adder() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    
    // function call operator
    int operator()() const {
        return a + b;
    }
};


int main()
{
    soo::function<const char*()> fn = soo::bind(&std::string::c_str, std::string("hello world") );
    soo::function<const char*(const std::string&)> fn2 = &std::string::c_str;
    soo::function<int()> fn3 = Adder{1,2};
    
    // ZN3soo4bindIMNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEKDoFPKcvES6_JEEEDaOT_OT0_DpOT1_EUlDpOT_E_
    std::cout << fn.target_type().name() << std::endl;
    
    // MNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEKDoFPKcvE
    std::cout << fn2.target_type().name() << std::endl;
    
    std::cout << fn3.target_type().name() << std::endl; // 5Adder
    
    fn = nullptr;
    std::cout << fn.target_type().name() << std::endl; // v
    // Adder::~Adder()
}
```
</td></tr></table> 

<table><tr><td>

## soo::function<Ret(Args...)>::target
``` c++
template<typename T>
constexpr auto target() const;
```
``*this`` 에 저장되어 있는 target 의 시작 주소 값을 얻어옵니다.

### Template parameter
**T** - target type.

### Parameter
(none)

### Return value
현재 target 이 존재한다면, target 이 저장되어 있는 곳의 시작 주소를 얻습니다. 만약, target 이 비어있는 경우라면 ``nullptr`` 를 얻게 됩니다.

### Example
``` c++

# include"myfunctional.hpp"

struct Adder {
    int a, b;
    
    // default constructor
    Adder() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    
    // move constructor
    Adder(Adder&& other) : a(other.a), b(other.b) { 
        std::cout << __PRETTY_FUNCTION__ << std::endl; 
        other.a = other.b = 0;
    }
    
    // copy constructor
    Adder(const Adder& other) : a(other.a), b(other.b) { 
        std::cout << __PRETTY_FUNCTION__ << std::endl; 
    }
    
    // other constructor
    Adder(int a, int b) : a(a), b(b) {
        std::cout << __PRETTY_FUNCTION__ << std::endl; 
    }
    
    // destructor
    ~Adder() { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    
    // function call operator
    int operator()() const {
        return a + b;
    }
};


int main()
{
    soo::function<int()> fn3 = Adder{1,2}; // Adder::Adder(int,int)
                                           // Adder::Adder(Adder&&)
                                           // ~Adder::Adder()
    std::cout << fn3() << std::endl; // 3
    
    fn3.target<Adder>()->a = 10;
    fn3.target<Adder>()->b = 20;
    std::cout << fn3() << std::endl; // 30
    // Adder::~Adder()
}
```
</td></tr></table> 

<table><tr><td>

## soo::function<Ret(Args...)>::operator bool()
``` c++
operator bool() const noexcept;
```
``*this`` 의 target 이 비어있는지를 확인합니다.

### Parameter
(none)

### Return value
target 이 있으면 ``true``, 이외의 경우는 ``false``.

### Example
``` c++
# include"myfunctional.hpp"

int main()
{
    std::cout << std::boolalpha;
    soo::function<int()> fn = nullptr;
    
    std::cout << fn << std::endl; // false
    fn = soo::bind(printf,"hello world");
    std::cout << fn << std::endl; // true
}
```

</td></tr></table> 

### Non-member functions

- ``invoke  (private)`` - 현재 target 을 invoke 시킵니다.
- ``manager (private)`` - 현재 target 의 타입을 알고 있으며, target 의 소멸자 호출 및 복사 생성자 호출을 담당합니다.

<table><tr><td>

## soo::function<Ret(Args...)>::invoke
``` c++
template<MFP Functor, typename Class, typename...Params>
static Ret invoke(function& fn, Class&& pthis, Params&&...params); (1)
```
``` c++
template<typename RawFunctor, typename Functor>
static Ret invoke(function& fn, Args&&...args) requires (!MFP<RawFunctor>);  (2)
```
``*this`` 의 target 이 있다면, ``function<Ret(Args...)>::operator()`` 에서 호출하는 함수입니다. target 을 invoke 시킵니다. <br>
1 ) target 이 member function pointer 일 때, invoke 시키는 함수입니다. <br>
2 ) target 이 member function pointer 가 아닌 이외의 function object 일 때, invoke 시키는 함수입니다.

### Template parameters
**Functor** - 현재 target 의 원래 타입. <br>
**Class** - ``Functor`` 가 member function pointer 일 때, ``'this'`` 에 해당하는 타입. <br>
**...Params** - ``...Args`` 에서 ``'this'`` 에 해당하는 부분을 뺀 나머지 인자들의 타입. <br>
**RawFunctor** - ``std::remove_reference_t<Functor>``.

### Parameters
**fn** - ``*this`` <br>
**pthis** - ``fn`` 이 member function pointer 일 때, ``'this'`` 객체. <br>
**...params** - ``fn`` 이 member function pointer 일 때, ``'this'`` 를 뺀 나머지 인자들. <br>
**...args** - ``fn`` 이 member function pointer 가 아닐 떄, 호출인자들.<br>

### Return value
``Ret``

</td></tr></table>

<table><tr><td>

## soo::function<Ret(Args...)>::manager
``` c++
template<size_t FunctorSize, typename Functor>
static void manager(const function& fn, void* out, Operation op);
```
``fn`` 의 target 의 타입을 알고 있는 target 관리 함수입니다. ``op`` 로 명시한 연산을 수행합니다. 

### Template parameters
**FunctorSize** - 현재 target 의 크기 <br>
**Functor** - 현재 target 의 타입 <br>

### Parameters
**fn** - ``*this`` <br>
**out** - ``op`` 의 연산 결과를 담을 객체의 포인터. <br>
**op** - ``soo::function<Ret(Args...)>::Operation`` 에 명시되어있는 연산 중 하나.

### Return value
(none)

</td></tr></table>

</td></tr></table> 










