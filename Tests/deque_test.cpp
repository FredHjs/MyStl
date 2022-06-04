#include <string>

#include "../Headers/Deque.h"
#include "common_test_funcs.h"
#include "../Headers/Vector.h"

int main(){
    MyStl::Deque<char> d_0;
    MyStl::Tests::print(d_0, "deque_0");

    MyStl::Deque<std::string> d_1(6, "test");
    MyStl::Tests::print(d_1, "deque_1");

    MyStl::Deque<int> d_2(15);
    MyStl::Tests::print(d_2, "deque_2");

    MyStl::Vector<std::string> v{"hello", "world", "I", "am", "Fred", "!!!"};
    MyStl::Deque<std::string> d_3(v.begin(), v.end());
    MyStl::Tests::print(d_3, "deque_3");

    MyStl::Deque<int> d_4(d_2);
    MyStl::Tests::print(d_4, "deque_4");

    MyStl::Deque<int> d_5(std::move(d_2));
    MyStl::Tests::print(d_5, "deque_5");

    MyStl::Deque<std::string> d_6{"my", "last", "name", "is", "Huang"};
    MyStl::Tests::print(d_6, "deque_6");

    MyStl::Deque<std::string> d_7 = d_6;
    MyStl::Tests::print(d_7, "deque_7");

    std::cout << d_3.at(0) << d_3[1] << d_3.back() << std::endl;
    d_3[0] = "halo";
    std::cout << d_3.at(0) << d_3[1] << d_3.back() << std::endl;

    d_4.pop_back();
    d_4.pop_front();
    d_4.shrink_to_fit();
    std::cout << d_4.size() << std::endl;

    d_4.erase(d_4.begin(), d_4.end());
    MyStl::Tests::print(d_4, "deque_4_after_erase");

    d_5.push_back(1);
    d_5.push_back(2);
    d_5.push_front(-1);
    d_5.erase(d_5.end() - 2);
    MyStl::Tests::print(d_5, "deque_5_after_modification");

    MyStl::Vector<std::string> s{")))))"};
    d_6.insert(d_6.end(), ":");
    d_6.insert(d_6.end(), s.begin(), s.end());
    d_6.insert(d_6.end(), {")", ")", ")"});
    MyStl::Tests::print(d_6, "deque_6_after_modification");
    return 0;
}