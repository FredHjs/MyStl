#ifndef MYSTL_VECTOR_H
#define MYSTL_VECTOR_H

#include <memory>
#include <assert.h>

namespace MyStl{
    template <typename T>
    class Vector{
        typedef typename std::allocator<T>::value_type value_type;
        typedef typename std::allocator<T>::size_type size_type;

        private:
            /* member fields*/
            std::allocator<T> alloc;

            T* begin;

            T* end;

            T* capacity;

        public:
            /* ctors */
            Vector() noexcept {
                try{
                    end = begin = alloc.allocate(16);
                    capacity = begin + 16;
                }catch(...){
                    begin = end = capacity = nullptr;
                }
            }

            Vector(size_type count, const T& value){
                size_type cap = count < 16 ? 16 : count;
                
                try{
                    begin = alloc.allocate(cap);
                    end = begin + count;
                    capacity = begin + cap;
                }catch(...){
                    begin = end = capacity = nullptr;
                    throw;
                }

                initialize_all(value);
            }

            explicit Vector(size_type count): Vector(count, T()){};

            template<class InputIt> 
            Vector(InputIt first, InputIt last){
                assert(first < last);

                begin = alloc.allocate(last - first);

                size_type cap = (last - first) < 16 ? 16 : (last - first);

                end = uninitialized_copy(first, last, begin);

                capacity = begin + cap;
            }

            Vector(const Vector& other);

            Vector(Vector&& other);

            Vector(std::initializer_list<T> init);

            ~Vector();

        private:
            /* helpers */
            void initialize_all(const T& val){
                T* i = begin;
                try{
                    for (; i != end; ++i){
                        alloc.construct(i, val);
                    }
                }catch(...){
                    for (T* temp = begin; temp != i; ++temp){
                        alloc.destroy(temp);
                    }
                }
            }

            template <typename InputIt, typename FowardIt>
            FowardIt uninitialized_copy(InputIt beg, InputIt end, FowardIt result){
                FowardIt out = result;
                try{
                    for (; beg != end; ++beg, ++out){
                        alloc.construct(&*out, *beg);
                    }
                }catch(...){
                    for (; out != result; --out){
                        alloc.destroy(&*out);
                    }
                }

                return out;
            }
    };
}

#endif