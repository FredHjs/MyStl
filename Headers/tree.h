/* This header file implements RB tree class template, 
    an internal data structure for MtStl */
#include "Iterator.h"
#include <assert.h>
#include <memory>
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

        static _Base_ptr _get_successor(_Base_ptr ptr) {
            if (ptr->_right) return _RB_tree_node_base::_min(ptr->_right);

            while (!(ptr->_is_lchild())) {
                ptr = ptr->_parent;
            }

            return ptr->_parent;
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
    };

    template <class T>
    class _RB_tree_node : public _RB_tree_node_base {
    public:
        typedef _RB_tree_node<T>* _Node_ptr;

        T _val;

        T* _val_ptr() {
            return std::addressof(_val);
        }

        const T* _val_ptr() const {
            return std::addressof(_val);
        } 

        _Base_ptr inline _get_base() {return this;}

    };

    template <class T, class pointer, class reference>
    class _RB_tree_iterator
        : public MyStl::Iterator<MyStl::Bidirectional_Iterator_Tag, T, ptrdiff_t, pointer, reference> {
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

    template<typename TKey, typename TVal, typename KeyofValue, typename Compare>
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
        static_assert(std::__is_invocable<KeyofValue, const key_type&, const key_type&>{}, 
                        "Key getter must be invocable.");

    private:
        typedef _RB_tree_node_base _Base_type;
        typedef _Base_type* _Base_ptr;
        typedef const _Base_type _Const_Base_ptr;
        typedef _RB_tree_node<TVal> _Node_type;
        typedef _Node_type* _Node_ptr;
        typedef const _Node_type* _Const_Node_ptr;
        typedef _RB_tree<TKey, TVal, KeyofValue, Compare> _Self;

        inline allocator_type _get_al() {return allocator_type();}

    private:
        _Base_ptr _header;
        size_type _size;
        Compare _key_compare;

        _Base_ptr& root() {return _header->_parent;}
        _Base_ptr& max_node() {return _header->_right;}
        _Base_ptr& min_node() {return _header->_left;}

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

        /* capacity */
        size_type size() {
            return _size;
        }

        bool empty() {
            return _size == 0;
        }

        size_type capacity() {
            return -1;
        }

        /* iterators */
        iterator begin() noexcept {
            return min_node();
        }

        const_iterator begin() const noexcept {
            return min_node();
        }

        iterator end() noexcept {
            return _header;
        }

        const_iterator end() const noexcept {
            return _header;
        }

        const_iterator cbegin() const noexcept {
            return min_node();
        }

        const_iterator cend() const noexcept {
            return _header;
        }

        reverse_iterator rbegin() noexcept {
            return reverse_iterator(end());
        }

        const_reverse_iterator rbegin() const noexcept {
            return reverse_iterator(end());
        }

        reverse_iterator rend() noexcept {
            return reverse_iterator(begin());
        }

        const_reverse_iterator rend()  const noexcept {
            return reverse_iterator(begin());
        }

        const_reverse_iterator crbegin() const noexcept {
            return rbegin();
        }

        const_reverse_iterator crend() const noexcept {
            return rend();
        }

        /* modifiers */
        void clear() noexcept {
            reset();
        }

        std::pair<iterator, bool>
        insert_unique(const_reference val) {
            assert(size() < capacity() && "RB tree has reached capacity");

            auto res = get_insert_unique_pos(KeyofValue()(val));
            
            if (res.second) {
                auto n = construct_node(val);
                return std::make_pair(insert_at_pos(res.second, n, res.first != nullptr), true);
            }

            return std::make_pair(res.first, false);
        }

        template <class P, 
                  typename std::enable_if<std::is_constructible<value_type, P&&>::value, int>::type = 0>
        std::pair<iterator, bool>
        insert_unique(P&& other) {
            return emplace_unique(std::forward<P>(other));
        }

        iterator insert_unique(const_iterator hint, const_reference val) {
            return emplace_hint(hint, val);
        }

        template <class P,
                  typename std::enable_if<std::is_constructible<value_type, P&&>::value, int>::type = 0>
        iterator insert_unique(const_iterator hint, P&& other) {
            return emplace_hint(hint, std::forward<P>(other));
        }

        template <class InputIt>
        void insert_unique(InputIt first, InputIt last) {
            size_type num = MyStl::distance(first, last);
            assert(size() + num <= capacity() && "number of nodes will exceed RB tree capacity");

            while (num) {
                insert_unique(*first);
                ++first;
                --num;
            }
        }

        void insert_unique(std::initializer_list<value_type> ilist) {
            insert_unique(ilist.begin(), ilist.end());
        }

        template <class...Args>
        std::pair<iterator, bool>
        emplace_unique(Args&& ...args) {
            assert(size() < capacity() && "RB tree has reached capacity");
            auto n = construct_node(std::forward<Args>(args)...);

            auto res = get_insert_unique_pos(get_key(n));

            if (res.second) {
                return std::make_pair(insert_at_pos(res.second, n, res.first != nullptr), true);
            }

            delete_node(n);
            return std::make_pair(res.first, false);
        }

        template <class...Args>
        iterator emplace_hint(iterator hint, Args&& ...args) {
            assert(size() < capacity() && "RB tree has reached capacity");

            auto n = construct_node(std::forward<Args>(args)...);

            try {
                auto res = get_emplace_hint_unique_pos(hint, get_key(n));
                if (res.second) {
                    return insert_at_pos(res.second, n, res.first != nullptr);
                }
                delete_node(n);
                return iterator(res.first);
            } catch (...) {
                delete_node(n);
                throw;
            }
        }

        iterator erase(iterator pos) {
            assert(pos._node_base != _header);

            auto n = get_node(pos._node_base);
            auto ret = pos;
            ++ret;

            delete_fix(n);
            delete_node(n);
            return ret;
        }

        iterator erase(const_iterator pos) {
            return erase(iterator(const_cast<_Base_ptr>(pos._node_base)));
        }

        iterator erase(const_iterator first, const_iterator last) {
            if (first == begin() && last == end()) {
                reset();
            } else {
                while (first != last) {
                    erase(first++);
                }
            }

            return iterator(const_cast<_Base_ptr>(last._node_base));
        }

        size_type erase(const key_type& key) {
            auto i = find(key);
            if (i == end()) return 0;
            erase(i);
            return 1;
        }

        void swap(_RB_tree& other) {
            if (this != other) {
                std::swap(_header, other._header);
                std::swap(_size, other._size);
                std::swap(_key_compare, other._key_compare);
            }
        }

        /* Look up */
        size_type count_unique (const key_type& key) const {
            return find(key) == end() ? 0 : 1;
        }

        std::pair<iterator, iterator>
        equal_range_unique(const key_type& key) {
            auto i = find(key);
            auto j = i;
            return i == end() ? std::make_pair(end(), end()) : std::make_pair(i, ++j);
        }

        std::pair<const_iterator, const_iterator>
        equal_range_unique(const key_type& key) const {
            auto i = find(key);
            auto j = i;
            return i == end() ? std::make_pair(end(), end()) : std::make_pair(i, ++j);
        }

        iterator find(const key_type& key) {
            auto i = lower_bound(key);
            return (i == end() || _key_compare(key, get_key(i._base_node))) ? end() : i;
        }

        const_iterator find(const key_type& key) const {
            auto i = lower_bound(key);
            return (i == end() || _key_compare(key, get_key(i._base_node))) ? end() : i;
        }

        iterator lower_bound(const key_type& key) {
            _Base_ptr lb = _header;
            _Base_ptr cur = root();

            while (lb) {
                if (_key_compare(get_key(cur), key)) {
                    // cur.key < key
                    cur = cur->_right;
                } else {
                    // cur.key >= key
                    lb = cur;
                    cur = cur->_left;
                }
            }

            return iterator(lb);
        }

        const_iterator lower_bound(const key_type& key) const {
            _Base_ptr lb = _header;
            _Base_ptr cur = root();

            while (lb) {
                if (_key_compare(get_key(cur), key)) {
                    // cur.key < key
                    cur = cur->_right;
                } else {
                    // cur.key >= key
                    lb = cur;
                    cur = cur->_left;
                }
            }

            return const_iterator(lb);
        }

        iterator upper_bound(const key_type& key) {
            _Base_ptr ub = _header;
            _Base_ptr cur = root();

            while (ub) {
                if (_key_compare(key, get_key(cur))) {
                    // cur.key > key
                    ub = cur;
                    cur = cur->_left;
                } else {
                    // cur.key <= key
                    cur = cur->_right;
                }
            }

            return iterator(ub);
        }

        const_iterator upper_bound(const key_type& key) const {
            _Base_ptr ub = _header;
            _Base_ptr cur = root();

            while (ub) {
                if (_key_compare(key, get_key(cur))) {
                    // cur.key > key
                    ub = cur;
                    cur = cur->_left;
                } else {
                    // cur.key <= key
                    cur = cur->_right;
                }
            }

            return const_iterator(ub);
        }

        /* Observers */
        Compare key_comp() const {
            return _key_compare;
        }



    private:
        // p is the parent of insertion position
        // left_indicator ==> insert left, but !left_indicator =\=> insert right
        iterator insert_at_pos(_Base_ptr p, _Base_ptr n, bool left_indicator) {
            n->_parent = p;

            if (p == _header) {
                root() = n;
                min_node() = n;
                max_node() = n;
            }

            bool insert_left = left_indicator || _key_compare(get_key(n), get_key(p));

            if (insert_left) {
                p->_left = n;
                if (p == min_node())
                    min_node() = n;
            } else {
                p->_right = n;
                if (p == max_node())
                    max_node() = n;
            }

            ++_size;
            insert_fix(n);
            return iterator(n);
        }

        // first base ptr contains supposed insertion position if there's duplicate
        // first != nullptr ==> should insert to left, but should insert to left =\=> first != nullptr
        // second base ptr is the parent of insertion position (nullptr if duplication found)
        std::pair<_Base_ptr, _Base_ptr>
        get_insert_unique_pos(const key_type& key) {
            using Res = std::pair<_Base_ptr, _Base_ptr>;
            auto cur = root();
            auto p = _header;
            bool comp = true;

            while (cur) {
                p = cur;
                comp = _key_compare(key, get_key(cur));
                cur = comp ? cur->_left : cur->_right;
            }

            iterator i = iterator(p);

            if (comp) {
                if (p == _header || p == min_node())
                    return Res(cur, p);
                else
                    --i;
            }

            if (_key_compare(KeyofValue()(*i), key))
                return Res(cur, p);
            
            return Res(i._node_base, nullptr);
        }

        std::pair<_Base_ptr, _Base_ptr>
        get_emplace_hint_unique_pos(iterator hint, const key_type& key) {
            using Res = std::pair<_Base_ptr, _Base_ptr>;

            if (hint._node_base == _header) {
                // hint == end()
                if (size() && _key_compare(get_key(max_node()), key))
                    return Res(nullptr, max_node());

                return get_insert_unique_pos(key);
            } else if (_key_compare(key, get_key(hint._node_base))) {
                // key < hint
                iterator before = hint;
                if (hint._node_base == min_node())
                    return Res(min_node(), min_node());     // should insert to left child of min_node()
                else if (_key_compare(get_key((--before)._node_base), key)) {
                    // before < key < hint
                    if (!before._node_base->_right)
                        return Res(nullptr, before._node_base);
                    // if before._node_base->_right != nullptr, it means hint._left == nullptr
                    else
                        return Res(hint._node_base, hint._node_base);   
                } else {
                    return get_insert_unique_pos(key);
                }
            } else if (_key_compare(get_key(hint._node_base), key)) {
                // hint < key
                iterator after = hint;
                if (hint._node_base == max_node())
                    return Res(nullptr, max_node());
                else if (_key_compare(key, get_key(key, (++after)._node_base))) {
                    // hint < key < after
                    if (!after._node_base->_left)
                        return Res(after._node_base, after._node_base);
                    else
                        return Res(nullptr, hint._node_base);
                } else {
                    return get_insert_unique_pos(key);
                }
            } else {
                // key == hint
                return (hint._node_base, nullptr);
            }
        }

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

        // n should be passed in as the ptr to the node to be deleted.
        // This function will modify the node pointed to by n and make
        // sure that n is good to be deleted for the next step
        void delete_fix(_Base_ptr n) noexcept {
            auto to_delete = (n->_left == nullptr || n->_right == nullptr) ? n : _Base_type::_get_successor(n);
            // now to_delete has at most 1 child, let replacement be the child or nullptr
            auto replacement = to_delete->_left ? to_delete->_left : to_delete->_right;
            _Base_ptr replacement_parent;

            if (to_delete == n) {
                // n has at most 1 child
                replacement_parent = to_delete->_parent;

                if (replacement) {
                    replacement->_parent = to_delete->_parent;
                }
                
                if (to_delete == root()) {
                    root() = replacement;
                } else if (to_delete->_is_lchild()) {
                    to_delete->_parent->_left = replacement;
                } else {
                    to_delete->_parent->_right = replacement;
                }

                if (min_node() == to_delete) {
                    min_node() = replacement;
                }
                if (max_node() == to_delete) {
                    max_node() == replacement;
                }
            } else {
                // n has 2 children, replacement is the right child of to_delete (or nullptr)
                // to_delete doesn't have a left child
                // we use to_delete to replace n
                to_delete->_left = n->_left;
                n->_left->_parent = to_delete;

                if (to_delete != n->_right) {
                    replacement_parent = to_delete->_parent;

                    if (replacement) replacement->_parent = to_delete->_parent;

                    to_delete->_parent->_right = replacement;
                    to_delete->_right = n->_right;
                    n->_right->_parent = to_delete;
                } else {
                    replacement_parent = to_delete;
                }

                if (n == root()) {
                    root() = to_delete;
                } else if (n->_is_lchild()) {
                    n->_parent->_left = to_delete;
                } else {
                    n->_parent->_right = to_delete;
                }

                to_delete->_parent = n->_parent;
                // the colors should not be replaced
                std::swap(to_delete->_color, n->_color);
                to_delete = n;
            }

            // now deleting n will not cause any connection/pointer issues,
            // we now rebalance the tree (only nececcary if to_delete is black)
            if (!(to_delete->_is_red())) {
                while (replacement != root() && (!replacement || !(replacement->_is_red()))) {
                    _Base_ptr sibling;
                    if(replacement == replacement_parent->_left)
                        sibling = replacement_parent->_right;
                    else
                        sibling = replacement_parent->_left;
                    
                    if (sibling->_is_red) {
                        // implies that both replacement_parent and sibling's children are black
                        sibling->_set_black();
                        replacement_parent->_set_red();
                        if (replacement == replacement_parent->_left) {
                            rotate_left(replacement_parent);
                            sibling = replacement_parent->_right;
                        } else {
                            rotate_right(replacement_parent);
                            sibling = replacement_parent->_left;
                        }  
                    }
                    // now both replacement and sibling are black (or nullptr)
                    if ((sibling->_left == nullptr || !(sibling->_left->_is_red())) &&
                        (sibling->_right == nullptr || !(sibling->_right->_is_red()))) {
                        // both of sibling's children are black
                        sibling->_set_red();
                        replacement = replacement_parent;
                        replacement_parent = replacement_parent->_parent;
                    } else {
                        // at least one of sibling's children is red
                        if (replacement == replacement_parent->_left) {
                            if (sibling->_right == nullptr || !(sibling->_right->_is_red())) {
                                sibling->_left->_set_black();     // sibling->_left must not be nullptr and must be red
                                sibling->_set_red();
                                rotate_right(sibling);
                                sibling = replacement_parent->_right;
                            }

                            sibling->_color = replacement_parent->_color;
                            replacement_parent->_set_black();
                            if(sibling->_right)
                                sibling->_right->_set_black();
                            rotate_left(replacement_parent);
                            break;
                        } else {
                            if (sibling->_left == nullptr || !(sibling->_left->_is_red())) {
                                sibling->_right->_set_black();     // sibling->_right must not be nullptr and must be red
                                sibling->_set_red();
                                rotate_left(sibling);
                                sibling = replacement_parent->_left;
                            }

                            sibling->_color = replacement_parent->_color;
                            replacement_parent->_set_black();
                            if(sibling->_left)
                                sibling->_left->_set_black();
                            rotate_right(replacement_parent);
                            break;
                        }
                    }
                }

                if (replacement)
                    replacement->_set_black();
            }
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

        void delete_node(_Node_ptr n) {
            _get_al().destroy(n->_val_ptr());
            _get_al().deallocate(n);
        }

        // Recursively create a copy of the subtree with rhs as root and 
        // return the root of the copied tree, where p is the copied root's 
        // parent. Caller must ensure rhs != nullptr when calling.
        _Base_ptr copy_from(_Base_ptr rhs, _Base_ptr p) {
            _Base_ptr root = construct_node(get_node(rhs)->_val);
            
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
            if (n->_left) erase_from(n->_left);
            if (n->_right) erase_from(n->_right);

            delete_node(n);
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

        void default_init() {
            _header->_parent = nullptr;

            // must not set to nullptr in case begin() or end() called on empty tree
            _header->_left = _header->_right = _header;  

            _header->_color = _RB_tree_color::_red;
            _size = 0;
            _key_compare = Compare();
        }        

        static _Node_ptr get_node(_Base_ptr n) {
            return static_cast<_Node_ptr>(n);
        }

        static _Const_Node_ptr get_node(_Const_Base_ptr n) {
            return static_cast<_Const_Node_ptr>(n);
        }

        static const key_type& get_key(_Const_Base_ptr n) {
            return KeyofValue()(*(get_node(n)->_val_ptr()));
        }

        static const key_type& get_key(_Const_Node_ptr n) {
            return KeyofValue()(*(n->_val_ptr()));
        }
    };
} // namespace MyStl
