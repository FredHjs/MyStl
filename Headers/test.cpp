#include "Vector.h"
#include <iostream>

int main(){
    MyStl::Vector<int> v{1,2,3,4,5,6};
    std::cout << v.capacity() << v.size();
    return 0;
}