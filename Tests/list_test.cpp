#include"../Headers/List.h"
#include "common_test_funcs.h"
#include <iostream>
#include <string>

int main(){
    MyStl::List<int> l1;
    MyStl::Tests::print(l1, "list_1");

    MyStl::List<char> l2(7, 'f');
    MyStl::Tests::print(l2, "list_2");

    MyStl::List<int> l3(7);
    MyStl::Tests::print(l3, "list_3");

    MyStl::List<char> l4(l2.begin(), l2.end());
    MyStl::Tests::print(l4, "list_4");

    MyStl::List<char> l5(std::move(l2));
    MyStl::Tests::print(l5, "list_5");

    MyStl::List<std::string> l6{"hello", "world", "I'm", "Fred", "Huang"};
    MyStl::Tests::print(l6, "list_6");

    l1 = l3;
    l1.front() = 1;
    l1.back() = 2;
    MyStl::Tests::print(l1, "list_1");

    l1.unique();
    MyStl::Tests::print(l1, "list_1");

    l1.remove_if([](const int& x) -> bool{return x == 0;});
    MyStl::Tests::print(l1, "list_1");

    l6.erase(l6.cbegin());
    l6.insert(l6.cend(), "!!!");
    l6.push_back("!!!");
    l6.pop_front();
    MyStl::Tests::print(l6, "list_6");

    l5.resize(15, 'h');
    l5.push_front('?');
    MyStl::Tests::print(l5, "list_5");

    MyStl::List<int> l7{-3,-1,1,3,5,7,9};
    MyStl::List<int> l8{-2,0,2,4,6,8,10};
    l7.merge(std::move(l8));
    MyStl::Tests::print(l7, "list_7");

    MyStl::List<int> l9{11,85,-1,0,1,3};
    MyStl::List<int> l10{6,8,4,3,-2,-9};
    l9.splice(l9.end(), std::move(l10));
    l9.sort();
    MyStl::Tests::print(l9, "list_9");

    return 0;
}