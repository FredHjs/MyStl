#ifndef MYSTL_ALGORITHM_H
#define MYSTL_ALGORITHM_H

#include <type_traits>
#include <memory>

namespace MyStl
{
template<typename InputIt1, typename InputIt2> 
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2){
    for (; first1 != last1; ++first1, ++first2){
        if (*first1 != *first2) return false;
    }

    return true;
}

template<typename InputIt1, typename InputIt2, typename F> 
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2, F& pred){
    for (; first1 != last1; ++first1, ++first2){
        if (!pred(*first1, *first2)) return false;
    }

    return true;
}

template <typename T>
const T& max(const T& op_1, const T& op_2){return op_1 < op_2 ? op_2 : op_1;}

template <typename T, typename F>
const T& max(const T& op_1, const T& op_2, F pred){return pred(op_1, op_2) ? op_2 : op_1;}

template <typename T>
const T& min(const T& op_1, const T& op_2){return op_1 < op_2 ? op_1 : op_2;}

template <typename T, typename F>
const T& min(const T& op_1, const T& op_2, F pred){return pred(op_1, op_2) ? op_1 : op_2;}

int abs (int n){
    return n < 0 ? -n : n;
}

long abs(long n){
    return n < 0 ? -n : n;
}

long long llabs(long long n){
    return n < 0 ? -n : n;
}

template<typename T>
void destroy_unchecked(T* p, std::true_type){}

template<typename T>
void destroy_unchecked(T* p, std::false_type){
    if (p != nullptr) p -> ~T();
}

template<typename T>
void destroy(T* p){
    destroy_unchecked(p, std::is_trivially_destructible<T>{});
}

template<typename ForwardIt, typename T>
void fill(ForwardIt first, ForwardIt last, const T& value){
    for (; first != last; ++first) {*first = value;}
}

template<class InputIt, class OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt d_first){
    while(first != last){
        *(d_first++) = *(first++);
    }

    return d_first;
}

template<typename ForwardIt, typename T>
void uninitialized_fill_unchecked(ForwardIt first, ForwardIt last, const T& value, std::true_type){
    fill(first, last, value);
}

template<typename ForwardIt, typename T>
void uninitialized_fill_unchecked(ForwardIt first, ForwardIt last, const T& value, std::false_type){
    auto cur = first;
    try{
        for (; cur != last; ++cur){
            ::new((void*) &*cur) T(value);
        }
    }catch(...){
        for (; first != cur; ++first){
            destroy(&*first);
        }
    }
}

template<typename ForwardIt, typename T>
void uninitialized_fill(ForwardIt first, ForwardIt last, const T& value){
    uninitialized_fill_unchecked(first, last, value, std::is_trivially_copy_constructible<T>{});
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy_unchecked(InputIt first, InputIt last, ForwardIt d_first, std::true_type){
    return copy(first, last, d_first);
}

template <typename InputIt, typename ForwardIt>
ForwardIt uninitialized_copy_unchecked(InputIt first, InputIt last, ForwardIt d_first, std::false_type){
    auto cur = d_first;
    try{
        for (; first != last; ++cur, ++first){
            ::new((void*) &*cur) typename Iterator_Traits<InputIt>::value_type(*first);
        }
    }catch(...){
        for (; d_first != cur; ++d_first){
            destroy(&*d_first);
        }
    }
    return cur;
}

template<class InputIt, class ForwardIt>
ForwardIt uninitialized_copy(InputIt first, InputIt last, ForwardIt d_first){
    return uninitialized_copy_unchecked(first, last, d_first, std::is_trivially_copy_assignable<typename Iterator_Traits<InputIt>::value_type>{});
}

template<class BidirIt1, class BidirIt2>
BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 d_last){ //returns the last element copied (the one originally pointed to by first)
    while (last != first){
        *(--d_last) = *(--last);
    }

    return d_last;
}

template <typename InputIt1, typename InputIt2>
bool lexicographical_compare(InputIt1 first_1, InputIt1 last_1, InputIt2 first_2, InputIt2 last_2){
    while  (first_1 != last_1 && first_2 != last_2){
        if (*first_1 < *first_2) return true;
        else if (*first_2 < *first_1) return false;

        ++first_1, ++first_2;
    }

    return (first_1 == last_1 && first_2 != last_2);
}

template <typename InputIt1, typename InputIt2, typename F>
bool lexicographical_compare(InputIt1 first_1, InputIt1 last_1, InputIt2 first_2, InputIt2 last_2, F pred){
    while  (first_1 != last_1 && first_2 != last_2){
        if (pred(*first_1, *first_2)) return true;
        else if (pred(*first_2, *first_1)) return false;

        ++first_1, ++first_2;
    }

    return (first_1 == last_1 && first_2 != last_2);
}

template<typename InputIt, typename F>
F for_each(InputIt first, InputIt last, F func){
    for (; first != last; ++first){
        func(*first);
    }

    return func;
}

template<typename InputIt, typename OutputIt, typename F>
OutputIt copy_if(InputIt first, InputIt last, OutputIt d_first, F pred){
    for (; first != last; ++first){
        if (pred(*first)){
            *d_first = *first;
            ++d_first;
        }
    }
    return d_first;
}
} // namespace MyStl


#endif