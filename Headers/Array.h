#ifndef MYSTL_ARRAY_H
#define MYSTL_ARRAY_H

#include <stdexcept>
#include <utility>

#include "Iterator.h"

namespace MyStl{
    template<class T, std::size_t N> 
    struct Array{
        static_assert(!(N == 0), "can't initialize array of length 0");
        public:
            using value_type = T;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type&;
            using const_reference = const value_type&;
            using pointer = value_type*;
            using const_pointer = const value_type*;

            using iterator = T*;
            using const_iterator = const T*;

            using reverse_iterator = Reverse_Iterator<iterator>;
            using const_reverse_iterator = Reverse_Iterator<const_iterator>;

            //has to be public due to implicitly declared ctor and dtors
            T _elements[N];

        public:
            /* member access */
            reference at(size_type pos){
                if (!(pos < N)) throw std::out_of_range("member access beyond size");
                return _elements[pos];
            }

            const_reference at(size_type pos) const {
                if (!(pos < N)) throw std::out_of_range("member access beyond size");
                return _elements[pos];
            }

            reference operator[](size_type pos){return _elements[pos];}

            const_reference operator[](size_type pos) const {return _elements[pos];}

            reference front(){return _elements[0];}

            const_reference front() const {return _elements[0];}

            reference back(){return _elements[N - 1];}

            const_reference back() const {return _elements[N - 1];}

            T* data() noexcept {return _elements;}

            const T* data() const noexcept {return _elements;}

        public:
            /* iterators */
            iterator begin() noexcept {return _elements;}

            const_iterator begin() const noexcept {return _elements;}

            const_iterator cbegin() const noexcept {return _elements;}

            iterator end() noexcept {return _elements + N;}

            const_iterator end() const noexcept {return _elements + N;}

            const_iterator cend() const noexcept {return _elements + N;}

            reverse_iterator rbegin() noexcept {return reverse_iterator(end());}

            const_reverse_iterator rbegin() const noexcept {return reverse_iterator(end());}

            const_reverse_iterator crbegin() const noexcept {return reverse_iterator(end());}

            reverse_iterator rend() noexcept {return reverse_iterator(begin());}

            const_reverse_iterator rend() const noexcept {return reverse_iterator(begin());}

            const_reverse_iterator crend() const noexcept {return reverse_iterator(begin());}

        public:
            /* capacity */
            constexpr bool empty() const noexcept {return false;}

            constexpr size_type size() const noexcept {return N;}

            constexpr size_type max_size() const noexcept {return N;}

        public:
            /* operations */
            void fill(const T& value){
                for (size_type i = 0; i < N; ++i){
                    _elements[i] = value;
                }
            }

            void swap(Array& other) noexcept(noexcept(std::swap(std::declval<T&>(), std::declval<T&>()))){
                range_swap(_elements, _elements + N, other._elements);
            }

        private:
            /* helpers */
            template<typename ForwardIt1, typename ForwardIt2>
            ForwardIt2 range_swap(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2){
                for (; first1 != last1; ++first1, ++first2){
                    std::swap(*first1, *first2);
                }

                return first2;
            }
    };
}

#endif