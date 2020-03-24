#pragma once
#include<string>
#include<unordered_map>
#include<memory>
#include<functional>
#include <any>
#include "NonCopyable.hpp"

using namespace std;

class IocContainer : NonCopyable
{
public:
    IocContainer() = default;
    ~IocContainer() = default;

    template<class T, typename Depend, typename... Args>
    typename std::enable_if<!std::is_base_of<T,Depend>::value>::type RegisterType(const string& strKey){
        std::function<T* (Args&&...)> function = [](Args&&... args){ return new T(new Depend(std::forward<Args>(args)...)); };//通过闭包擦除了参数类型
        RegisterType(strKey, std::move(function));
    }

    template<class T, typename Depend, typename... Args>
    typename std::enable_if<std::is_base_of<T,Depend>::value>::type RegisterType(const string& strKey){
        std::function<T* (Args&&...)> function = [](Args&&... args){ return new Depend(std::forward<Args>(args)...); };
        RegisterType(strKey, std::move(function));
    }

    template <class T, typename... Args>
    void RegisterSimple(const string& strKey){
        std::function<T*(Args&&...)> function = [](Args&&... args){return new T(std::forward<Args>(args)...);};
        RegisterType(strKey,std::move(function));
    }

    template<class T, typename... Args>
    T* Resolve(const string& strKey, Args&&... args){
        if (m_creatorMap.find(strKey) == m_creatorMap.end())
            return nullptr;

        std::any resolver = m_creatorMap[strKey];
        std::function<T* (Args&&...)> function = std::any_cast<std::function<T* (Args&&...)>>(resolver);
        return function(std::forward<Args>(args)...);
    }

    template<class T, typename... Args>
    std::shared_ptr<T> ResolveShared(const string& strKey, Args&&... args){
        T* t = Resolve<T>(strKey, std::forward<Args>(args)...);
        return std::shared_ptr<T>(t);
    }

private:
    void RegisterType(const string& strKey, std::any&& constructor){
        if (m_creatorMap.find(strKey) != m_creatorMap.end())
            throw std::invalid_argument("this key has already exist!");

        //通过Any擦除了不同类型的构造器
        m_creatorMap.emplace(strKey, constructor);
    }

private:
    unordered_map<string, std::any> m_creatorMap;
};

