/* This header file implements RB tree, an internal data structure for MtStl. */
#include "Iterator.h"
#include <set>
namespace MyStl
{
    enum _RB_tree_color {
        _red = 0,
        _black = 1
    };

    class _RB_tree_node_base {
    public:
        typedef _RB_tree_node_base* _Base_ptr;
        typedef const _RB_tree_node_base* _Const_Base_ptr;

        _Base_ptr _parent;
        _Base_ptr _left;
        _Base_ptr _right;
        _RB_tree_color _color;

        static _Base_ptr _min(_Base_ptr ptr) {
            while (ptr->_left != nullptr)
                ptr = ptr->_left;
            return ptr;
        }

        static _Const_Base_ptr _min(_Const_Base_ptr ptr) {
            while (ptr->_left != nullptr)
                ptr = ptr->_left;
            return ptr;
        }

        static _Base_ptr _max(_Base_ptr ptr) {
            while (ptr->_right != nullptr)
                ptr = ptr->_right;
            return ptr;
        }

        static _Const_Base_ptr _max(_Const_Base_ptr ptr) {
            while (ptr->_right != nullptr)
                ptr = ptr->_right;
            return ptr;
        }

        bool _is_red() {
            return _color == _RB_tree_color::_red;
        }

        void _set_red() {
            _color = _RB_tree_color::_red;
        }

        void _set_black() {
            _color = _RB_tree_color::_black;
        }

        void _flip_color() {
            _color = (_color == _RB_tree_color::_black) ? _RB_tree_color::_red : _RB_tree_color::_black;
        }
    };

    template <class T>
    class _RB_tree_node : public _RB_tree_node_base {
    public:
        typedef _RB_tree_node<T>* _Node_ptr;

        T _val;

        T* _val_ptr() {
            return std::__addressof(_val);
        }

        const T* _val_ptr() const {
            return std::__addressof(_val);
        } 

    };

    template <class T, class pointer, class reference>
    class _RB_tree_iterator : public MyStl::Iterator<MyStl::Bidirectional_Iterator_Tag, T, ptrdiff_t, pointer, reference> {
    public:
        using _Base_ptr = typename _RB_tree_node_base::_Base_ptr;
        using _Node_Ptr = _RB_tree_node<T>*;
        using _Self = _RB_tree_iterator<T>;

        _Base_ptr _node_base;

        _RB_tree_iterator() = default;

        _RB_tree_iterator(_Base_ptr n) {
            _node_base = n;
        }

        _RB_tree_iterator(_Node_Ptr n) {
            _node_base = n;
        }

        _RB_tree_iterator(const _RB_tree_iterator& other) {
            _node_base = other._node_base;
        }

        _RB_tree_iterator(_RB_tree_iterator&& other) {
            _node_base = other._node_base;
        }

        _Self& operator++() {
            increment();
            return *this;
        }

        _Self operator++(int) {
            _Self tmp = *this;
            increment();
            return tmp;
        }

        _Self& operator--() {
            decrement();
            return *this;
        }

        _Self operator--(int) {
            _Self tmp = *this;
            decrement();
            return tmp;
        }

        pointer operator->() {
            return static_cast<_Node_Ptr>(_node_base)->_val_ptr();
        }

        reference operator*() {
            return *static_cast<_Node_Ptr>(_node_base)->_val_ptr();
        }

    private:
        void increment() {
            if (_node_base->_right) {
                _node_base = _node_base->_right->_min();
            } else {
                auto p = _node_base->_parent;
                while (p->_right == _node_base) {
                    _node_base = p;
                    p = p->_parent;
                }

                if (_node_base->_right != p) {
                    // if we are incrementing on the maximum node of the tree, the result of _node_base will be the header
                    // which will result in UB if dereferenced.
                    _node_base = p;
                }
            }
        }

        void decrement() {
            if (_node_base->_left) {
                _node_base = _node_base->_left->_max();
            } else {
                auto p = _node_base->_parent;
                while (p->_left == _node_base) {
                    _node_base = p;
                    p = p->_parent;
                }

                if (_node_base->_left != p) {
                    _node_base = p;
                }
            }
        }
    }; 

    template<typename TKey, typename TVal, typename Compare>
    class _RB_tree {
    public:
        typedef TKey                 key_type;
        typedef TVal                 value_type;
        typedef value_type*          pointer;
        typedef const value_type*    const_pointer;
        typedef value_type&          reference;
        typedef const value_type&    const_reference;
        typedef size_t               size_type;
        typedef ptrdiff_t            difference_type;
        
        typedef std::allocator<_RB_tree_node<TVal>> allocator_type;

        typedef _RB_tree_iterator<TVal, pointer, reference> iterator;
        typedef _RB_tree_iterator<TVal, const_pointer, const_reference> const_iterator;
        typedef MyStl::Reverse_Iterator<iterator> reverse_iterator;
        typedef MyStl::Reverse_Iterator<const_iterator> const_reverse_iterator;

        static_assert(std::__is_invocable<Compare, const key_type&, const key_type&>{}, 
                        "Compare predicate must be invocable.");
    };
} // namespace MyStl
