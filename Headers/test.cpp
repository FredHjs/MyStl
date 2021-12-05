#include "Vector.h"
#include <iostream>

int main(){
    MyStl::Vector<int> v{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
    //v = {2,3,4,5,6};
    MyStl::Vector<char> v2(18);
    MyStl::Vector<int> v3;
    std::cout << v.capacity() << " " << v.size();
    std::cout << std::endl;
    std::cout << v2.capacity() << " " << v2.size();
    std::cout << std::endl;
    std::cout << v3.capacity() << " " << v3.size();
    return 0;
}