#include <cstddef>

template<typename Category, typename T, typename Difference_t = std::ptrdiff_t, typename Pointer = T*, 
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
    constexpr static const bool value = (sizeof(check<I>(0)) == sizeof(char));
    private:
        using one = char;
        struct two {char c[2];};
        template <typename C> static two check(...);
        template <typename C> static one check(typename C::iterator_category*);
};

