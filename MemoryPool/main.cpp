#include <iostream>
#include<string>
#include<cassert>
#include<memory>
#include <functional>
#include<vector>
#include <array>

using namespace std;

template <typename T, int MF = 128>
class MemPool{
public:
    using CycleFun = std::function<void(T*)>;
public:
    inline MemPool(){objs_.fill(nullptr);}
    virtual ~MemPool(){
        for(auto &p : objs_){
            if(p!= nullptr){
                Free(p);//实际上调用2次obj->~T();1次释放真实物理内存
                p= nullptr;
            }
        }
    }

public:
    template <typename... Args>
    inline T* Get(Args&&... args){
        T* obj = nullptr;
        if(count_==0){
            obj = Alloc(std::forward(args)...);
        }else{
            -- count_;
            obj = new(objs_[count_]) T(std::forward(args)...);
            objs_[count_] = nullptr;
        }

        obj->index = index_++; //要求T中有index字段
        if(static_cast<size_t >(index_)>=this->allobjs_.size()){
            this->allobjs_.resize(index_*2 + 1);
        }
        this->allobjs_[obj->index] = obj;
        return obj;
    }

    inline void Cycle(T* obj){
        assert(obj!= nullptr);
        if(count_<MF){
            obj->~T();
            objs_[count_++] = obj;
            allobjs_[obj->index] = nullptr;
        }else{
            assert(static_cast<size_t >(obj->index)<allobjs_.size());
            allobjs_[obj->index] = nullptr;
            delete obj;
        }
    }

    inline T* GetByIndex(int index) const{
        if(index < MF){
            if(index > count_){
                return allobjs_[index];
            }
        }
        if(index< static_cast<size_t>(allobjs_.size())){
            return allobjs_[index];
        }
        return nullptr;
    }

private:
    template <typename... Args>
    static inline T* Alloc(Args&&... args){
        return new T(std::forward(args)...);
    }

    static inline void Free(T* obj){
        if(obj!= nullptr){
            delete obj;
        }
    }
private:
    int index_=0;
    int count_=0;
    std::vector<T*> allobjs_;
    std::array<T*,MF> objs_;
};

class A{
public:
    std::string string = "";
    int index = -1; //内存池中初始索引
};


int main(){
    MemPool<A> APoll;
    {
        A* aptr = APoll.Get();
        aptr->string = "test a";

        A* bptr = APoll.Get();
        aptr->string = "test b";

        APoll.Cycle(aptr);
        APoll.Cycle(bptr);

        A* cptr = APoll.Get();
        aptr->string = "test c";
        APoll.Cycle(cptr);

        std::unique_ptr<A,MemPool<A>::CycleFun> dptr(APoll.Get(),[&](A*p){APoll.Cycle(p);});
        dptr->string="test d";
    }
    {
        A* eptr = APoll.Get();
        eptr->string = "test e";
        APoll.Cycle(eptr);
    }

    return 0;
}