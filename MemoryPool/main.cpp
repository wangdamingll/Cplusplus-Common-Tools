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
    virtual ~MemPool()
    {
        for(auto *&p : objs_)
        {
            if(p != nullptr)
            {
                Free(p);
                p = nullptr;
            }
        }

        for(auto *&p : allobjs_)
        {
            if(p != nullptr)
            {
                p->empty = true;
                p->~T();
                Free(p);
                p = nullptr;
            }
        }
    }

public:
    template <typename... Args>
    inline T* Get(Args&&... args)
    {
        T* obj = nullptr;
        if(count_ == 0)
        {
            obj = Alloc(std::forward(args)...);
            obj->empty = false;//防止构造函数不初始化
            obj->index = index_++;
        }else
        {
            -- count_;
            //防止构造函数把index初始化
            int index = objs_[count_]->index;
            obj = new(objs_[count_]) T(std::forward(args)...);
            obj->empty = false;//防止构造函数不初始化
            obj->index = index;
            objs_[count_] = nullptr;
        }

        assert(obj->index != -1);

        if(static_cast<size_t >(index_) >= this->allobjs_.size()){
            this->allobjs_.resize(index_ * 2 + 1);
        }
        this->allobjs_[obj->index] = obj;
        return obj;
    }

    inline void Cycle(T* obj)
    {
        assert(obj != nullptr);
        assert(static_cast<size_t>(obj->index) < allobjs_.size());
        if(count_ < MF)
        {
            //防止析构函数将index也初始化了
            int index = obj->index;
            obj->empty = true;
            obj->~T();
            obj->index = index;
            objs_[count_++] = obj;
            allobjs_[obj->index] = nullptr;
        }else
        {
            allobjs_[obj->index] = nullptr;
            obj->empty = true;
            obj->~T();
            Free(obj);
        }
    }

    inline T* GetByIndex(int index) const
    {
        if(index == -1)
        {
            return nullptr;
        }

        if(static_cast<size_t>(index) < allobjs_.size())
        {
            if(allobjs_[index] != nullptr && (!allobjs_[index]->empty))
            {
                return allobjs_[index];
            }
        }
        return nullptr;
    }

private:
    template <typename... Args>
    static inline T* Alloc(Args&&... args)
    {
        char* p = new char[sizeof(T) + sizeof(int)];
        return new (p) T(std::forward(args)...);
    }

    static inline void Free(T* obj)
    {
        if(obj != nullptr)
        {
            char* p = reinterpret_cast<char*>(obj);
            delete [] p;
        }
    }
private:
    int index_ = 0;
    int count_ = 0;
    std::vector<T*> allobjs_;
    std::array<T*,MF> objs_;
};

class A{
public:
    std::string string = "";
    int index = -1; //内存池中初始索引
    bool empty = false;
    virtual ~A()
    {
        empty = true;
    }
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