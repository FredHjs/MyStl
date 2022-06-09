#ifndef LIST_H
#define LIST_H

#include "Iterator.h"

namespace MyStl{
    template<typename T> struct Node;

    template<typename T>
    struct Node_Base{
        Node_Base<T>* _previous;
        Node_Base<T>* _next;

        Node_Base() = default;

        Node_Base(const Node_Base<T>*& prev, const Node_Base<T>*& next): _previous(prev), _next(next){}

        Node<T>* as_node(){return static_cast<Node<T>*>(this);}
    };

    template<typename T>
    struct Node: public Node_Base<T>{
        using value_type = T;

        Node() = default;

        Node(const value_type& val): _val(val){}

        Node(const Node_Base<T>*& prev, const Node_Base<T>*& next, const value_type& val): node_base(prev, next), _val(val){}

        Node_Base<T>* as_base(){return static_cast<Node_Base<T>*>(this);}

        value_type _val;
    };

    template<typename T>
    class List_Iterator: public MyStl::Iterator<Bidirectional_Iterator_Tag, T>{
        public:
            using value_type = T;
            using reference = T&;
            using pointer = T*;
            using base_pointer = Node_Base<T>*;
            using node_pointer = Node<T>*;

        private:
            base_pointer _node;

        public:
            List_Iterator() = default;

            
    };
}

#endif