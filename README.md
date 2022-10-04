# myfunctional
My implementation of std::function, std::bind in c++20

표준 라이브러리에서 지원하는 ``std::function`` 과 ``std::bind`` 를 구현해봤습니다. 

<table><tr><td>

## soo::always_false
<sub>Defined in header "mytype_traits.hpp"</sub>
```c++
template<typename T>
constexpr bool always_false = false;  
```
항상 ``false`` 값을 갖는 bool 타입의 variable template 입니다. ``static_assert`` 와 함께 사용하여, 원하지 않는 class template 의 인스턴스화를 컴파일 시간에 방지할 수 있습니다. 

### Template paramters
<strong>T</strong> - template instantiation 에 필요한 타입.
</td></tr></table>
<table><tr><td>

## soo::decay_function
<sub>Defined in header "mytype_traits.hpp"</sub>
```c++
template<typename T>
struct decay_function; 
```
T 가 non-member function 이라면, function pointer 로 변경합니다. 그 이외의 타입일 경우, 결과는 T 그대로 입니다. 

### Member type
<strong>type</strong> - static function pointer 또는 T.

### Template parameters
<strong>T</strong> - decay 시킬 non-member function 타입. 

### Helper type
```c++
template<typename T>
using decay_function_t = typename decay_function<T>::type;
```

### Example
``` c++
# include"mytype_traits.hpp"
int main() {
  soo::print_type<
    soo::decay_function_t<int() const volatile && noexcept>, 
    soo::decay_function_t<int()>,
    soo::decay_function_t<int>
  >();  
  // int() const volatile && noexcept, int(*)(), int
}
```
</td></tr></table>

<table><tr><td>

## soo::print_type
<sub>Defined in header "mytype_traits.hpp"</sub>
``` c++
template<typename...Tn>
void print_type(const char* endstr="");
```
``#if defined(__GNUC__)`` 가 ``true`` 일 경우에만, 사용할 수 있습니다. 템플릿 인자로 받은 타입들을 stdout 으로 출력합니다. 타입을 출력한 후에는 ``endstr`` 으로 받은 multibyte character string 을 stdout 으로 출력합니다.

### Template parameter
<strong>Tn...</strong> - 출력할 타입들

### Parameters
<strong>endstr</strong> - 마지막에 추가로 출력할 mutibyte character string. 아무 값도 주지 않는다면, 비어있는 문자열을 사용합니다.

### Return value
(none)

### Example
``` c++
# include"mytype_traits.hpp"
int main() {
   soo::print_type<
     decltype(std::cout), float, double    // std::basic_ostream<char, std::char_traits<char> >, float, double
   >("\n\n");
}
```
</td></tr></table>

<table><tr><td>

## soo::find_type
<sub>Defined in header "mytype_traits.hpp"</sub>
``` c++
template<typename T, typename...Tn>
struct find_type;
```
``Tn...`` template parameter pack 에서, ``T`` 가 있는지를 검사합니다. ``Tn`` 안에서 ``T`` 가 존재한다면, member constant 의 값은 ``true``. 그 이외의 경우에는 ``false`` 입니다.


### Template parameters
<strong>T</strong> - parameter pack 에서 찾고자 하는 타입.<br>
<strong>Tn...</strong> - 검사할 parameter pack.

### Helper variable template
``` c++
template<typename T, typename...Tn>
constexpr bool find_type_v = find_type<T,Tn...>::result;
```
### Example
``` c++
# include"mytype_traits.hpp"
int main() {
  std::cout << soo::find_type_v<int, float,double,long, size_t, int> // 1
            << soo::find_type_v<int&&, int&, const int&>; // 0
}
```
</td></tr></table>

<table><tr><td>

## remove_const_ref
<sub>Defined in header "mytype_traits.hpp"</sub>
``` c++
template<typename T>
struct remove_const_ref;
```
``T`` 에 ``std::remove_reference_t`` 와 ``std::remove_const_t`` 를 순서대로 적용합니다.
### Member type
<strong>type</strong> - ref 와 c qualifier 를 제거한 타입 T.

### Helper Type
``` c++
template<typename T>
using remove_const_ref_t = typename remove_const_ref<T>::type;
```
### Example
```c++
# include"mytype_traits.hpp"
int main() {
  soo::print_type<
    soo::remove_const_ref_t<const volatile int&&> 
  >();
  // volatile int
}
```
</td></tr></table>

<table><tr><td>

## this_type
<sub>Defined in header "mytype_traits.hpp"</sub>
``` c++
template<typename T>
struct this_type;
```
``T`` 가 member function pointer 라면, 그 member function pointer 를 호출하는데 가장 적합한 Class 의 타입을 얻습니다. 만약, ``T`` 가 member function pointer 가 아니라면, 컴파일 타임 에러를 발생시킵니다.

### Member type
<strong>type</strong> - member function pointer 를 호출하는데 가장 적합한 Class 의 타입.

### Helper type
``` c++
template<typename T>
using this_type_t = typename this_type<T>::type;
```
### Example
```c++
# include"mytype_traits.hpp"
struct A {
  void foo()const volatile && noexcept {}
  void goo(){}
};

int main() {
    soo::print_type<
      soo::this_type_t<decltype(&A::foo)>,
      soo::this_type_t<decltype(&A::goo)>
    >();
    // const volatile A&&, A
}
```
</td></tr></table>

<table><tr><td>

## is_function_reference
<sub>Defined in header "mytype_traits.hpp"</sub>
```c++
template<typename T>
struct is_function_reference;
```
``T`` 가 non-member 함수의 reference 라면,  member constants 의 값은 ``true``. 아니라면 ``false`` 입니다.
### Template parameters
<strong>T</strong> - 함수의 reference 인지를 검사할 타입 T.

### Helper variable template
``` c++
template<typename T>
constexpr bool is_function_reference_v = is_function_reference<T>::result;
```
### Example
```c++
# include"mytype_traits.hpp"
int main() {
  std::cout << is_function_reference_v<int(&)()> // 1
            << is_function_reference_v<int()const>; // 0
}
```
</td></tr></table>


<table><tr><td>

## __IS_PLACEHOLDER
<sub>Defined in header "myfunctional.hpp"</sub>
``` c++
# define __IS_PLACEHOLDER(N)  
```
``soo::bind_arg`` 에서 사용하는 helper macro 입니다. ``t0`` 이 ``const Placeholder<N>`` 인지를 확인합니다.

### Parameters
<strong>N</strong> - 확인할 placeholder 의 번호. N 은 항상 `` 1 <= N <= 7`` 을 만족해야 하며, 해당하는 Placeholder 변수가 존재하지 않으면 컴파일 에러입니다.

### Expanded results
``` c++
std::is_same_v<const placeholders::Placeholder<N>,
               std::remove_reference_t<decltype(t0)> >  
```

</td></tr></table>


<table><tr><td>

## __FORWARD
<sub>Defined in header "myfunctional.hpp"</sub>
``` c++
# define __FORWARD(tn)
```
``std::forward`` 의 macro 버전입니다. template 인자가 필요한 기존의 함수와 다르게, 수식만을 이용하여 캐스팅시킵니다.

### Parameters
<strong>tn</strong> - forwarding 할 수식.

### Expanded results
``` c++
(decltype(tn) ) tn
```

</td></tr></table>

<table><tr><td>

## __T0
<sub>Defined in header "myfunctional.hpp"</sub>
``` c++
# define __T0
```
``soo::bind_arg`` 에서 사용되는 helper macro 입니다. t0 의 ``const`` qualifier 를 제거합니다. 
### Parameters
(none)

### Expanded results
``` c++
const_cast<remove_const_ref_t<decltype(t0)>&>(t0)
```

</td></tr></table>


<table><tr><td>

## __M_FTOR
<sub>Defined in header "myfunctional.hpp"</sub>
``` c++
# define __M_FTOR
```
``soo::bind`` 에서 사용하는 helper macro 입니다. functor 의 ``const`` qualifier 를 제거한 후, ``soo::bind`` 에서 받았던 타입 그대로 되돌립니다. 만약, functor 가 함수의 레퍼런스였다면, 함수의 포인터로 캐스팅해줍니다.
### Parameters
(none)
### Expanded results
``` c++
((std::conditional_t<is_function_reference_v<Functor&&>, \
                     RawFunctor,                         \
                     Functor&&>) *const_cast<RawFunctor*>(&m_ftor))
```

</td></tr></table>


<table><tr><td>

## __INVOKE_MEM_FN
<sub>Defined in header "myfunctional.hpp"</sub>
``` c++
# define __INVOKE_MEM_FN(...)
```
``soo::bind`` 에서 사용하는 helper macro 입니다. member function 호출을 위한 this pointer 와 인자들을 올바르게 바인딩 시킨 후, 멤버 함수를 호출합니다.

### Parameters
<strong>...</strong> - ``bind_arg`` 에서 placeholder 들과 매칭되는 ``<lambda()>::operator()`` 의 인자들.
### Expanded results
``` c++
(bind_this<ThisType>(                                     \
   bind_arg<Class>(__FORWARD(thisptr), __VA_ARGS__)       \
 ).*mfp) (bind_arg<Args>(__FORWARD(args), __VA_ARGS__)...)
```

</td></tr></table>



<table><tr><td>

## bind_arg
<sub>Defined in header "myfunctional.hpp"</sub>
``` c++
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0);  (1)

template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1);  (2)

template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2);  (3)

template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3);  (4)

template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, auto&& t4);  (5)

template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                  auto&& t4, auto&& t5);  (6)
                                  
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                  auto&& t4, auto&& t5, auto&& t6);  (7)
                                  
template<typename Args>
constexpr decltype(auto) bind_arg(auto&& t0, auto&& t1, auto&& t2, auto&& t3, 
                                  auto&& t4, auto&& t5, auto&& t6, auto&& t7);  (8)
```
``soo::bind`` 의 구현에 사용되는 핵심 함수입니다. ``t0`` 을 올바른 인자로 바인딩합니다. <br>
 1) ``t0`` 을 현재 그대로 바인딩합니다.<br>
 2) ``t0`` 을 [t0,t1] 중 하나로 바인딩합니다.  <br>
 3) ``t0`` 을 [t0,t2] 중 하나로 바인딩합니다. <br>
 4) ``t0`` 을 [t0,t3] 중 하나로 바인딩합니다. <br>
 5) ``t0`` 을 [t0,t4] 중 하나로 바인딩합니다. <br>
 6) ``t0`` 을 [t0,t5] 중 하나로 바인딩합니다. <br>
 7) ``t0`` 을 [t0,t6] 중 하나로 바인딩합니다. <br>
 8) ``t0`` 을 [t0,t7] 중 하나로 바인딩합니다. 
  
### Template parameter
<strong>Args</strong> - ``soo::bind`` 에서 받았던 ``t0`` 의 원래 타입 Args.

### Parameters
<strong>t0</strong> - ``soo::bind`` 로 전달했던 인자 중 하나 <br>
<strong>t1</strong> - ``placeholders::_1`` 에 매칭시킬 값의 레퍼런스. <br>
<strong>t2</strong> - ``placeholders::_2`` 에 매칭시킬 값의 레퍼런스. <br>
<strong>t3</strong> - ``placeholders::_3`` 에 매칭시킬 값의 레퍼런스. <br>
<strong>t4</strong> - ``placeholders::_4`` 에 매칭시킬 값의 레퍼런스. <br>
<strong>t5</strong> - ``placeholders::_5`` 에 매칭시킬 값의 레퍼런스. <br>
<strong>t6</strong> - ``placeholders::_6`` 에 매칭시킬 값의 레퍼런스. <br>
<strong>t7</strong> - ``placeholders::_7`` 에 매칭시킬 값의 레퍼런스.

### Return values
``__IS_PLACEHOLDER(1)`` 가 ``true`` 라면, ``__FORWARD(t1)``. <br>
``__IS_PLACEHOLDER(2)`` 가 ``true`` 라면, ``__FORWARD(t2)``. <br>
``__IS_PLACEHOLDER(3)`` 가 ``true`` 라면, ``__FORWARD(t3)``. <br>
``__IS_PLACEHOLDER(4)`` 가 ``true`` 라면, ``__FORWARD(t4)``. <br>
``__IS_PLACEHOLDER(5)`` 가 ``true`` 라면, ``__FORWARD(t5)``. <br>
``__IS_PLACEHOLDER(6)`` 가 ``true`` 라면, ``__FORWARD(t6)``. <br>
``__IS_PLACEHOLDER(7)`` 가 ``true`` 라면, ``__FORWARD(t7)``. <br>
그 이외의 경우에는 ``(Args&&) (__T0)``

</td></tr></table>


<table><tr><td>

## bind_this
<sub>Defined in header "myfunctional.hpp"</sub>
``` c++
template<typename ThisType>
constexpr decltype(auto) bind_this(auto&& thisptr)
requires std::is_convertible_v<decltype(thisptr), std::remove_reference_t<ThisType>*>;  (1)

template<typename ThisType>
constexpr decltype(auto) bind_this(auto&& thisptr)
requires std::is_convertible_v<decltype(thisptr), ThisType>;  (2)
```
``soo::bind`` 에서 사용하는 helper function 입니다. member function 

### Template parameter
<strong>ThisType</strong> - member function pointer 호출을 위해서 가장 적합한 Class 의 타입.

</td></tr></table>










