#include "Vector.h"
#include <iostream>
#include <string>
#include <vector>

int main(){
    MyStl::Vector<int> v{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
    //v = {2,3,4,5,6};
    MyStl::Vector<char> v2(18);
    MyStl::Vector<int> v3;
    std::cout << v.capacity() << " " << v.size();
    std::cout << std::endl;

    std::cout << v3.size() << " " << v3.capacity() << std::endl;

    std::cout << v.front() << " " << v.back() << " " << v[9] << std::endl;

    for (auto& i : v){
        std::cout << i << " ";
    }

    std::cout << std::endl;

    try{
        v.at(0) = 2;
        std::cout << v.at(0) << std::endl;
    }catch(std::out_of_range const& ex){
        std::cout << ex.what() << std::endl;
    }
    
    std::string s(9, 'a');

    v2.assign(6, 'g');
    std::cout << v2.size() << " " << v2.capacity() << std::endl;
    v2.shrink_to_fit();
    std::cout << v2.size() << " " << v2.capacity() << std::endl;
    v.erase(v.begin(), v.begin() + 3);
    v.erase(v.end() - 4);
    for (auto& c : v){
        std::cout << c << " ";
    }
    std::cout << std::endl;

    MyStl::Vector<std::string> mvs{"hello", "world"};
    for (auto& s : mvs){
        std::cout << s << " ";
    } 
    std::cout << std::endl;
    mvs.insert(mvs.begin(), 3, "zzz");
    for (auto& s : mvs){
        std::cout << s << " ";
    }

    // std::vector<int> vi;
    // vi.insert();

    std::cout << std::endl;
    v.clear();
    std::cout << (v.empty() ? "empty" : "still not empty...") << std::endl;

    MyStl::Vector<char> temp{'z', 'z', 'z', 'z', 'z'};
    v2.insert(v2.begin(), 10, 'c');
    v2.insert(v2.begin(), temp.begin(), temp.begin() + 3);
    

    v2.resize(20, '3');
    for (auto& c : v2){
        std::cout << c << " ";
    }
    std::cout << std::endl;

    mvs.resize(10, "haha");
    for (auto& s : mvs){
        std::cout << s << " ";
    }
    std::cout << std::endl;
}