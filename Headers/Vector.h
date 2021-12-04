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

            value_type* begin;

            value_type* end;

            value_type* cap;

        public:
            /* ctors and dtors */
            Vector() noexcept {
                try{
                    end = begin = alloc.allocate(16);
                    cap = begin + 16;
                }catch(...){
                    begin = end = cap = nullptr;
                }
            }

            Vector(size_type count, const T& value){
                size_type cap = count < 16 ? 16 : count;
                
                try{
                    begin = alloc.allocate(cap);
                    end = begin + count;
                    cap = begin + cap;
                }catch(...){
                    begin = end = cap = nullptr;
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

                cap = begin + cap;
            }

            Vector(const Vector& other): Vector(other.begin, other.end){}

            Vector(Vector&& other): begin(other.begin), end(other.end), cap(other.cap){
                other.begin = other.end = other.cap = nullptr;
            }

            Vector(std::initializer_list<T> init): Vector(init.begin(), init.end()){}

            ~Vector(){free();}
        
            Vector& operator=(const Vector& other){
                if (&other != this){
                    auto new_beg = alloc.allocate(other.size());
                    auto new_end = uninitialized_copy(other.begin, other.end, new_beg);
                    free();
                    begin = new_beg;
                    end = cap = new_end;
                }

                return *this;
            }

            Vector& operator=(Vector&& other){
                if (&other != this){
                    free();
                    this->begin = other.begin;
                    this->end = other.end;
                    this->cap = other.cap;
                    other.begin = other.end = other.cap = nullptr;
                }
                
                return *this;
            }

            Vector& operator=(std::initializer_list<T> ilist){
                this->operator=(Vector(ilist.begin(), ilist.end()));
            }

        public:
            /* capacity */
            size_type capacity(){return cap - begin;}

            size_type size(){return end - begin; }

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

                return out;     //returns the iterator after last copied element
            }

            void free(){
                for (value_type* i = begin; i != end; ++i){
                    alloc.destroy(i);
                }
                alloc.deallocate(begin, cap - begin);

                begin = end = cap = nullptr;
            }
    };
}

#endif