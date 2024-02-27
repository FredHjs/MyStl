/* This header file implements RB tree class template, 
    an internal data structure for MtStl */
#include "Iterator.h"
#include <set>
namespace MyStl
{
    enum _RB_tree_color {
        _red = 0,
        _black = 1
    };

    template <typename T> class _RB_tree_node;

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

        bool _is_lchild() {
            return _parent->_left == this;
        }

        template<typename T>
        inline _RB_tree_node<T>* _get_node() {return reinterpret_cast<_RB_tree_node<T>*>(this);}
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

        _Base_ptr inline _get_base() {return this;}

    };

    template <class T, class pointer, class reference>
    class _RB_tree_iterator : public MyStl::Iterator<MyStl::Bidirectional_Iterator_Tag, T, ptrdiff_t, pointer, reference> {
    public:
        using _Base_ptr = typename _RB_tree_node_base::_Base_ptr;
        using _Node_Ptr = _RB_tree_node<T>*;
        using _Node_base = _RB_tree_node_base;
        using _Self = _RB_tree_iterator<T, pointer, reference>;

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
        void increment() noexcept {
            if (_node_base->_right) {
                _node_base = _Node_base::_min(_node_base->_right);
            } else {
                auto p = _node_base->_parent;
                while (p->_right == _node_base) {
                    _node_base = p;
                    p = p->_parent;
                }

                if (_node_base->_right != p) {
                    // if we are incrementing on the maximum node of the tree,
                    // the result of _node_base will be the header, which
                    // will result in UB if dereferenced.
                    _node_base = p;
                }
            }
        }

        void decrement() noexcept {
            if (_node_base->_left) {
                _node_base = _Node_base::_max(_node_base->_left);
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

    private:
        typedef _RB_tree_node_base _Base_type;
        typedef _Base_type* _Base_ptr;
        typedef _RB_tree_node<TVal> _Node_type;
        typedef _Node_type* _Node_ptr;
        typedef _RB_tree<TKey, TVal, Compare> _Self;

        inline allocator_type _get_al() {return allocator_type();}

    private:
        _Base_ptr _header;
        size_type _size;
        Compare _key_compare;

        _Base_ptr& root() {return _header->_parent;}
        _Base_ptr& max_node() {return _header->_right;}
        _Base_ptr& min_node() {return _header->_left;}

        void default_init() {
            _header->_parent = nullptr;

            // must not set to nullptr in case begin() or end() called on empty tree
            _header->_left = _header->_right = _header;  

            _header->_color = _RB_tree_color::_red;
            _size = 0;
            _key_compare = Compare();
        }

    public:
        /* constructors */
        _RB_tree() {default_init();}

        _RB_tree(const _RB_tree& rhs) {
            default_init();

            copy_tree(rhs);
        }

        _RB_tree(_RB_tree&& rhs)
            : _header(std::move(rhs._header)),
              _size(rhs._size),
              _key_compare(rhs._key_compare) {}

        _Self& operator=(const _RB_tree& rhs) {
            reset();
            copy_tree(rhs);
            return *this;
        }

        _Self& operator=(_RB_tree&& rhs) {
            reset();

            _header = std::move(rhs._header);
            _size = rhs._size;
            _key_compare = rhs._key_compare;

            return *this;
        }

    private:
        // n should be passed in as the newly inserted node
        void insert_fix(_Base_ptr n) noexcept {
            n->_set_red();

            while (root() != n && n->_parent->_is_red()) {
                // n->_parent->_is_red() ==> n->_parent != root()
                _Base_ptr grandparent = n->_parent->_parent;
                _Base_ptr uncle;
                if (n->_parent->_is_lchild())
                    uncle = grandparent->_right;
                else
                    uncle = grandparent->_left;
                
                if (uncle && uncle->_is_red()) {
                    // grandparent must have been black
                    // try re-coloring
                    uncle->_set_black();
                    n->_parent->_set_black();
                    grandparent->_set_red();
                    n = grandparent;
                } else {
                    // re-coloring can't keep balance, must rotate, 4 cases:
                    if (n->_parent->_is_lchild()) {
                        if (!(n->_is_lchild())) {
                            // LR
                            n = n->_parent;
                            rotate_left(n);
                        }

                        // LL
                        n->_parent->_set_black();
                        grandparent->_set_red();
                        rotate_right(grandparent);
                    } else {
                        if (n->_is_lchild()) {
                            // RL
                            n = n->_parent;
                            rotate_right(n);
                        }

                        // RR
                        n->_parent->_set_black();
                        grandparent->_set_red();
                        rotate_left(grandparent);
                    }

                    // while loop will terminate after this iteration since n->_parent is set to black on all paths 
                    break;
                }
            }

            root()->_set_black();
        }

        void rotate_left(_Base_ptr pivot) {
            auto rchild = pivot->_right;
            pivot->_right = rchild->_left;

            if (rchild->_left) {
                rchild->_left->_parent = pivot;
            }

            rchild->_left = pivot;
            rchild->_parent = pivot->_parent;
            

            if (root() == pivot)
                root() = rchild;
            else if (pivot->_is_lchild())
                pivot->_parent->_left = rchild;
            else
                pivot->_parent->_right = rchild;

            pivot->_parent = rchild;
        }

        void rotate_right(_Base_ptr pivot) {
            auto lchild = pivot->_left;
            pivot->_left = lchild->_right;

            if (lchild->_right) {
                lchild->_right->_parent = pivot;
            }

            lchild->_right = pivot;
            lchild->_parent = pivot->_parent;
            

            if (root() == pivot)
                root() = lchild;
            else if (pivot->_is_lchild())
                pivot->_parent->_left = lchild;
            else
                pivot->_parent->_right = lchild;

            pivot->_parent = lchild;
        }

        template<class...Args>
        _Node_ptr construct_node(Args&& ...args) {
            _Node_ptr ptr = _get_al().allocate(1);
            try{
                _get_al().construct(ptr->_val_ptr(), std::forward<Args>(args)...);
                ptr->_parent = ptr->_left = ptr->_right = 0;
            } catch (...) {
                _get_al().deallocate(ptr);
                throw;
            }

            return ptr;
        }

        void destroy_node(_Node_ptr n) {
            _get_al().destroy(n->_val_ptr());
            _get_al().deallocate(n);
        }

        // Recursively create a copy of the subtree with rhs as root and 
        // return the root of the copied tree, where p is the copied root's 
        // parent. Caller must ensure rhs != nullptr when calling.
        _Base_ptr copy_from(_Base_ptr rhs, _Base_ptr p) {
            _Base_ptr root = construct_node(rhs->_get_node<TVal>()->_val);
            
            try{
                root->_parent = p;

                if (rhs->_left) {
                    root->_left = copy_from(rhs->_left, root);
                }

                if (rhs->_right) {
                    root->_right = copy_from(rhs->_right, root);
                }

                return root;
            } catch (...) {
                erase_from(root);
                throw;
            }
            
        }

        // recursively destroy the subtree rooted at n
        void erase_from(_Base_ptr n) {
            if (n->_left) erase_from(n->_left->_get_node<TVal>());
            if (n->_right) erase_from(n->_right->_get_node<TVal>());

            destroy_node(n);
        }

        void reset() {
            erase_from(root());

            default_init();
        }

        void copy_tree(const _RB_tree& rhs) {
            root() = copy_from(rhs.root(), _header);
            max_node() = _Base_type::_max(root());
            min_node() = _Base_type::_min(root());

            _size = rhs._size;
            _key_compare = rhs._key_compare;
        }
    };
} // namespace MyStl
