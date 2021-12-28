#pragma once

#include <iostream>

using std::cout;
using std::endl;

namespace MyStl{
namespace Tests{

    template<typename Container> void 
    print(Container& container, std::string container_name){
        cout << container_name << ": ";
        for (auto &e : container){cout << e << " ";}
        cout << endl;
    }
    
}
}
