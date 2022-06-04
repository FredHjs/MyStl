#include "../Headers/Deque.h"
#include "common_test_funcs.h"

int main(){
    MyStl::Deque<int> d(6, 6);
    d.push_back(6);
    MyStl::Tests::print(d, "deque");
    return 0;
}