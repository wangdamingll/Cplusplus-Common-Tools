#include<string>
#include<functional>
#include<memory>
#include<unordered_map>
#include <iostream>
using namespace std;

const int MaxObjectNum = 10;

class NonCopyable{
protected:
    NonCopyable() = default;
    virtual ~NonCopyable() = default;
public:
    NonCopyable(const NonCopyable&) = delete; // 禁用复制构造
    NonCopyable& operator = (const NonCopyable&) = delete;    // 禁用赋值构造
};

template<typename T>
class ObjectPool : NonCopyable{
    template<typename... Args>
    using Constructor = std::function<std::unique_ptr<T>(Args...)>;
    using DelType = std::function<void(T*)>;

public:
    // 默认创建多少个对象
    template<typename... Args>
    void Init(size_t num, Args&&... args){
        if (num<= 0 || num> MaxObjectNum)
            throw std::logic_error("object num out of range.");
        auto constructName = typeid(Constructor<Args...>).name(); // 不区分引用
        for (size_t i = 0; i <num; i++){
            m_object_map.emplace(constructName, std::unique_ptr<T>(new T(std::forward<Args>(args)...)));
        }
    }

    // 从对象池中获取一个对象
    template<typename... Args>
    std::unique_ptr<T,DelType> Get(){
        string constructName = typeid(Constructor<Args...>).name();
        auto range = m_object_map.equal_range(constructName);
        for (auto it = range.first; it != range.second; ++it){
            std::unique_ptr<T, DelType> ptr(it->second.release(), [this,constructName](T* p){
                m_object_map.emplace(constructName,unique_ptr<T>(p));    //删除函数执行的操作
            });
            m_object_map.erase(it);
            return ptr;
        }
        return nullptr;
    }

    inline size_t Size() const{
        return m_object_map.size();
    }
    inline bool empty() const{
        return m_object_map.empty();
    }
private:
    unordered_multimap<string, std::unique_ptr<T>> m_object_map;
};

/*-----------------------------*/
struct BigObject{
    BigObject() = default;
    explicit BigObject(int a) {};
    BigObject(const int& a, const int& b){}
    void Print(const string& str){
        cout <<str<< endl;
    }
};

template <typename T>
void Print(T&& p, const string& str){
    if (p != nullptr){
        p->Print(str);
    }
}

void TestObjPool(){
    ObjectPool<BigObject> pool;
    pool.Init(2); // 初始化对象池，初始创建两个对象
    {
        auto p = pool.Get();
        Print(std::move(p), "p");
        pool.Get();
        std::cout<<"pool.size()="<<pool.Size()<<std::endl;
    }// 出了作用域之后，对象池返回出来的对象又会自动回收
    std::cout<<"pool.size()="<<pool.Size()<<std::endl;
    {
        pool.Get();
        pool.Get();
        std::cout<<"pool.size()="<<pool.Size()<<std::endl;
    }
    std::cout<<"pool.size()="<<pool.Size()<<std::endl;

    // 对象池支持重载构造函数
    {
        pool.Init(2, 1);
        auto p4 = pool.Get<int>();
        Print(std::move(p4), "p4");
        pool.Init(2, 1, 2);
        auto p5 = pool.Get<int, int>();
        Print(std::move(p5), "p5");
    }
    std::cout<<"pool.size()="<<pool.Size()<<std::endl;
}

int main(){
    TestObjPool();
    return 0;
}
