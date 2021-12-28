#include "common_test_funcs.h"
#include "..\Headers\Array.h"
#include <stdexcept>

using std::cout;
using std::endl;

int main(){
    MyStl::Array<int, 10> a1;
    MyStl::Array<char, 3> a2{'a', 'b', 'c'};
    MyStl::Array<std::string, 10> a3{"hello", "world"};
    MyStl::Array<int, 10> a4;

    try{
        cout << a3.at(0) << endl;
        cout << a2.at(3) << endl;
    }catch(std::exception& e){
        cout << e.what() << endl;
    }

    cout << a3.front() << '\t' << a3.front()[0] << endl;
    cout << a3.back() << endl;

    a1.fill(1000);
    MyStl::Tests::print(a1, "a1");

    a4.fill(100);
    a1.swap(a4);
    MyStl::Tests::print(a1, "a1");
    MyStl::Tests::print(a4, "a4");

    for (MyStl::Array<char, 3>::iterator i = a2.begin(); i != a2.end(); ++i){
        cout << *i << '\t';
    }

    cout << endl;


    //MyStl::Array<int, 0> err;         //fails static_assert -- doesn't compile

    return 0;
}
