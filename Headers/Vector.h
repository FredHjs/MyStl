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

            iterator _begin;

            iterator _end;

            iterator cap;

        public:
            /* ctors and dtors */
            Vector() noexcept {
                try{
                    _end = _begin = alloc.allocate(16);
                    cap = _begin + 16;
                }catch(...){
                    _begin = _end = cap = nullptr;
                }
            }

            Vector(size_type count, const T& value){
                size_type capa = count < 16 ? 16 : count;
                
                try{
                    _begin = alloc.allocate(capa);
                    _end = _begin + count;
                    cap = _begin + capa;
                }catch(...){
                    _begin = _end = cap = nullptr;
                    throw;
                }

                initialize_all(value);
            }

            explicit Vector(size_type count): Vector(count, T()){};

            template<class InputIt> 
            Vector(InputIt first, InputIt last){
                assert(first < last);

                _begin = alloc.allocate(last - first);

                size_type capa = (last - first) < 16 ? 16 : (last - first);

                _end = uninitialized_copy(first, last, _begin);

                cap = _begin + capa;
            }

            Vector(const Vector& other): Vector(other._begin, other._end){}

            Vector(Vector&& other): _begin(other._begin), _end(other._end), cap(other.cap){
                other._begin = other._end = other.cap = nullptr;
            }

            Vector(std::initializer_list<T> init): Vector(init.begin(), init.end()){}

            ~Vector(){free();}
        
            Vector& operator=(const Vector& other){
                if (&other != this){
                    auto new_beg = alloc.allocate(other.size());
                    auto new_end = uninitialized_copy(other._begin, other._end, new_beg);
                    free();
                    _begin = new_beg;
                    _end = cap = new_end;
                }

                return *this;
            }

            Vector& operator=(Vector&& other){
                if (&other != this){
                    free();
                    this->_begin = other._begin;
                    this->_end = other._end;
                    this->cap = other.cap;
                    other._begin = other._end = other.cap = nullptr;
                }
                
                return *this;
            }

            Vector& operator=(std::initializer_list<T> ilist){
                Vector<T> temp(ilist._begin, ilist._end);
                this->swap(temp);
                return *this;
            }

            void assign(size_type count, const T& value){
                Vector<T> temp(count, value);
                this->swap(temp);
            }

            //TODO: differenciate between this and the above overload
            template<class InputIt> void assign(InputIt first, InputIt last){
                assert(first < last);

                if (capacity() < last - first || last - first < size()){
                    Vector<T> temp(first, last);
                    this->swap(temp);
                }else{
                    auto new_end = _begin;
                    for (auto i = first; i != last ; ++i, ++new_end){
                        *new_end = *i;
                    }
                    _end = new_end;
                }
            }

            void assign(std::initializer_list<T> ilist){
                this->assign(ilist.begin(), ilist.end());
            }

        public:
            /* member access */
            reference front(){
                assert(!empty());
                return *_begin;
            }

            const_reference front() const {
                assert(!empty());
                return *_begin;
            }

            reference back(){
                assert(!empty());
                return *(_end - 1);
            }

            const_reference back() const {
                assert(!empty());
                return *(_end - 1);
            }

            reference operator[](size_type pos){
                assert(pos < size());
                return *(_begin + pos);
            }

            const_reference operator[](size_type pos) const {
                assert(pos < size());
                return *(_begin + pos);
            }

            reference at(size_type pos){
                if (pos >= size()) throw std::out_of_range("element index out of range");

                return this->operator[](pos);
            }

            const_reference at(size_type pos) const {
                if (pos >= size()) throw std::out_of_range("element index out of range");

                return this->operator[](pos);
            }

            T* data() noexcept {return _begin;}
            const T* data() const noexcept {return _begin;}

        public:
            /* iterators */
            iterator begin() noexcept{
                return _begin;
            }

            const_iterator begin() const noexcept {
                return _begin;
            }

            const_iterator cbegin() const noexcept {
                return _begin;
            }

            iterator end() noexcept{
                return _end;
            }

            const_iterator end() const noexcept {
                return _end;
            }

            const_iterator cend() const noexcept {
                return _end;
            }

            //TODO: reverse_iterator functions

        public:
            /* capacity */
            size_type capacity() const noexcept {return cap - _begin;}

            size_type size() const noexcept {return _end - _begin; }

            bool empty() const noexcept {return _begin == _end;}

            size_type max_size() const noexcept {return static_cast<size_type>(-1) / sizeof(value_type);}

            void reserve(size_type new_cap){
                if (new_cap <= capacity()) return;

                if (new_cap > max_size()){
                    throw std::length_error("cannot reserve capacity bigger than max_size");
                }

                T* new_begin = alloc.allocate(new_cap);
                T* new_end = uninitialized_move(_begin, _end, new_begin);

                alloc.deallocate(_begin, capacity());

                _begin = new_begin;
                _end = cap = new_end;
            }

            void shrink_to_fit(){
                auto new_beg = alloc.allocate(size());
                
                T* new_end;
                try{
                    new_end = uninitialized_move(_begin, _end, new_beg);
                }catch(...){
                    alloc.deallocate(new_beg, size());
                    throw;
                }

                alloc.deallocate(_begin, capacity());

                _begin = new_beg;
                _end = cap = new_end;
            }

        private:
            /* helpers */
            void initialize_all(const T& val){
                T* i = _begin;
                try{
                    for (; i != _end; ++i){
                        alloc.construct(i, val);
                    }
                }catch(...){
                    for (T* temp = _begin; temp != i; ++temp){
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

            template <typename InputIt, typename FowardIt>
            FowardIt uninitialized_move(InputIt beg, InputIt end, FowardIt result){
                FowardIt out = result;
                try{
                    for (; beg != end; ++beg, ++out){
                        alloc.construct(&*out, std::move(*beg));
                    }
                }catch(...){
                    for (; out != result; --out){
                        alloc.destroy(&*out);
                    }
                }

                return out;     //returns the iterator after last copied element
            }

            void free(){
                for (value_type* i = _begin; i != _end; ++i){
                    alloc.destroy(i);
                }
                alloc.deallocate(_begin, cap - _begin);

                _begin = _end = cap = nullptr;
            }

            void swap(Vector<T>& other) noexcept {
                if (&other != this){
                    std::swap(this->_begin, other._begin);
                    std::swap(this->_end, other._end);
                    std::swap(this->cap, other.cap);
                }
            }
    };
}

#endif