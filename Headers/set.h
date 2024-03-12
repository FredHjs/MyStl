#ifndef MYSTL_SET_H
#define MYSTL_SET_H

#include <functional>
#include <memory>
#include "tree.h"

namespace MyStl {
template <class Key, class Compare = std::less<Key>>
class set {
public:
    typedef Key                 key_type;
    typedef Key                 value_type;
    typedef Compare             key_compare;
    typedef Compare             value_compare;
    typedef std::allocator<key_type>                             allocator_type;
    typedef std::allocator_traits<allocator_type>::pointer       pointer;
    typedef std::allocator_traits<allocator_type>::const_pointer const_pointer;

private:
    typedef MyStl::_RB_tree<key_type, value_type, 
        std::_Identity<value_type>, key_compare> _Rep_type;

public:
    typedef typename _Rep_type::size_type               size_type;
    typedef typename _Rep_type::difference_type         difference_type;
    typedef typename _Rep_type::reference_type          reference_type;
    typedef typename _Rep_type::const_reference         const_reference;
    typedef typename _Rep_type::iterator                iteraotr;
    typedef typename _Rep_type::const_iterator          const_iterator;
    typedef typename _Rep_type::reverse_iterator        reverse_iterator;
    typedef typename _Rep_type::const_reverse_iterator  const_reverse_iterator;

private:
    _Rep_type _tree;

public:
    /* constructors and destructors */
    set() = default;

    explicit set(const Compare& comp): _tree(comp) {}

    template <class InputIt>
    set(InputIt first, InputIt last): _tree() {
        _tree.insert_unique(first, last);
    }

    set(const set& other): _tree(other._tree) {}

    set(set&& other): _tree(std::move(other._tree)) {}

    set(std::initializer_list<value_type> ilist, const Compare& comp)
        : _tree() {
        _tree._key_compare = comp;
        _tree.insert_unique(ilist.begin(), ilist.end());
    }
};
} // namespace MyStl

#endif