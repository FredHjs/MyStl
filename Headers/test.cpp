#include "Vector.h"
#include <iostream>

int main(){
    MyStl::Vector<int> v{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
    //v = {2,3,4,5,6};
    MyStl::Vector<char> v2(18);
    MyStl::Vector<int> v3;
    std::cout << v.capacity() << " " << v.size();
    std::cout << std::endl;

    std::cout << v3.size() << " " << v3.capacity() << std::endl;
    
    std::cout << v.front() << " " << v.back() << " " << v[9] << std::endl;

    v = v3;
    try{
        v.at(0) = 2;
        std::cout << v.at(0) << std::endl;
    }catch(std::out_of_range const& ex){
        std::cout << ex.what() << std::endl;
    }
    return 0;
}