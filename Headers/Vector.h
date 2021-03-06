#ifndef MYSTL_VECTOR_H
#define MYSTL_VECTOR_H

#include <memory>
#include <assert.h>
#include <stdexcept>
#include <cstring>
#include <utility>
#include <algorithm>

#include "Iterator.h"
#include "Algorithm.h"

namespace MyStl{
    template <typename T>
    class Vector{
        public:
            using value_type = typename std::allocator<T>::value_type;
            using size_type = typename std::allocator<T>::size_type;
            using difference_type = typename std::allocator<T>::difference_type;
            using pointer = typename std::allocator_traits<std::allocator<T>>::pointer;
            using const_pointer = typename std::allocator_traits<std::allocator<T>>::const_pointer;
            using reference = T&;
            using const_reference = const T&;
            using iterator = T*;
            using const_iterator = const T*;
            using reverse_iterator = Reverse_Iterator<iterator>;
            using const_reverse_iterator = Reverse_Iterator<const_iterator>;

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
                    _end = _begin = alloc.allocate(8);
                    cap = _begin + 8;
                }catch(...){
                    _begin = _end = cap = nullptr;
                }
            }

            Vector(size_type count, const T& value){
                size_type capa = count < 8 ? 8 : count;
                
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

            template<class InputIt, typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true> 
            Vector(InputIt first, InputIt last){
                assert(first < last);
                size_type n = last - first;

                size_type capa = n < 8 ? 8 : n;

                _begin = alloc.allocate(capa);

                _end = MyStl::uninitialized_copy(first, last, _begin);

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
                    auto new_end = MyStl::uninitialized_copy(other._begin, other._end, new_beg);
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
                Vector<T> temp(ilist.begin(), ilist.end());
                this->swap(temp);
                return *this;
            }

            void assign(size_type count, const T& value){
                Vector<T> temp(count, value);
                this->swap(temp);
            }

            template<class InputIt, typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true> 
            void assign(InputIt first, InputIt last){
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

            reverse_iterator rbegin() noexcept {
                return reverse_iterator(_end);
            } 

            const_reverse_iterator rbegin() const noexcept {
                return const_reverse_iterator(_end);
            }

            const_reverse_iterator crbegin() const noexcept {
                return const_reverse_iterator(_end);
            }

            reverse_iterator rend() noexcept {
                return reverse_iterator(_begin);
            }

            const_reverse_iterator rend() const noexcept {
                return const_reverse_iterator(_begin);
            }

            const_reverse_iterator crend() const noexcept {
                return const_reverse_iterator(_begin);
            }

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

        public:
            /* modifiers */
            void clear() noexcept {
                erase(_begin, _end);
            }

            iterator erase(const_iterator pos){
                assert(pos >= _begin && pos < _end);
                iterator erase_pos = const_cast<iterator>(pos);
                std::move(erase_pos + 1, _end, erase_pos);
                alloc.destroy(--_end);
                return const_cast<iterator>(pos);
            }

            iterator erase(const_iterator first, const_iterator last){
                assert((first >= _begin && first < _end) 
                      && (last >= _begin && last <= _end) 
                      && (first <= last));

                auto first_to_move = const_cast<iterator>(last);
                auto move_to = const_cast<iterator>(first);
                auto old_end = _end;

                std::move(first_to_move, _end, move_to);
                _end -= (last - first);

                while(old_end != _end){
                    alloc.destroy(--old_end);
                }

                return first_to_move;
            }

            // T must be copy-assignable and copy-insertable to use this overload
            iterator insert(const_iterator pos, const T& value){
                assert(pos >= _begin && pos <= _end);
                // in case value refers to an element within the container
                T value_backup(value);
                return emplace(pos, value_backup);
            }

            // T must be move-assignable and move-insertable to use this overload
            iterator insert(const_iterator pos, T&& value){
                assert(pos >= _begin && pos <= _end);
                return emplace(pos, std::move(value));
            }

            iterator insert(const_iterator pos, size_type count, const T& value){
                if (count == 0) return const_cast<iterator>(pos);

                T value_backup(value);
                auto insert_pos = const_cast<iterator>(pos);
                
                if (cap - _end < count){
                    size_type insert_index = pos - _begin;
                    reallocate(size() + count);
                    insert_pos = _begin + insert_index;
                }

                size_type num_after_pos = _end - insert_pos;
                if (num_after_pos >= count){
                    auto new_end = MyStl::uninitialized_copy(_end - count, _end, _end);
                    MyStl::copy_backward(insert_pos, _end - count, _end);
                    MyStl::fill(insert_pos, insert_pos + count, value_backup);

                    _end = new_end;

                    return insert_pos;
                }else{
                    auto new_end = uninitialized_move(insert_pos, _end, insert_pos + count);
                    MyStl::fill(insert_pos, _end, value_backup);
                    MyStl::uninitialized_fill(_end, new_end, value_backup);

                    _end = new_end;
                    return insert_pos;
                }
            }

            template<class InputIt, typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true> 
            iterator insert(const_iterator pos, InputIt first, InputIt last){
                assert(first <= last);
                if (first == last) return const_cast<iterator>(pos);

                auto insert_pos = const_cast<iterator>(pos);
                size_type count = last - first;

                if (cap - _end < count){
                    size_type insert_index = pos - _begin;
                    reallocate(size() + count);
                    insert_pos = _begin + insert_index;
                }
                iterator old_end = _end;
                
                for (; first != last; ++first){
                    emplace_back(*first);
                }

                std::rotate(insert_pos, old_end, _end); //TODO: define rotate in algorithm part

                return insert_pos;
            }

            iterator insert(const_iterator pos, std::initializer_list<T> ilist){
                return insert(pos, ilist.begin(), ilist.end());
            }

            template<class... Args> iterator emplace(const_iterator pos, Args&&... args){
                assert(pos >= _begin && pos <= _end);
                
                iterator insert_pos;
                if (_end != cap){
                    insert_pos = const_cast<iterator>(pos);
                }else{
                    size_type insert_index = pos - _begin;
                    reallocate(size() + 1);
                    insert_pos = _begin + insert_index;
                }

                batch_move_backward_unchecked(insert_pos, _end, _end + 1);
                *insert_pos = value_type(std::forward<Args>(args)...);
                ++_end;
                return insert_pos; 
            }

            template<class... Args> void emplace_back(Args&&... args){
                if (_end == cap) reallocate(size() + 1);

                *(_end++) = value_type(std::forward<Args>(args)...);
            }

            void push_back(const T& value){
                emplace_back(value);
            }

            void push_back(T&& value){
                emplace_back(std::move(value));
            }

            void pop_back(){
                assert(!(size() == 0));
                alloc.destroy(_end);
                --_end;
            }

            void resize(size_type count){
                resize(count, value_type());
            }

            void resize(size_type count, const value_type& value){
                if (count < size()){
                    erase(_begin + count, _end);
                }

                if (size() < count){
                    insert(_end, count - size(), value);
                }

                //if count == size() do nothing
            }

            void swap(Vector<T>& other) noexcept {
                if (&other != this){
                    std::swap(this->_begin, other._begin);
                    std::swap(this->_end, other._end);
                    std::swap(this->cap, other.cap);
                }
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

                return out;     //returns the iterator after last moved element
            }

            template <typename FowardIter, typename Count>
            FowardIter uninitialized_fill_n(FowardIter beg, Count count, T value){
                FowardIter out = beg;
                try{
                    for (; count > 0; --count, ++out){
                        alloc.construct(&*out, value);
                    }
                }catch(...){
                    for (; out != beg; --out){
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
            
            /* move backward from [first, last) to [..., result) */
            template<typename InputIter>
            iterator batch_move_backward_unchecked(InputIter first, InputIter last, iterator result){
                if (first == last) return result;

                while(last != first){
                    *(--result) = std::move(*(--last));
                }

                return result;  //returns iterator after the last moved element
            }

            void reallocate(size_type reserve_cap){
                auto size_ = size();
                size_type new_cap = size_ * 2 < reserve_cap ? reserve_cap : size_ * 2;
                iterator new_begin = alloc.allocate(new_cap);
                auto new_cap_iter = new_begin + new_cap;
                auto new_end = uninitialized_move(_begin, _end, new_begin);
                
                free();
                _begin = new_begin;
                _end = new_end;
                cap = new_cap_iter;
            }
    };

    /* operators */
    template<class T>
    bool operator==(const MyStl::Vector<T>& lhs, const MyStl::Vector<T>& rhs){
        if (lhs.size() != rhs.size()) return false;

        return MyStl::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<class T>
    bool operator!=(const MyStl::Vector<T>& lhs, const MyStl::Vector<T>& rhs){return !(lhs == rhs);}

    template<class T>
    bool operator<(const MyStl::Vector<T>& lhs, const MyStl::Vector<T>& rhs){
        return MyStl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<class T>
    bool operator<=(const MyStl::Vector<T>& lhs, const MyStl::Vector<T>& rhs){return !(rhs < lhs);}

    template<class T>
    bool operator>(const MyStl::Vector<T>& lhs, const MyStl::Vector<T>& rhs){return rhs < lhs;}

    template<class T>
    bool operator>=(const MyStl::Vector<T>& lhs, const MyStl::Vector<T>& rhs){return !(lhs < rhs);}
}

#endif