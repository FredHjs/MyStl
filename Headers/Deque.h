#ifndef MYSTL_DEQUE_H
#define MYSTL_DEQUE_H

#include <memory>

#include "Iterator.h"

namespace MyStl
{
    template<typename T, typename Reference, typename Pointer>
    class Deque_Iterator: Iterator<Random_Access_Iterator_Tag, T, ptrdiff_t, Pointer, Reference>{
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
            using iterator = Deque_Iterator<value_type>;
            using const_iterator = Deque_Iterator<const value_type>;
            using reverse_iterator = Reverse_Iterator<iterator>;
            using const_reverse_iterator = Reverse_Iterator<const_iterator>;
    };
} // namespace MyStl


#endif