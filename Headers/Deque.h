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

            Deque_Iterator& operator++(){
                
            }

            private:
                /* helpers */
                void change_node_by(difference_type n){
                    map_node += n;
                    first = cur = *map_node;
                    last = *map_node + block_size;
                }
    };
} // namespace MyStl


#endif