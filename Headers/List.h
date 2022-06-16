#ifndef LIST_H
#define LIST_H

#include "Iterator.h"
#include "Algorithm.h"
#include <cassert>
#include <memory>

namespace MyStl{
    template<typename T> struct Node;

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

    template<typename T, typename Pointer, typename Reference>
    class List_Iterator: public MyStl::Iterator<Bidirectional_Iterator_Tag, T, ptrdiff_t, Pointer, Reference>{
        public:
            using value_type = T;
            using reference = Reference;
            using pointer = Pointer;
            using base_ptr = Node_Base<T>*;
            using node_ptr = Node<T>*;

        private:
            base_ptr _node;

        public:
            List_Iterator() = default;

            List_Iterator(const List_Iterator& other): _node(other._node){}

            List_Iterator(List_Iterator&& other): _node(other._node){other._node = nullptr;}

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
            using iterator = List_Iterator<T, T*, T&>;
            using const_iterator = List_Iterator<T, const T*, const T&>;
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

        public:
        /* member access */
        iterator begin() noexcept {return iterator(_end->_next);}

        iterator end() noexcept {return iterator(_end);}

        public:
        /* modifiers */
        void clear(){
            tidy();
        }

        private:
        /* helpers */
        void fill_init_n(size_type count, const value_type& value){
            try{
                _end = _get_node_al().allocate(1);
                _end->set_init_status();

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
                _get_node_al().construct(p_node, std::forward(args)...);
            }catch(...){
                _get_node_al().deallocate(p_node, 1);
                throw;
            }
            
            if (at){
                p_node->_previous = at->_previous;
                at->_previous = p_node;
                p_node->_next = at;
                p_node->_previous->next = p_node;
            }

            return p_node;
        }

        void tidy(){
            auto cur = _end->_next;
            while(cur != _end){
                _get_al().destroy(&(cur->as_node()->_val));     //has to use address of _val to call its dtor
                _get_node_al().deallocate(cur->as_node(), 1);

                cur = cur->next;
            }

            _end->set_init_status();
            _size = 0;
        }

    };
}

#endif