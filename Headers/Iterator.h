#ifndef MYSTL_ITERATOR_H
#define MYSTL_ITERATOR_H

#include <cstddef>
#include <type_traits>

namespace MyStl{
    template<typename Category, typename T, typename Difference_t = ptrdiff_t, typename Pointer = T*, 
    typename Reference = T&>
    struct Iterator{
        using iterator_category = Category;
        using value_type = T;
        using difference_type = Difference_t;
        using pointer = Pointer;
        using reference = Reference;
    };

    struct Input_Iterator_Tag {};
    struct Output_Iterator_Tag {};
    struct Forward_Iterator_Tag : Input_Iterator_Tag {};
    struct Bidirectional_Iterator_Tag : Forward_Iterator_Tag {};
    struct Random_Access_Iterator_Tag : Bidirectional_Iterator_Tag {};

    template<typename I>
    struct Is_Iterator
    {
        private:
            using one = char;
            struct two {char c[2];};
            template <typename C> static two check(...);
            template <typename C> static one check(typename C::iterator_category*);
        public:
            constexpr static const bool value = (sizeof(check<I>(0)) == sizeof(char));

    };

    template <typename Iter, bool>
    struct Iterator_Traits_Checked {};

    template <typename Iter>
    struct Iterator_Traits_Checked<Iter, true> {
        using iterator_category = typename Iter::iterator_category;
        using value_type = typename Iter::value_type;
        using difference_type = typename Iter::difference_type;
        using pointer = typename Iter::pointer;
        using reference = typename Iter::reference;
    };

    template <typename Iter>
    struct Iterator_Traits : Iterator_Traits_Checked<Iter, Is_Iterator<Iter>::value> {};

    template <typename T>
    struct Iterator_Traits<T*>{
        using iterator_category = Random_Access_Iterator_Tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T*;
        using reference = T&;
    };

    template <typename T>
    struct Iterator_Traits<const T*>{
        using iterator_category = Random_Access_Iterator_Tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;
    };

    template <typename Iter, typename Category, bool = Is_Iterator<Iterator_Traits<Iter>>::value>
    struct Is_Of_Category : 
        std::integral_constant<bool, std::is_convertible<
        typename Iterator_Traits<Iter>::iterator_category, Category>::value>{};

    template <typename Iter, typename Category>
    struct Is_Of_Category<Iter, Category, false> : std::integral_constant<bool, false>{};

    template<typename Iter> 
    struct Is_Input_Iterator : Is_Of_Category<Iter, Input_Iterator_Tag>{};

    template<typename Iter>
    struct Is_Output_Iterator : Is_Of_Category<Iter, Output_Iterator_Tag>{};

    template<typename Iter>
    struct Is_Forward_Iterator : Is_Of_Category<Iter, Forward_Iterator_Tag>{};

    template<typename Iter>
    struct Is_Bidirectional_Iterator : Is_Of_Category<Iter, Bidirectional_Iterator_Tag>{};

    template<typename Iter>
    struct Is_Random_Access_Iterator : Is_Of_Category<Iter, Random_Access_Iterator_Tag>{};


    /* reverse iterator */
    template <typename Iter>
    class Reverse_Iterator{
        public:
            using iterator_type = Iter;
            using iterator_category = typename Iterator_Traits<Iter>::iterator_category;
            using value_type = typename Iterator_Traits<Iter>::value_type;
            using difference_type = typename Iterator_Traits<Iter>::difference_type;
            using pointer = typename Iterator_Traits<Iter>::pointer;
            using reference = typename Iterator_Traits<Iter>::reference; 

        protected:
            Iter _current;

        public:
            /* ctors */
            Reverse_Iterator() = default;

            explicit Reverse_Iterator(iterator_type x) : _current(x){}

            template<class U> Reverse_Iterator(const Reverse_Iterator<U>& other) : _current(other._current){};

            template<class U> reverse_iterator& operator=(const reverse_iterator<U>& other){
                _current = other.base();
            }

        public:
            /* operations */
            Iter base(){return _current;}

            reference operator*() const {
                auto temp = _current;
                return *(--temp);
            }

            pointer operator->() const {
                auto temp = _current;
                return &(*(--temp));
            }

            reference operator[](difference_type n){
                return *(_current - n);
            }

            Reverse_Iterator<Iter> operator+(difference_type n) const {
                return Reverse_Iterator<Iter>(_current - n);
            }

            Reverse_Iterator<Iter> operator-(difference_type n) const {
                return Reverse_Iterator<Iter>(_current + n);
            }

            Reverse_Iterator<Iter>& operator+=(difference_type n){
                this->_current = this->_current - n;
                return *this;
            }

            Reverse_Iterator<Iter>& operator-=(difference_type n){
                this->_current = this->_current + n;
                return *this;
            }

            //pre
            Reverse_Iterator<Iter>& operator++(){
                _current = _current - 1;
                return *this;
            }

            Reverse_Iterator<Iter>& operator--(){
                _current = _current + 1;
                return *this;
            }

            //post
            Reverse_Iterator<Iter> operator++(int){
                auto temp = *this;
                ++(*this);
                return temp;
            }

            Reverse_Iterator<Iter> operator--(int){
                auto temp = *this;
                --(*this);
                return temp;
            }
    };

    template<class Iterator1, class Iterator2>
    bool operator==(const Reverse_Iterator<Iterator1>& lhs, const Reverse_Iterator<Iterator2>& rhs){
        return lhs._current == rhs._current;
    }

    template<class Iterator1, class Iterator2>
    bool operator<(const Reverse_Iterator<Iterator1>& lhs, const Reverse_Iterator<Iterator2>& rhs){
        return rhs._current < lhs._current;
    }

    template<class Iterator1, class Iterator2>
    bool operator!=(const Reverse_Iterator<Iterator1>& lhs, const Reverse_Iterator<Iterator2>& rhs){
        return !(lhs == rhs);
    }

    template<class Iterator1, class Iterator2>
    bool operator<=(const Reverse_Iterator<Iterator1>& lhs, const Reverse_Iterator<Iterator2>& rhs){
        return !(rhs < lhs);
    }

    template<class Iterator1, class Iterator2>
    bool operator>(const Reverse_Iterator<Iterator1>& lhs, const Reverse_Iterator<Iterator2>& rhs){
        return rhs < lhs;
    }

    template<class Iterator1, class Iterator2>
    bool operator>=(const Reverse_Iterator<Iterator1>& lhs, const Reverse_Iterator<Iterator2>& rhs){
        return !(lhs < rhs);
    }

    template<class Iter>
    Reverse_Iterator<Iter>
    operator+(typename Reverse_Iterator<Iter>::difference_type n,const Reverse_Iterator<Iter>& it){
        return Reverse_Iterator<Iter>(it._current - n);
    }

    template<class Iter>
    typename Reverse_Iterator<Iter>::difference_type
    operator-(const Reverse_Iterator<Iter>& lhs, const Reverse_Iterator<Iter>& rhs){
        return lhs._current - rhs._current;
    }
}

#endif
