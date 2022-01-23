#ifndef MYSTL_ALGORITHM_H
#define MYSTL_ALGORITHM_H

namespace MyStl
{
template<typename InputIt1, typename InputIt2> 
bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2){
    for (; first1 != last1; ++first1, ++first2){
        if (*first1 != *first2) return false;
    }

    return true;
}

template <typename T>
const T& max(const T& op_1, const T& op_2){return op_1 < op_2 ? op_2 : op_1;}

template <typename T, typename F>
const T& max(const T& op_1, const T& op_2, F& pred){return pred(op_1, op_2) ? op_2 : op_1;}

template <typename T>
const T& min(const T& op_1, const T& op_2){return op_1 < op_2 ? op_1 : op_2;}

template <typename T, typename F>
const T& min(const T& op_1, const T& op_2, F& pred){return pred(op_1, op_2) ? op_1 : op_2;}
} // namespace MyStl


#endif