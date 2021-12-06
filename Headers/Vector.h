#ifndef MYSTL_VECTOR_H
#define MYSTL_VECTOR_H

#include <memory>
#include <utility>
#include <assert.h>
#include <stdexcept>
#include <type_traits>
//#include <type_traits>

namespace MyStl{
    template <typename T>
    class Vector{
        using value_type = typename std::allocator<T>::value_type;
        using size_type = typename std::allocator<T>::size_type;
        using difference_type = typename std::allocator<T>::difference_type;
        using pointer = typename std::allocator_traits<std::allocator<T>>::pointer;
        using const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer;
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = const T*;

        //TODO: reverse_iterator

        private:
            /* member fields*/
            std::allocator<T> alloc;

            iterator begin;

            iterator end;

            iterator cap;

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
                size_type capa = count < 16 ? 16 : count;
                
                try{
                    begin = alloc.allocate(capa);
                    end = begin + count;
                    cap = begin + capa;
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

                size_type capa = (last - first) < 16 ? 16 : (last - first);

                end = uninitialized_copy(first, last, begin);

                cap = begin + capa;
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
                Vector<T> temp(ilist.begin, ilist.end);
                this->swap(temp);
                return *this;
            }

            void assign(size_type count, const T& value){
                Vector<T> temp(count, value);
                this->swap(temp);
            }

            template<class InputIt> void assign(InputIt first, InputIt last){
                assert(first < last);

                if (capacity() < last - first || last - first < size()){
                    Vector<T> temp(first, last);
                    this->swap(temp);
                }else{
                    auto new_end = begin;
                    for (auto i = first; i != last ; ++i, ++new_end){
                        *new_end = *i;
                    }
                    end = new_end;
                }
            }

            void assign(std::initializer_list<T> ilist){
                this->assign(ilist.begin(), ilist.end());
            }

        public:
            /* member access */
            reference front(){
                assert(!empty());
                return *begin;
            }

            const_reference front() const {
                assert(!empty());
                return *begin;
            }

            reference back(){
                assert(!empty());
                return *(end - 1);
            }

            const_reference back() const {
                assert(!empty());
                return *(end - 1);
            }

            reference operator[](size_type pos){
                assert(pos < size());
                return *(begin + pos);
            }

            const_reference operator[](size_type pos) const {
                assert(pos < size());
                return *(begin + pos);
            }

            reference at(size_type pos){
                if (pos >= size()) throw std::out_of_range("element index out of range");

                return this->operator[](pos);
            }

            const_reference at(size_type pos) const {
                if (pos >= size()) throw std::out_of_range("element index out of range");

                return this->operator[](pos);
            }

            T* data() noexcept {return begin;}
            const T* data() const noexcept {return begin;}

        public:
            /* iterators */
            iterator begin() noexcept{
                return begin;
            }

            const_iterator begin() const noexcept {
                return begin;
            }

            const_iterator cbegin() const noexcept {
                return begin;
            }

            iterator end() noexcept{
                return end;
            }

            const_iterator end() const noexcept {
                return end;
            }

            const_iterator cend() const noexcept {
                return end;
            }

            //TODO: reverse_iterator functions

        public:
            /* capacity */
            size_type capacity() const noexcept {return cap - begin;}

            size_type size() const noexcept {return end - begin; }

            bool empty() const noexcept {return begin == end;}

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

            void swap(Vector<T>& other) noexcept {
                if (&other != this){
                    std::swap(this->begin, other.begin);
                    std::swap(this->end, other.end);
                    std::swap(this->cap, other.cap);
                }
            }
    };
}

#endif