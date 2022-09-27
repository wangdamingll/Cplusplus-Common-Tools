//
// Created by Administrator on 2022/9/27.
//

#ifndef ORDEREDMAP_H
#define ORDEREDMAP_H

#include <map>
#include <list>
#include <unordered_map>
#include <functional>
#include <algorithm>

template <typename CONTAINER, typename V, typename KEY_FUNC, typename COMPARE_FUNC>
class OrderedMapHandle
{
public:
    using key_type = typename KEY_FUNC::result_type;

private:
    static_assert(std::is_pointer<V>::value, "V is not pointer");
    static_assert(std::is_same<typename KEY_FUNC::argument_type, V>::value, "V is not same KEY_FUNC argument_type");
    static_assert(std::is_same<typename COMPARE_FUNC::first_argument_type, V>::value, "V is not same COMPARE_FUNC first argument_type");
    static_assert(std::is_same<typename COMPARE_FUNC::second_argument_type, V>::value, "V is not same COMPARE_FUNC second argument_type");

public:
    OrderedMapHandle() = default;
    ~OrderedMapHandle() = default;

public:
    bool Insert(V value)
    {
        auto ret = map_.emplace(KEY_FUNC()(value), std::make_pair(value, list_.end()));
        if(!ret.second)
        {
            return false;
        }
        ret.first->second.second = list_.emplace(std::find_if(list_.begin(), list_.end(), std::bind(COMPARE_FUNC(), value, std::placeholders::_1)), value);
        return true;
    }

    void Erase(const key_type key)
    {
        auto it = map_.find(key);
        if(it != map_.end())
        {
            list_.erase(it->second.second);
            map_.erase(it);
        }
    }

    template<typename FUNC>
    void Foreach(FUNC&& func)
    {
        for(auto it = list_.begin(); it != list_.end(); ++it)
        {
            func(*it);
        }
    }

    template<typename FUNC>
    void ForeachRemove(FUNC&& func)
    {
        for(auto it = list_.begin(); it != list_.end();)
        {
            auto iter_cur = it++;
            auto* value = *iter_cur;
            if(!func(value))
            {
                break;
            }
            this->Erase(KEY_FUNC()(value));
        }
    }

private:
    std::list<V> list_;
    CONTAINER map_;
};

template <typename V, typename KEY_FUNC, typename COMPARE_FUNC>
using OrderedMap = OrderedMapHandle<std::map<typename KEY_FUNC::result_type, std::pair<V, typename std::list<V>::iterator> >, V, KEY_FUNC, COMPARE_FUNC>;

template <typename V, typename KEY_FUNC, typename COMPARE_FUNC>
using OrderedHashMap = OrderedMapHandle<std::unordered_map<typename KEY_FUNC::result_type, std::pair<V, typename std::list<V>::iterator> >, V, KEY_FUNC, COMPARE_FUNC>;

#endif //ORDEREDMAP_H
