#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <unordered_map>
#include <limits>
using namespace std;

namespace binaryheap{
/*
BinaryHeap class
******************PUBLIC OPERATIONS*********************
void Push( x )       --> Push x
Pop( minItem )       --> Remove (and optionally return) smallest item
T Top( )  --> Return smallest item
bool Empty( )        --> Return true if empty; else false
void Clear( )        --> Remove all items
void Remove()        --> Remove heap intenal data

attention:
T is can be int string double userclass and so on

if T is user class:
1. T must have value filed and value type is integral, be used to cmp function.
2. T must have key filed, be used hash table
3. T must override operator < or >,be used to cmp function
4. assume T is string or userclass when T is class
*/

//common T template
template <typename T, typename CmpFun,typename Enable=void>
class BinaryHeap {};

//specail T is int
template <typename T, typename CmpFun>
class BinaryHeap<T,CmpFun,typename std::enable_if<std::is_same<T,int>::value>::type> {};

//specail T is string
template <typename T, typename CmpFun>
class BinaryHeap<T,CmpFun,typename std::enable_if<std::is_same<T,std::string>::value>::type> {};

//special T is user class
template <typename T,typename CmpFun>
class BinaryHeap<T,CmpFun,typename std::enable_if<!std::is_same<T,std::string>::value
                 && std::is_class<T>::value && std::is_integral<decltype(T().value)>::value>::type>{
private:
    using CmpType = decltype(T().value);//get T operator fun parameter type
    using Key = decltype(T().key);  //get T hash table key

public:
    explicit BinaryHeap(CmpFun&& cmpF, int capacity=5000 ):cmpF(std::forward<CmpFun>(cmpF)),
                                                           currentSize{0},capacity(capacity+1),array(capacity+1){}
    explicit BinaryHeap(CmpFun&& cmpF, const vector<T>& items):cmpF(std::forward<CmpFun>(cmpF)),
                                                               array(items.size()+10),currentSize{items.size()}{
        for( int i=0; i<items.size();++i){
            array[i+1] = items[i];
        }
        BuildHeap();
    }

    bool Empty()const{
        return currentSize==0;
    }

    /**
     * Find the top item in the priority queue.
     * Return the top item.
     * O(1)
     */
    const T& Top() const{
        return array[1];
    }

    /**
     * Push item x, allowing duplicates.
     * O(logN)
     */
    void Push(const T& x){
        if( currentSize == array.size() - 1 ){
            array.resize( array.size()*2);
        }

        T copy = x;
        int hole = ++currentSize;
        array[hole] = std::move(x);
        PercolateUp(hole);
    }


    /**
     * Push item x, allowing duplicates.
     * O(logN)
     */
    void Push(T&& x){
        if(currentSize == array.size()-1){
            array.resize( array.size() * 2 );
        }

        int hole = ++currentSize;
        array[hole] = std::move(x);
        PercolateUp(hole);
    }

    /**
     * Remove the top item.
     * O(logN)
     */
    void Pop(){
        DelKey(array[1].key);
        array[1] = std::move(array[currentSize--]);
        PercolateDown(1);
        if(Empty()){
            keyMap.clear();
        }
    }

    /**
     * Remove the top item and place it in top.
     * O(logN)
     */
    void Pop(T& top){
        DelKey(array[1].key);
        top = std::move(array[1]);
        array[1] = std::move(array[currentSize--]);
        PercolateDown(1);
        if(Empty()){
            keyMap.clear();
        }
    }

    /**
     * Remove the Internal item.
     * O(2logN) -> O(logN)
     */
    void Remove(const Key& key){
        int hole = GetHole(key);
        if(hole==0){
            return;
        }
        DelKey(key);
        array[hole].value = value; //这里假设比较值为整数值
        PercolateUp(hole);
        Pop();
    }

    /**
     * clear all item.
     */
    void Clear(){
        currentSize = 0;
        array.resize(capacity);
        keyMap.clear();
    }

public:
    void Print(){//Debug Function
        for(int i=1;i<=currentSize;i++){
            std::cout<<"index:"<<i<<" key:"<<array[i].key<<" ";
        }
        std::cout<<std::endl;

        for(auto& it:keyMap){
            std::cout<<"key:"<<it.first<<" index:"<<it.second<<" ";
        }
        std::cout<<std::endl;

    }
private:
    /**
     * Establish heap order property from an arbitrary
     * arrangement of items. Runs in linear time.
     * O(MlogN)
     */
    void BuildHeap(){
        for(int i = currentSize/2; i>0;--i){
            PercolateDown(i);
        }
    }

    /**
     * Internal method to percolate down in the heap.
     * hole is the index at which the percolate begins.
     * O(logN)
     */
    void PercolateDown( int hole ){
        int child;
        T tmp = std::move(array[hole]);
        for( ;hole*2<=currentSize; hole=child ){
            child = hole*2;
            if(child!=currentSize && cmpF(array[child+1],array[child])){
                ++child;
            }
            if(cmpF(array[child],tmp)){
                array[hole] = std::move(array[child]);
                AddKey(array[hole].key,hole);
            }else{
                break;
            }
        }
        array[hole] = std::move(tmp);
        AddKey(array[hole].key,hole);
    }

    /**
     * Internal method to percolate up in the heap.
     * hole is the index at which the percolate begins.
     * O(logN)
     */
    void PercolateUp(int hole){
        array[0] = std::move(array[hole]);
        for(;hole>1 && cmpF(array[0],array[hole/2]);hole/=2){
            array[hole] = std::move(array[hole/2]);
            AddKey(array[hole].key,hole);
        }
        array[hole] = std::move(array[0]);
        AddKey(array[hole].key,hole);
    }

    /**
     * Internal method to add key in the hash table.
     * key is T key, hole is the index.
     */
    void AddKey(const Key& key, int hole){
        keyMap[key]=hole;
    }

    /**
     * Internal method to get hole by T key in the hash table.
     * key is T key, hole is the index.
     */
    int GetHole(const Key& key){
        const auto& iter = keyMap.find(key);
        if(iter!=keyMap.end()){
            return iter->second;
        }
        return 0;
    }

    /**
     * Internal method to delete key in the hash table.
     * key is T key, hole is the index.
     */
    void DelKey(const Key& key){
        const auto& iter = keyMap.find(key);
        if(iter!=keyMap.end()){
            keyMap.erase(iter);
        }
    }

private:
    CmpType value {std::is_same<CmpFun,decltype(std::less<T>{})>::value
                    ?std::numeric_limits<CmpType>::min():std::numeric_limits<CmpType>::max()}; //cmpF value, T is min or max
    CmpFun cmpF;                  // cmpF function

    int currentSize {0};        // Number of elements in heap
    int capacity {0};           // array capacity
    std::vector<T> array;       // The heap array
    std::unordered_map<Key,int> keyMap;//key hash table
};

template<typename T>
using  MinHeap = BinaryHeap<T,decltype(std::less<T>{})>;
template<typename T>
using  MaxHeap = BinaryHeap<T,decltype(std::greater<T>{})>;

}

#endif
