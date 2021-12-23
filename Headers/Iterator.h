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
}
