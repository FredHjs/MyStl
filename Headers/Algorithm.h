#ifndef MYSTL_ALGORITHM_H
#define MYSTL_ALGORITHM_H

namespace MyStl
{
template<typename InputIt1, typename InputIt2> bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2){
    for (; first1 != last1; ++first1, ++first2){
        if (*first1 != *first2) return false;
    }

    return true;
}
} // namespace MyStl


#endif