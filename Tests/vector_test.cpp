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
    v10 = {1,2,3,4,5,6,7,8,9};
}
