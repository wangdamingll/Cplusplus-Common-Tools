#include <iostream>
#include <string>
#include "BinaryHeap.hpp"
using namespace std;

// Test program
struct Test{
    Test()= default;
    Test(int value,uint32_t key):value(value),key(key){}
    bool operator<(const Test& anther)const{
        return value<anther.value;
    }

    bool operator>(const Test& anther)const{
        return value>anther.value;
    }

    int value{0}; //cmp value
    uint32_t key{0};//hash table key
};

int main(){
    binaryheap::MinHeap<Test> h(std::less<Test>{});//min heap
    //binaryheap::MaxHeap<Test> h(std::greater<Test>{});//max heap

    h.Push(Test{1,11});
    h.Push(Test{5,55});
    h.Push(Test{3,33});
    h.Push(Test{3,34});
    h.Push(Test{2,22});
    h.Push(Test{4,44});
    h.Push(Test{8,88});
    h.Push(Test{7,77});
    h.Push(Test{6,66});
    h.Push(Test{9,99});

    //remove internal data
    h.Remove(55);
    h.Remove(33);
    h.Remove(77);


    Test x ;
    while(!h.Empty()){
        h.Pop( x );
        std::cout<<"pop:value:"<<x.value<<" key:"<<x.key<<std::endl;
        //h.Print();
    }
    std::cout<<std::endl;
    return 0;
}
