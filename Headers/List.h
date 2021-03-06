#ifndef LIST_H
#define LIST_H

#include "Iterator.h"
#include "Algorithm.h"
#include <cassert>
#include <memory>

namespace MyStl{
    template<typename T> struct Node;
    template<typename T> class List;
    template<typename T> class List_Const_Iterator;

    template<typename T>
    struct Node_Base{
        Node_Base<T>* _previous;
        Node_Base<T>* _next;

        Node_Base() = default;

        Node_Base(const Node_Base<T>*& prev, const Node_Base<T>*& next): _previous(prev), _next(next){}

        Node<T>* as_node(){return static_cast<Node<T>*>(this);}

        void set_init_status(){_previous = _next = this;}
    };

    template<typename T>
    struct Node: public Node_Base<T>{
        using value_type = T;

        Node() = default;

        Node(const value_type& val): _val(val){}

        Node(const Node_Base<T>*& prev, const Node_Base<T>*& next, const value_type& val): Node_Base<T>(prev, next), _val(val){}

        Node_Base<T>* as_base(){return static_cast<Node_Base<T>*>(this);}

        value_type _val;
    };

    template<typename T>
    class List_Iterator: public MyStl::Iterator<Bidirectional_Iterator_Tag, T>{
        friend class List<T>;
        friend class List_Const_Iterator<T>;
        public:
            using value_type = T;
            using reference = T&;
            using pointer = T*;
            using base_ptr = Node_Base<T>*;
            using node_ptr = Node<T>*;

        private:
            base_ptr _node;

        public:
            List_Iterator() = default;

            List_Iterator(const List_Iterator& other): _node(other._node){}

            List_Iterator(base_ptr bp): _node(bp){}

            List_Iterator(node_ptr np): _node(np->as_base()){}

            T& operator*() const {return _node->as_node()->_val;}

            T* operator->() const {return &(operator*());}

            List_Iterator& operator++(){
                assert(_node);
                _node = _node->_next;
                return *this;
            }

            List_Iterator& operator--(){
                assert(_node);
                _node = _node->_previous;
                return *this;
            }

            List_Iterator operator++(int){
                assert(_node);
                auto temp = *this;
                _node = _node->_next;
                return temp;
            }

            List_Iterator operator--(int){
                assert(_node);
                auto temp = *this;
                _node = _node->_previous;
                return temp;
            }

            bool operator==(const List_Iterator& rhs) const {
                return _node == rhs._node;
            }

            bool operator != (const List_Iterator& rhs) const {
                return _node != rhs._node;
            }
    };

    template<typename T>
    class List_Const_Iterator: public MyStl::Iterator<Bidirectional_Iterator_Tag, T>{       //make iterator convertible to const_iterator
        friend class List<T>;
        public:
            using value_type = T;
            using reference = const T&;
            using pointer = const T*;
            using base_ptr = Node_Base<T>*;
            using node_ptr = Node<T>*;

        private:
            base_ptr _node;

        public:
            List_Const_Iterator() = default;

            List_Const_Iterator(const List_Const_Iterator& other): _node(other._node){}

            List_Const_Iterator(const List_Iterator<T>& other): _node(other._node){}

            List_Const_Iterator(base_ptr bp): _node(bp){}

            List_Const_Iterator(node_ptr np): _node(np->as_base()){}

            T& operator*() const {return _node->as_node()->_val;}

            T* operator->() const {return &(operator*());}

            List_Const_Iterator& operator++(){
                assert(_node);
                _node = _node->_next;
                return *this;
            }

            List_Const_Iterator& operator--(){
                assert(_node);
                _node = _node->_previous;
                return *this;
            }

            List_Const_Iterator operator++(int){
                assert(_node);
                auto temp = *this;
                _node = _node->_next;
                return temp;
            }

            List_Const_Iterator operator--(int){
                assert(_node);
                auto temp = *this;
                _node = _node->_previous;
                return temp;
            }

            bool operator==(const List_Const_Iterator& rhs) const {
                return _node == rhs._node;
            }

            bool operator != (const List_Const_Iterator& rhs) const {
                return _node != rhs._node;
            }
    };

    /* circular doubly-linked list, maintains a pointer to the pass-the-end sentinel node */
    template<typename T>
    class List{
        public:
        /* type defs */
            using value_type = T;
            using allocator_type = std::allocator<T>;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type&;
            using const_reference = const value_type&;
            using pointer = typename allocator_type::pointer;
            using const_pointer = typename allocator_type::const_pointer;
            using iterator = List_Iterator<T>;
            using const_iterator = List_Const_Iterator<T>;
            using reverse_iterator = Reverse_Iterator<iterator>;
            using const_reverse_iterator = Reverse_Iterator<const_iterator>;

            using base_ptr = Node_Base<T>*;
            using node_ptr = Node<T>*;
            using node_type = Node<T>;
            using base_type = Node_Base<T>;
            using base_allocator = std::allocator<base_type>;
            using node_allocator = std::allocator<node_type>;

            allocator_type inline _get_al() {return allocator_type();}
            base_allocator inline _get_base_al(){return base_allocator();}
            node_allocator inline _get_node_al() {return node_allocator();}

        private:
            base_ptr _end;  //pass-the-end sentinel node
            size_type _size;

        public:
        /* ctor and dtor */
        List(): _end(_get_node_al().allocate(1)), _size(0){
            _end->set_init_status();
        }

        List(size_type count, const T& value){
            fill_init_n(count, value);
        }

        explicit List(size_type count){
            fill_init_n(count, value_type());
        }

        template<class InputIt,typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true> 
        List(InputIt first, InputIt last){
            _size = MyStl::distance(first, last);

            try{
                init_end_node();

                for (; first != last; ++first){
                    create_node(_end, *first);
                }
            }catch(...){
                tidy();
                _get_base_al().deallocate(_end, 1);     //node_base only has pointers as member variables, so memory can be directly released
                _end = nullptr;
                throw;
            }
        }

        List(const List& other): List(other.begin(), other.end()){}

        List(List&& other): _size(other._size), _end(other._end){
            other._size = 0;
            other._end = nullptr;
        }

        List(std::initializer_list<T> init): List(init.begin(), init.end()){}

        ~List(){
            tidy();
            _get_base_al().deallocate(_end, 1);
            _end = nullptr;
            _size = 0;
        }

        List& operator=(const List& other){
            if (this != &other){
                copy(other.begin(), other.end());
            }

            return *this;
        }

        List& operator=(List&& other){
            List<T> temp(std::move(other));
            swap(temp);
        }

        List& operator=(std::initializer_list<T> init){
            copy(init.begin(), init.end());
            return *this;
        }

        void assign(size_type count, const T& value){
            if (count >= _size){
                auto insert_start = MyStl::fill(begin(), end(), value);
                insert(end(), count - _size, value);
            }else{
                auto erase_start = begin();
                while(count > 0){
                    *(erase_start++) = value;
                    --count;
                }

                erase(erase_start, end());
            }
        }

        template<class InputIt>
        void assign(InputIt first, InputIt last){
            copy(first, last);
        }

        void assign(std::initializer_list<T> ilist){
            copy(ilist.begin(), ilist.end());
        }

        public:
        /* member access */
        reference front(){return *begin();}

        const_reference front() const {return *begin();}

        reference back(){return *(--end());}

        const_reference back() const {return *(--end());}

        public:
        /* iterators */
        iterator begin() noexcept {return iterator(_end->_next);}

        const_iterator begin() const noexcept {return const_iterator(_end->_next);}

        const_iterator cbegin() const noexcept {return const_iterator(_end->_next);}

        iterator end() noexcept {return iterator(_end);}

        const_iterator end() const noexcept {return const_iterator(_end);}

        const_iterator cend() const noexcept {return const_iterator(_end);}

        reverse_iterator rbegin() noexcept {return reverse_iterator(end());}

        const_reverse_iterator rbegin() const noexcept {return const_reverse_iterator(end());}

        const_reverse_iterator crbegin() const noexcept {return const_reverse_iterator(end());}

        reverse_iterator rend() noexcept {return reverse_iterator(begin());}

        const_reverse_iterator rend() const noexcept {return const_reverse_iterator(begin());}

        const_reverse_iterator crend() const noexcept {return const_reverse_iterator(begin());}

        public:
        /* capacity */
        bool empty() const noexcept {return _size == 0;}

        size_type size() const noexcept {return _size;}

        public:
        /* modifiers */
        void clear(){
            tidy();
        }

        void swap(List& other){
            std::swap(_size, other._size);
            std::swap(_end, other._end);
        }

        template<class InputIt, typename std::enable_if<MyStl::Is_Input_Iterator<InputIt>::value, bool>::type = true>
        iterator insert(const_iterator pos, InputIt first, InputIt last){
            base_ptr _pos = pos._node;
            if (first == last) return _pos;

            auto count = MyStl::distance(first, last);
            auto other_beg = create_node(nullptr, *first), other_end = other_beg;
            ++first;
            while(first != last){
                auto node =  create_node(nullptr, *first); //avoid assigning each time
                other_end->_next = node->as_base();
                node->_previous = other_end;
                other_end = node;

                ++first;
            }

            _size += count;
            _pos->_previous->_next = other_beg->as_base();
            _pos->_previous = other_end->as_base();
            other_beg->_previous = _pos->_previous;
            other_end->_next = _pos;

            return other_beg;
        }

        iterator insert(const_iterator pos, size_type count, const T& value){
            auto _pos = pos._node;
            if (count == 0) return _pos;

            auto _count = count;
            auto other_beg = create_node(nullptr, value), other_end = other_beg;
            --_count;

            while (_count > 0){
                auto node = create_node(nullptr, value);
                other_end->next = node->as_base();
                node->prev = other_end;
                other_end = node;

                --_count;
            }
            
            _size += count;
            _pos->_previous->next = other_beg->as_base();
            _pos->_previous = other_end->as_base();
            other_beg->_previous = _pos->_previous;
            other_end->next = _pos._node;

            return other_beg;
        }

        iterator insert(const_iterator pos, const T& value){
            auto node = create_node(pos._node->as_base(), value);
            ++_size;
            return iterator(node);
        }

        iterator insert(const_iterator pos, T&& value){
            auto node = create_node(pos._node, std::move(value));
            ++_size;
            return iterator(node);
        }

        iterator insert(const_iterator pos, std::initializer_list<T> ilist){
            return insert(ilist.begin(), ilist.end());
        }

        template<class... Args>
        iterator emplace(const_iterator pos, Args&&... args){
            auto node = create_node(pos._node->as_base(), std::forward<Args>(args)...);
            ++_size;
            return iterator(node);
        }

        iterator erase(const_iterator pos){
            assert(!empty());

            base_ptr node = pos._node, ret = node->_next;
            take_out_nodes(node, node);

            delete_node(node->as_node());
            --_size;

            return iterator(ret);
        }

        iterator erase(const_iterator first, const_iterator last){
            assert(!empty());

            base_ptr _first = first._node, _last = last._node;

            if (first != last){
                take_out_nodes(_first, _last->_previous);

                while(first != last){
                    delete_node(first._node->as_node());
                    ++first;
                }
                _size += MyStl::distance(first, last);
            }

            return iterator(_last);
        }

        void push_back(const T& value){
            create_node(_end, value);
            ++_size;
        }

        void push_back(T&& value){
            create_node(_end, std::move(value));
            ++_size;
        }

        template<class... Args>
        void emplace_back(Args&&... args){
            create_node(_end, std::forward<Args>(args)...);
            ++_size;
        }

        void pop_back(){
            assert(!empty());

            base_ptr _back = _end->_previous;
            take_out_nodes(_back, _back);

            delete_node(_back);
            --_size;
        }

        void push_front(const T& value){
            create_node(_end->_nextn, value);
            ++_size;
        }

        void push_front(T&& value){
            create_node(_end->_next, std::move(value));
            ++_size;
        }

        template<class... Args>
        void emplace_front(Args&&... args){
            create_node(_end->_next, std::forward<Args>(args)...);
            ++_size;
        }

        void pop_front(){
            assert(!empty());

            base_ptr _front = _end->_next;
            take_out_nodes(_front, _front);

            delete_node(_front->as_node());
            --_size;
        }

        void resize(size_type count){
            resize(count, T());
        }

        void resize(size_type count, const value_type& value){
            long long diff = MyStl::llabs(_size - count);
            if(count <= _size){
                iterator new_end = end();
                while (diff > 0){
                    --new_end;
                    --diff;
                }

                erase(new_end, end());
                _size -= (_size - count);
            }else{
                while (diff > 0){
                    create_node(_end, value);
                    --diff;
                }

                _size += (count - _size);
            }
        }

        public:
        /* operations */
        template <class Compare> 
        void merge(List&& other, Compare comp){
            if (&other != this){
                base_ptr cur = _end->_next, other_cur = other._end->_next;

                for (; cur != _end && other_cur != other._end; cur = cur->_next){
                    if (comp(other_cur->as_node()->_val, cur->as_node()->_val)){
                        // find the interval on other that fits comp
                        base_ptr interval_end = other_cur->_next;
                        const_reference val = cur->as_node()->_val;

                        while(interval_end != other._end && comp(interval_end->as_node()->_val, val)){
                            interval_end = interval_end->_next;
                        }
                        auto next_start = interval_end;
                        interval_end = interval_end->_previous;
                        //now interval_end points to the last node that comp(interval_end->as_node()->_val, val) == true
                        other.take_out_nodes(other_cur, interval_end);
                        this->link_nodes_at(other_cur, interval_end, cur);

                        other_cur = next_start;
                    }else{
                        continue;
                    }
                }
                
                if (other_cur != other._end){
                    auto other_last = other._end->_previous;
                    other.take_out_nodes(other_cur, other_last);
                    this->link_nodes_at(other_cur, other_last, this->_end);
                }

                this->_size += other._size;
                other._size = 0;
            }
        }

        void merge(List&& other){
            merge(std::move(other), [](const T& val_1, const T& val_2) -> bool {return val_1 < val_2;});
        }

        void splice(const_iterator pos, List&& other){
            if (&other != this && !other.empty()){
                auto other_first = other._end->_next, other_last = other._end->_previous;

                other.take_out_nodes(other_first, other_last);
                link_nodes_at(other_first, other_last, pos._node);

                _size += other._size;
                other._size = 0;
            }
        }

        void splice(const_iterator pos, List&& other, const_iterator it){
            if (&other != this && !other.empty()){
                auto other_node = it._node;

                other.take_out_nodes(other_node, other_node);
                link_nodes_at(other_node, other_node, pos._node);

                ++_size;
                --other._size;
            }
        }

        void splice(const_iterator pos, List&& other, const_iterator first, const_iterator last){
            if (&other != this && !other.empty()){
                auto other_first = first._node, other_last = last._node->_previous;
                auto num_elem = MyStl::distance(first, last);

                other.take_out_nodes(other_first, other_last);
                link_nodes_at(other_first, other_last, pos._node);

                _size += num_elem;
                other._size -=num_elem;
            }
        }

        void remove(const T& value){
            remove_if([&](const T& x) -> bool{return x == value;});
        }

        template<class UnaryPredicate> 
        void remove_if(UnaryPredicate p){
            iterator cur = begin();
            while(cur != end()){
                if (p(*cur))
                    cur = erase(cur);
                else
                    ++cur;
            }
        }

        template<class BinaryPredicate> 
        void unique(BinaryPredicate p){
            iterator next = begin(), cur = next;
            ++next;
            while(next != end()){
                if (p(*cur, *next)){
                    next = erase(next);
                }else{
                    ++cur;
                    ++next;
                }
            }
        }

        void unique(){
            unique([](const T& cur, const T& next) -> bool{return cur == next;});
        }

        void reverse() noexcept {
            if(!empty() && _size != 1){
                base_ptr cur = _end->_next;
                while(cur != _end){
                    std::swap(cur->_previous, cur->_next);
                    cur = cur->_previous;
                }

                std::swap(_end->_previous, _end->_next);
            }
        }

        void sort(){
            merge_sort(begin(), end(), _size, [](const T& val1, const T& val2) -> bool{return val1 < val2;});
        }

        //recursive merge sort
        template<class Compare>
        void sort(Compare comp){
            merge_sort(begin(), end(), _size, comp);
        }

        private:
        /* helpers */
        //recursively merge sort [first, last)
        template<typename Compare>
        iterator merge_sort(iterator first, iterator last, size_type size, Compare comp){
            if (size == 1) return first;
            else if (size == 2){
                if (comp(*(--last), *first))
                    swap_nodes(first._node, last._node);
                    return last;
                return first;
            }else{
                auto half_size = size / 2;
                iterator first_2 = first;
                MyStl::advance(first_2, half_size);
                first = merge_sort(first, first_2, half_size, comp);
                first_2 = merge_sort(first_2, last, size - half_size, comp);

                iterator last_1 = first_2;
                //the smallest node is either first or first_2
                iterator ret = comp(*first, *first_2) ? first : first_2;

                //merge
                for (;first != last_1 && first_2 != last; ++first){
                    if(comp(*first_2, *first)){
                        iterator interval_end = first_2;
                        ++interval_end;

                        while(interval_end != last && comp(*interval_end, *first)){
                            ++interval_end;
                        }
                        auto next_start = interval_end;
                        --interval_end;

                        take_out_nodes(first_2._node, interval_end._node);
                        link_nodes_at(first_2._node, interval_end._node, first._node);

                        first_2 = next_start;
                    }else{
                        continue;
                    }
                }

                return ret;
            }
        }

        //swap two consecutive nodes by pointer manipulations, no invalidations of iterators and/or references
        void swap_nodes(base_ptr node1, base_ptr node2){
            take_out_nodes(node2, node2);
            link_nodes_at(node2, node2, node1);
        }

        void init_end_node(){
            _end = _get_node_al().allocate(1);
            _end->set_init_status();
        }

        void fill_init_n(size_type count, const value_type& value){
            try{
                init_end_node();

                for (size_type i = 0; i < count; ++i){
                    create_node(_end, value);
                }
            }catch(...){
                tidy();
                _get_base_al().deallocate(_end, 1);     //node_base only has pointers as member variables, so memory can be directly released
                _end = nullptr;
                throw;
            }
            _size = count;
        }
        
        template <typename... Args>
        node_ptr create_node(base_ptr at, Args&&...args){
            node_ptr p_node = nullptr;
            try{
                p_node = _get_node_al().allocate(1);
                _get_node_al().construct(p_node, std::forward<Args>(args)...);
            }catch(...){
                _get_node_al().deallocate(p_node, 1);
                throw;
            }
            
            if (at){
                p_node->_previous = at->_previous;
                at->_previous = p_node;
                p_node->_next = at;
                p_node->_previous->_next = p_node;
            }

            return p_node;
        }

        void tidy(){
            if (_end){
                auto cur = _end->_next, next = cur->_next;
                while(cur != _end){
                    delete_node(cur->as_node());

                    cur = next;
                    next = cur->_next;
                }

                _end->set_init_status();
                _size = 0;
            }
        }

        template<typename InputIt>
        void copy(InputIt first, InputIt last){
            auto distance = MyStl::distance(first, last);
            if (distance <= _size){
                auto erase_start = MyStl::copy(first, last, begin());
                erase(erase_start, end());
            }else{
                auto other_cur = first;
                for (auto cur = begin(); cur != end(); ++cur, ++other_cur){
                    *cur = *other_cur;
                }

                insert(end(), other_cur, last);
            }
        }

        void delete_node(node_ptr node){
            _get_al().destroy(&(node->_val));       //has to use address of _val to call its dtor
            _get_node_al().deallocate(node, 1);
        }

        //take out [first, last] and re-link the rest
        void take_out_nodes(base_ptr first, base_ptr last){
            first->_previous->_next = last->_next;
            last->_next->_previous = first->_previous;
        }

        //add [first, last] at a specified position
        void link_nodes_at(base_ptr first, base_ptr last, base_ptr at){
            first->_previous = at->_previous;
            last->_next = at;
            at->_previous->_next = first;
            at->_previous = last;
        }
    };

    /* operators */
    template<class T>
    bool operator==(const List<T>& lhs, const List<T>& rhs){
        if (lhs.size() != rhs.size()) return false;
        for (auto lit = lhs.begin(), rit = rhs.begin(); 
            lit != lhs.end(); 
            ++lit, ++rit){
                if (*lit != *rit) return false;
        }
        return true;
    }
    template <class T>
    bool operator<(const List<T>& lhs, const List<T>& rhs){
        return MyStl::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template <class T>
    bool operator!=(const List<T>& lhs, const List<T>& rhs){
        return !(lhs == rhs);
    }

    template <class T>
    bool operator>(const List<T>& lhs, const List<T>& rhs){
        return rhs < lhs;
    }

    template <class T>
    bool operator<=(const List<T>& lhs, const List<T>& rhs){
        return !(rhs < lhs);
    }

    template <class T>
    bool operator>=(const List<T>& lhs, const List<T>& rhs){
        return !(lhs < rhs);
    }
}

#endif