#ifndef MYSTL_VECTOR_H
#define MYSTL_VECTOR_H

#include <memory>

namespace MyStl{
    template <typename T>
    class Vector{
        //typedef typename Allocator::value_type value_type;

        private:
            std::allocator<T> alloc;

            T* begin;

            T* end;

            T* capacity;

        public:
            /* ctors */
            vector() noexcept;

            vector(size_type count, const T& value);

            explicit vector(size_type count);

            template<class InputIt> 
            vector(InputIt first, InputIt last);

            vector(const vector& other);

            vector(vector&& other);

            vector(std::initializer_list<T> init);
    };
}

#endif