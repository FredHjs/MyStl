#ifndef MYSTL_DEQUE_H
#define MYSTL_DEQUE_H

#include <memory>
#include <assert.h>
#include <stdexcept>

#include "Iterator.h"
#include "Algorithm.h"

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
            static constexpr size_type block_size = sizeof(T) < 512 ? 512/sizeof(T) : 1;

        private:
            map_ptr map_node;
            value_ptr cur;
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
            }

            Deque_Iterator& operator++(int){
                auto temp = *this;
                ++(*this);
                return temp;
            }

            Deque_Iterator& operator--(){
                if (cur == first){
                    change_node_by(-1);
                    cur = last;
                }
                --cur;
            }

            Deque_Iterator& operator--(int){
                auto temp = *this;
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
            using pointer = allocator_type::pointer;
            using const_pointer = allocator_type::const_pointer;
            using iterator = Deque_Iterator<value_type, value_type&, value_type*>;
            using const_iterator = Deque_Iterator<value_type, const value_type&, const value_type*>;
            using reverse_iterator = Reverse_Iterator<iterator>;
            using const_reverse_iterator = Reverse_Iterator<const_iterator>;

            using map_ptr = pointer*;
            using map_allocator = std::allocator<pointer>;

            static constexpr size_type block_size = sizeof(T) < 512 ? 512/sizeof(T) : 1;

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
                init_map_n(0);
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
            /* capacity */
            size_type size() const noexcept{
                return _end - _begin;
            }

            bool empty() const noexcept{
                return _begin == _end;
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

        private:
            /* helpers */
            void init_map_n(size_type num_elem){
                size_type num_block = (num_elem % block_size) ? (num_elem / block_size + 1) 
                                                                : (num_elem / block_size);
                size_type map_size = MyStl::max(num_block + 2, 8);

                try{
                    _map = _get_map_al().allocate(map_size);
                }catch(...){
                    _map = nullptr;
                    throw;
                }

                map_ptr block_begin = _map + (map_size - num_block) / 2;

                try{
                    create_blocks_n(block_begin, num_block);
                }catch(...){
                    _get_map_al().deallocate(_map, map_size);
                    _map = nullptr;
                    throw;
                }
                
                _begin.change_node_to(block_begin);
                _end.change_node_to(block_begin + num_block);
                _begin.cur = _begin.first;
                _end.cur = _end.first + num_elem % block_size;
            }

            void create_blocks_n(map_ptr begin, size_type n){
                auto cur = begin;

                try{
                    for (size_type i = 0; i <= n; ++i, ++cur){
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
    };
} // namespace MyStl


#endif