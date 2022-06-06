#include "common_test_funcs.h"
#include "..\Headers\Vector.h"

using MyStl::Vector;

int main(){
    Vector<int> v1;
    Vector<char> v2(9, 'a');
    Vector<std::string> v3(8);
    Vector<char> v4(v2.begin(), v2.begin() + 3);
    Vector<std::string> v5(v3);
    Vector<char> v6(std::move(v4));
    Vector<std::string> v7{"hello", "world", "hello", "universe"};

    Vector<char> v8, v9;
    Vector<int> v10;
    v8 = v2;
    v9 = std::move(v2);
    MyStl::Tests::print(v9, "vector_9");
    v10 = {1,2,3,4,5,6,7,8,9};

    std::cout << v7.front() << v7.back() << std::endl;
    std::cout << v7.at(2) << v7[1] << std::endl;

    v9.insert(v9.begin(), 'b');
    v9.insert(v9.end(), {'q', 'w', 'e', 'r'});
    v9.insert(v9.end(), 2, 'z');
    v9.insert(v9.begin(), 3, 'c');
    MyStl::Tests::print(v9, "vector_9");
    std::cout << v9.size() << " " << v9.capacity() << std::endl;
    v9.shrink_to_fit();
    std::cout << v9.size() << " " << v9.capacity() << std::endl;

    v7.resize(18, "hello");
    MyStl::Tests::print(v7, "vector_7");
    
    return 0;
}
