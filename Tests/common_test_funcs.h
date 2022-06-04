#pragma once

#include <iostream>

using std::cout;
using std::endl;

namespace MyStl{
namespace Tests{

    template<typename Container> void 
    print(Container& container, std::string container_name){
        if (container.size() == 0){
            std::cout << container_name << " is empty!" << std::endl;
            return;
        }
        cout << container_name << ": ";
        for (auto &e : container){cout << e << " ";}
        cout << endl;
    }
    
}
}

