#ifndef MYSTL_DEQUE_H
#define MYSTL_DEQUE_H

#include <memory>
#include <assert.h>
#include <stdexcept>

#include "Iterator.h"
#include "Algorithm.h"

#define DEQUE_INITIAL_MINIMUN_MAP_SIZE 8

namespace MyStl
{
    template <typename T> class Deque;

    template<typename T, typename Reference, typename Pointer>
    class Deque_Iterator: Iterator<Random_Access_Iterator_Tag, T, ptrdiff_t, Pointer, Reference>{
        friend class Deque<T>;
        
        public:
            using map_ptr = T**;
            using value_ptr = T*;
            using size_type = std::size_t;
            
            using reference = Reference;
            using pointer = Pointer;
            using difference_type = ptrdiff_t;
            static constexpr size_type block_size = sizeof(T) < 512 ? 512/sizeof(T) : 1;

        private:
            map_ptr map_node;
            value_ptr cur;      //in the _end iterator, cur is pass-the-end pointer
            value_ptr first;
            value_ptr last;

        public:
            /* ctors */
            Deque_Iterator() noexcept = default;

            Deque_Iterator(const Deque_Iterator& other): map_node(other.map_node), cur(other.cur),
                first(other.first), last(other.last){}

            Deque_Iterator(Deque_Iterator&& other): map_node(other.map_node), cur(other.cur),
                first(other.first), last(other.last){
                    other.map_node = nullptr;
                    other.first = other.last = other.cur = nullptr;
                }

            Deque_Iterator(map_ptr map, value_ptr p_val): map_node(map), cur(p_val), 
                first(*map), last(*map + block_size){}

            //TODO: Deque_Iterator(size_type off, deque* p_deque)

        public:
            /* operators */
            Deque_Iterator& operator=(const Deque_Iterator& rhs){
                if (this != &rhs){
                    map_node = rhs.map_node;
                    cur = rhs.cur;
                    first = rhs.first;
                    last = rhs.last;
                }

                return *this;
            }

            reference operator*(){
                return *cur;
            }

            pointer operator->(){
                return cur;
            }

            Deque_Iterator& operator++(){
                ++cur;
                if (cur == last){
                    change_node_by(1);
                    cur = first;
                }
                return *this;
            }

            Deque_Iterator operator++(int){
                Deque_Iterator temp = *this;
                ++(*this);
                return temp;
            }

            Deque_Iterator& operator--(){
                if (cur == first){
                    change_node_by(-1);
                    cur = last;
                }
                --cur;
                return *this;
            }

            Deque_Iterator operator--(int){
                Deque_Iterator temp = *this;
                --(*this);
                return temp;
            }

            Deque_Iterator& operator+=(difference_type n){
                const difference_type distance_from_first = cur - first + n;

                if (distance_from_first >= 0){        //in current block or forward blocks
                    if (distance_from_first <  block_size){     //still in current block
                        cur += n;
                    }else{      //to another forward block
                        const difference_type node_diff = distance_from_first / block_size;
                        change_node_by(node_diff);
                        cur = first + (distance_from_first - node_diff * block_size);
                    }
                }else{      //move backward
                    const difference_type node_diff = distance_from_first 
                                        / static_cast<difference_type>(block_size) - 1;
                    change_node_by(node_diff);
                    cur = last + (distance_from_first - node_diff 
                                        * static_cast<difference_type>(block_size));
                }

                return *this;
            }

            Deque_Iterator& operator-=(difference_type n){
                return operator+=(-n);
            }

            Deque_Iterator& operator+(difference_type n) const {
                Deque_Iterator temp = *this;
                return temp.operator+=(n);
            }

            Deque_Iterator& operator-(difference_type n) const {
                Deque_Iterator temp = *this;
                return temp.operator+=(-n);
            }

            difference_type operator-(Deque_Iterator& rhs){
                return (cur - first) + (map_node - (rhs.map_node + 1)) * block_size + (rhs.last - rhs.cur);
            }

            bool operator==(const Deque_Iterator& rhs){return this->cur == rhs.cur;}

            bool operator!=(const Deque_Iterator& rhs){return this->cur != rhs.cur;}

            bool operator<(const Deque_Iterator& rhs){
                return map_node == rhs.map_node ? cur < rhs.cur : map_node < rhs.map_node;
            }

            bool operator>=(const Deque_Iterator& rhs){return !(*this < rhs);}

            bool operator>(const Deque_Iterator& rhs){return rhs < *this;}

            bool operator<=(const Deque_Iterator& rhs){return !(rhs < *this);}


        private:
            /* helpers */
            void change_node_by(difference_type n){
                map_node += n;
                first = cur = *map_node;
                last = *map_node + block_size;
            }

            void change_node_to(map_ptr node){
                map_node = node;
                first = *map_node;
                last = first + block_size;
            }
    };

    template <typename T>
    class Deque{
        public:
            using value_type = T;
            using allocator_type = std::allocator<T>;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type&;
            using const_reference = const value_type&;
            using pointer = typename allocator_type::pointer;
            using const_pointer = typename allocator_type::const_pointer;
            using iterator = Deque_Iterator<value_type, value_type&, value_type*>;
            using const_iterator = Deque_Iterator<value_type, const value_type&, const value_type*>;
            using reverse_iterator = Reverse_Iterator<iterator>;
            using const_reverse_iterator = Reverse_Iterator<const_iterator>;

            using map_ptr = pointer*;
            using map_allocator = std::allocator<pointer>;

            const size_type block_size = sizeof(T) < 512 ? 512/sizeof(T) : 1;

            allocator_type _get_al(){return allocator_type();}
            
            map_allocator _get_map_al(){return map_allocator();}

        private:
            iterator _begin;

            iterator _end;

            map_ptr _map;

            size_type _map_size;

        public:
            /* ctor and dtor */
            Deque(){
                fill_init_n(0, value_type());
            }

            Deque(size_type count, const T& value){
                fill_init_n(count, value);
            }

            explicit Deque(size_type count){
                fill_init_n(count, value_type());
            }
            
            template<class InputIt,typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true> 
            Deque(InputIt first, InputIt last){
                assert(first < last);

                copy_init(first, last);
            }

            Deque(const Deque& other): Deque(other._begin, other._end){}

            Deque(Deque&& other): _begin(std::move(other._begin)), _end(std::move(other._end)), 
                                _map(other._map), _map_size(other._map_size){
                other._map = nullptr;
            }

            Deque(std::initializer_list<T> init): Deque(init.begin(), init.end()){}

            ~Deque(){
                tidy();
            }

            Deque& operator=(const Deque& other){
                if (&other != this){
                    Deque<T> temp(other);
                    swap(temp);
                }

                return *this;
            }

            Deque& operator=(Deque&& other){
                tidy();
                _begin = std::move(other._begin);
                _end = std::move(other._end);
                _map = other._map;
                _map_size = other._map_size;

                other._map = nullptr;
                other._map_size = 0;

                return *this;
            }

            Deque& operator=(std::initializer_list<T> ilist){
                Deque<T> temp(ilist);
                swap(temp);
                return *this;
            }

            void assign (size_type count, const T& value){
                Deque<T> temp(count, value);

                swap(temp);
            }

            template<class InputIt,typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true> 
            void assign(InputIt first, InputIt last){
                Deque<T> temp(first, last);
                swap(temp);
            }

            void assign(std::initializer_list<T> ilist){
                assign(ilist.begin(), ilist.end());
            }

        public:
            /* member access */
            reference at(size_type pos){
                if (pos >= size()) throw std::out_of_range("member access out of range");

                return *(_begin + pos);
            }

            const_reference at(size_type pos) const{
                if (pos >= size()) throw std::out_of_range("member access out of range");

                return *(_begin + static_cast<difference_type>(pos));
            }

            reference operator[](size_type pos){
                assert(pos < size());
                return *(_begin + static_cast<difference_type>(pos));
            }

            const_reference operator[]( size_type pos ) const{
                assert(pos < size());
                return *(_begin + static_cast<difference_type>(pos));
            }

            reference front(){
                assert(!empty());
                return *_begin;
            }

            const_reference front() const{
                assert(!empty());
                return *_begin;
            }

            reference back(){
                assert(!empty());
                return *(_end - 1);
            }

            const_reference back() const{
                assert(!empty());
                return *(_end - 1);
            }

        public:
            /* iterators */
            iterator begin() noexcept {
                return _begin;
            }

            const_iterator begin() const noexcept {
                return _begin;
            }

            const_reference cbegin() const noexcept {
                return _begin;
            }

            iterator end() noexcept {
                return _end;
            }

            const_iterator end() const noexcept {
                return _end;
            }

            const_reference cend() const noexcept {
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
            size_type size() const noexcept{
                return _end - _begin;
            }

            bool empty() const noexcept{
                return _begin == _end;
            }

            size_type max_size() const noexcept {
                return static_cast<size_type>(-1) / sizeof(value_type);
            }

            void shrink_to_fit(){
                for (auto i = _map; i < _begin.map_node; ++i){
                    if (_map_size > static_cast<size_type>(DEQUE_INITIAL_MINIMUN_MAP_SIZE)){
                        _get_al().deallocate(*i, block_size);
                        _get_map_al().deallocate(i, 1);
                        ++_map;
                        --_map_size;
                    }
                }

                for (auto j = _map + _map_size; j > _end.map_node; --j){
                    if (_map_size > static_cast<size_type>(DEQUE_INITIAL_MINIMUN_MAP_SIZE)){
                        _get_al().deallocate(*j, block_size);
                        _get_map_al().deallocate(j, 1);
                        --_map_size;
                    }
                }
            }

        public:
            /*  modifiers  */
            void clear() noexcept {
                if (_begin.map_node == _end.map_node){
                    destroy_range(_begin.cur, _end.cur);
                }else{
                    destroy_range(_begin.cur, _begin.last);
                    for (auto i = _begin.map_node + 1; i != _end.map_node; ++i){
                        destroy_range(*i, *i + block_size);
                    }
                    destroy_range(_end.first, _end.cur);
                }

                _begin.change_node_to(_map + _map_size / 2);
                _begin.cur = _begin.first;
                _end = _begin;
            }

            void swap(Deque& other){
                if (&other != this){
                    std::swap(_begin, other._begin);
                    std::swap(_end, other._end);
                    std::swap(_map, other._map);
                    std::swap(_map_size, other._map_size);
                }
            }

            template<class... Args> void emplace_front(Args&&... args){
                if (_begin.cur == _begin.first){
                    add_block_front(1);
                }
                try{
                    _get_al().construct((--_begin).cur, std::forward<Args>(args)...);
                }catch(...){
                    ++_begin;
                    throw;
                }
            }

            template<class... Args> void emplace_back(Args&&... args){
                if (_end.cur == _end.last - 1){
                    add_block_back(1);
                }
                try{
                    _get_al().construct((_end++).cur, std::forward<Args>(args)...);
                }catch(...){
                    --_end;
                    throw;
                }
            }

            template<typename...Args>
            iterator emplace(const_iterator pos, Args&&...args){
                assert(_begin <= pos && pos < _end);

                size_type elem_before = pos - _begin, elem_after = _end - pos;
                if (pos.cur == _begin.cur) emplace_front(std::forward(args)...);
                else if (pos.cur == _end.cur) emplace_back(std::forward(args)...);
                else{
                    if (size() - elem_before >= elem_after){    //pos in the first half
                        if (_begin.cur == _begin.first){
                            add_block_front(1);
                        }
                        MyStl::copy(_begin, pos, _begin - 1);
                        *pos = value_type(std::forward(args)...);
                        --_begin;
                    }else{   //pos in the second half
                        if (_end.cur == _end.last - 1){
                            add_block_back(1); 
                        }
                        MyStl::copy_backward(pos, _end, _end + 1);
                        *pos = value_type(std::forward(args)...);
                        ++_end;
                    }
                }

                return _begin + elem_before;
            }

            iterator erase(const_iterator pos){
                assert(_begin <= pos && pos < _end);
                
                size_type elem_before = pos - _begin, elem_after = _end - pos;
                if (pos.cur == _begin.cur){
                    MyStl::destroy(_begin.cur);
                    ++_begin;
                }
                else if (pos.cur == (_end - 1).cur){
                    MyStl::destroy((_end - 1).cur);
                    --_end;
                }
                else{
                    auto next = pos + 1;
                    if (size() - elem_before >= elem_after){    //pos in the first half
                        MyStl::copy_backward(_begin, pos, next);
                        MyStl::destroy(_begin.cur);
                        ++_begin;
                    }else{   //pos in the second half
                        MyStl::copy(pos + 1, _end, pos);
                        MyStl::destroy((_end - 1).cur);
                        --_end;
                    }
                }

                return _begin + elem_before;
            }

            iterator erase(const_iterator first, const_iterator last){
                assert(first >= _begin && last <= _end);
                if (first == _begin && last == _end){
                    clear();
                    return _end;
                }
                size_type element_before = first - _begin, element_after = last - _end, element_erased = last - first;
                if (element_before <= element_after){   //less element at front to be copied
                    auto original_begin = _begin;
                    MyStl::copy_backward(_begin, first, last);
                    _begin = _begin + element_erased;
                    destroy_range(original_begin.cur, _begin.cur);
                }else{
                    auto original_end = _end;
                    MyStl::copy(last, _end, first);
                    _end = _end - element_erased;
                    destroy_range(_end, original_end);
                }

                return _begin + element_before;
            }

            void push_back(const T& value){
                emplace_back(value);
            }

            void push_back(T&& value){
                emplace_back(std::move(value));
            }

            void push_front(const T& value){
                emplace_front(value);
            }

            void push_front(T&& value){
                emplace_front(std::move(value));
            }

            void pop_back(){
                assert(!empty());

                --_end;
                MyStl::destroy(_end.cur);

                if (_end.cur == _end.last - 1){ //last block is totally empty after deletion
                    _get_map_al().deallocate(*(_end.map_node + 1), block_size);
                    *(_end.map_node + 1) = nullptr;
                }
            }

            void pop_front(){
                assert(!empty());

                MyStl::destroy(_begin.cur);
                ++_begin;

                if (_begin.cur == _begin.first){    //first block totally empty after deletion
                    _get_map_al().deallocate(*(_begin.map_node - 1), block_size);
                    *(_begin.map_node - 1) = nullptr;
                }
            }

            iterator insert(const_iterator pos, const T& value){
                assert(pos >= _begin && pos <= _end);

                size_type elem_before = pos - _begin;
                if (pos.cur == _begin.cur){
                    push_front(value);
                    return _begin;
                }else if (pos.cur == _end.cur){
                    push_back(value);
                    return (_end - 1);
                }else{
                    iterator pos_;
                    if (size() - elem_before <= size() / 2){    //pos in the first half
                        if (_begin.cur == _begin.first){
                            emplace_front(value);
                            ++_begin;
                        }
                        pos_ = _begin + elem_before;
                        MyStl::copy(_begin, pos_, _begin - 1);
                        *pos_ = value;
                        --_begin;
                    }else{   //pos in the second half
                        if (_end.cur == _end.last - 1){
                            emplace_back(value);
                            --_end;
                        }
                        pos_ = _begin + elem_before;
                        MyStl::copy_backward(pos_, _end, _end + 1);
                        *pos_ = value;
                        ++_end;
                    }

                    return pos_;
                }
            }

            iterator insert(const_iterator pos, T&& value){
                assert(pos >= _begin && pos <= _end);

                size_type elem_before = pos - _begin;
                if (pos.cur == _begin.cur){
                    push_front(value);
                    return _begin;
                }else if (pos.cur == _end.cur){
                    push_back(value);
                    return (_end - 1);
                }else{
                    iterator pos_;
                    if (elem_before <= size() / 2){    //pos in the first half
                        if (_begin.cur == _begin.first){
                            emplace_front(value);
                            ++_begin;
                        }
                        pos_ = _begin + elem_before;
                        MyStl::copy(_begin, pos_, _begin - 1);
                        *pos_ = std::move(value);
                        --_begin;
                    }else{   //pos in the second half
                        if (_end.cur == _end.last - 1){
                            emplace_back(value);
                            --_end;
                        }
                        pos_ = _begin + elem_before;
                        MyStl::copy_backward(pos_, _end, _end + 1);
                        *pos_ = std::move(value);
                        ++_end;
                    }

                    return pos_;
                }
            }

            iterator insert(const_iterator pos, size_type count, const T& value){
                assert(pos >= _begin && pos <= _end);

                size_type elem_before = pos - _begin, elem_after = _end - pos;
                if (elem_before <= elem_after){  //less than half elements needs to be copied at front
                    if (_begin.cur - _begin.first < count){
                        size_type num_extra_elems = count - (_begin.cur - _begin.first);
                        add_block_front(num_extra_elems % block_size ? num_extra_elems / block_size + 1 : num_extra_elems / block_size);
                    }

                    auto _pos = _begin + elem_before;

                    if (elem_before >= count){  //overlap
                        MyStl::uninitialized_copy(_begin, _begin + count, _begin - count);
                        MyStl::copy(_begin + count, _pos, _begin);
                        MyStl::fill(_pos - count, _pos, value);
                        _begin -= count;
                    }else{
                        auto fill_start_iter = MyStl::uninitialized_copy(_begin, _pos, _begin - count);
                        MyStl::uninitialized_fill(fill_start_iter, _begin, value);
                        MyStl::fill(_begin, _pos, value);
                        _begin -= count;
                    }
                }else{
                    if (_end.last - _end.cur - 1 < count){
                        size_type num_extra_elems = count - (_end.last - _end.cur - 1);
                        add_block_back(num_extra_elems / block_size + 1);
                    }

                    auto _pos = _end - elem_after;
                    
                    if (elem_after >= count){
                        MyStl::uninitialized_copy(_end - count, _end, _end);
                        MyStl::copy_backward(_pos, _end - count, _end);
                        MyStl::fill(_pos, _pos + count, value);
                        _end += count;
                    }else{
                        MyStl::uninitialized_copy(_pos, _end, _pos + count);
                        MyStl::fill(_pos, _end, value);
                        MyStl::uninitialized_fill(_end, _pos + count, value);
                    }
                }
                
                return _begin + elem_before;
            }

            template<class InputIt, typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true>
            iterator insert(const_iterator pos, InputIt first, InputIt last){
                assert(pos >= _begin && pos <= _end);

                return insert_unchecked(pos, first, last, typename Iterator_Traits<InputIt>::iterator_category());
            }

            iterator insert(const_iterator pos, std::initializer_list<T> ilist){
                return insert(pos, ilist.begin(), ilist.end());
            }


        private:
            /* helpers */
            template<class InputIt>
            iterator insert_unchecked(const_iterator pos, InputIt first, InputIt last, MyStl::Input_Iterator_Tag){  //input iterator only allows one pass of its container
                size_type elem_before = pos - _begin, original_elem_before = elem_before;
                while (first != last){
                    auto _pos = _begin + elem_before;
                    insert(_pos, *first);
                    ++elem_before;
                }

                return _begin + original_elem_before;
            }

            template<class ForwardIt>
            iterator insert_unchecked(const_iterator pos, ForwardIt first, ForwardIt last, MyStl::Forward_Iterator_Tag){
                auto count = distance(first, last);

                size_type elem_before = pos - _begin, elem_after = _end - pos;
                if (elem_before <= elem_after){  //less than half elements needs to be copied at front
                    if (_begin.cur - _begin.first < count){
                        size_type num_extra_elems = count - (_begin.cur - _begin.first);
                        add_block_front(num_extra_elems % block_size ? num_extra_elems / block_size + 1 : num_extra_elems / block_size);
                    }

                    auto _pos = _begin + elem_before;

                    if (elem_before >= count){  //overlap
                        MyStl::uninitialized_copy(_begin, _begin + count, _begin - count);
                        auto copy_start = MyStl::copy(_begin + count, _pos, _begin);
                        MyStl::copy(first, last, copy_start);
                        _begin -= count;
                    }else{
                        auto mid = first;
                        advance(mid, count - elem_before);
                        auto copy_start = MyStl::uninitialized_copy(_begin, _pos, _begin - count);
                        MyStl::uninitialized_copy(first, mid, copy_start);
                        MyStl::copy(mid, last, _begin);
                        _begin -= count;
                    }
                }else{
                    if (_end.last - _end.cur - 1 < count){
                        size_type num_extra_elems = count - (_end.last - _end.cur - 1);
                        add_block_back(num_extra_elems / block_size + 1);
                    }

                    auto _pos = _end - elem_after;
                    
                    if (elem_after >= count){
                        MyStl::uninitialized_copy(_end - count, _end, _end);
                        MyStl::copy_backward(_pos, _end - count, _end);
                        MyStl::copy(first, last, _pos);
                        _end += count;
                    }else{
                        auto mid = first;
                        advance(mid, count - elem_after);
                        auto copy_start = MyStl::uninitialized_copy(mid, last, _end);
                        MyStl::uninitialized_copy(_pos, _end, copy_start);
                        MyStl::copy(first, mid, _pos);
                        _end += count;
                    }
                }
                
                return _begin + elem_before;
            }

            void init_map_n(size_type num_elem){
                size_type num_block = (num_elem / block_size) + 1;
                _map_size = MyStl::max(num_block + 2, static_cast<size_type>(DEQUE_INITIAL_MINIMUN_MAP_SIZE));

                try{
                    _map = _get_map_al().allocate(_map_size);
                }catch(...){
                    _map = nullptr;
                    throw;
                }

                map_ptr block_begin = _map + (_map_size - num_block) / 2;

                try{
                    create_blocks_n(block_begin, num_block);
                }catch(...){
                    _get_map_al().deallocate(_map, _map_size);
                    _map = nullptr;
                    throw;
                }
                
                _begin.change_node_to(block_begin);
                _end.change_node_to(block_begin + num_block - 1);
                _begin.cur = _begin.first;
                _end.cur = _end.first + (num_elem % block_size);
            }

            void create_blocks_n(map_ptr begin, size_type n){
                auto cur = begin;

                try{
                    for (size_type i = 0; i < n; ++i, ++cur){
                        *(begin + i) = _get_al().allocate(block_size);
                    }
                }catch(...){
                    for (; cur != begin; --cur){
                        _get_al().deallocate(*(cur - 1), block_size);
                        cur = nullptr;
                    }
                    throw;
                }
            }

            void fill_init_n(size_type n, const value_type& value){
                init_map_n(n);

                for (auto cur = _begin.map_node; cur != _end.map_node; ++cur){
                    MyStl::uninitialized_fill(*cur, *cur + block_size, value);
                }

                MyStl::uninitialized_fill(*_end.map_node, _end.cur, value);
            }

            template <typename InputIt>
            void copy_init(InputIt first, InputIt last){
                init_map_n(static_cast<size_type>(last - first));

                for (auto i = _begin; first != last; ++first, ++i){
                    _get_al().construct(i.cur, *first);
                }
            }

            void destroy_range(pointer first, pointer last){
                for (auto cur = first; cur != last; ++cur){
                    _get_al().destroy(cur);
                }
            }

            void tidy(){
                if (_map){
                    clear();
                    for (auto i = _map; i < _map + _map_size; ++i){
                        _get_al().deallocate(*i, block_size);
                    }

                    _get_map_al().deallocate(_map, _map_size);
                    _map = nullptr;
                }
            }

            void add_block_front(size_type num_blk_require){      
                if (_begin.map_node - _map >= num_blk_require){   // if the number of map nodes are greater than or equal to n
                    decltype(_map) cur = _begin.map_node - num_blk_require;
                    try{
                        for (; cur != _begin.map_node; ++cur){
                            *cur = _get_map_al().allocate(block_size);
                        }
                    }catch(...){
                        for (; cur != _begin.map_node - num_blk_require - 1; --cur){
                            _get_map_al().deallocate(*(cur - 1), block_size);
                        }
                    }
                }else{
                    map_resize(num_blk_require, true);
                }
            }

            void add_block_back(size_type num_blk_require){
                if (_map_size - (_end.map_node - _map) - 1 >= num_blk_require){
                    decltype(_map) cur = _end.map_node + 1;
                    try{
                        for (size_t i = 0; i < num_blk_require; ++i, ++cur){
                            *cur = _get_al().allocate(block_size);
                        }
                    }catch(...){
                        for (; cur != _end.map_node; --cur){
                            _get_al().deallocate(*(cur - 1), block_size);
                        }
                    }
                }else{
                    map_resize(num_blk_require, false);
                }
            }

            void map_resize(size_type num_new_blocks, bool front){
                auto new_map_size = MyStl::max(_map_size * 2, _map_size + num_new_blocks);
                decltype(_map) new_map = _get_map_al().allocate(new_map_size);
                decltype(_map) new_beg, new_end;

                if (front){
                    //keep the # of blocks at the back unchanged
                    size_t block_at_back = ((_map + _map_size) - _end.map_node) - 1;
                    new_end = new_map + (new_map_size - block_at_back);
                    new_beg = new_end - _map_size;
                    create_blocks_n(new_beg, _map_size);
                    for (auto cur_new = new_beg, cur_original = _begin.map_node; 
                         cur_original != _end.map_node; 
                         ++cur_new, ++cur_original){
                             *cur_new = *cur_original;
                    }
                }else{
                    size_t block_at_front = _begin.map_node - _map;
                    new_beg = new_map + block_at_front;
                    new_end = new_beg + _map_size;
                    create_blocks_n(new_beg, _map_size);
                    for (auto cur_new = new_beg, cur_original = _begin.map_node; 
                         cur_original != _end.map_node; 
                         ++cur_new, ++cur_original){
                             *cur_new = *cur_original;
                    }
                }

                _get_map_al().deallocate(_map, _map_size);
                _map = new_map;
                _map_size = new_map_size;
                _begin = iterator(new_beg, *new_beg + (_begin.cur - _begin.first));
                _end = iterator(new_end - 1, *(new_end - 1) + (_end.cur - _end.first));
            }
    };
} // namespace MyStl


#endif