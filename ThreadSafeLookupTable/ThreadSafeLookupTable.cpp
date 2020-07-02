// ThreadSafeLookupTable.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include<map>
#include <memory>
#include <mutex>
#include <functional>
#include <list>
#include <utility>
#include <shared_mutex>
//#include <boost/thread/shared_mutex.hpp>

template<typename Key, typename Value, typename Hash = std::hash<Key> >
class ThreadsafeLookupTable
{
private:
    class bucket_type
    {
    private:
        typedef std::pair<Key, Value> bucket_value;
        typedef std::list<bucket_value> bucket_data;
        typedef typename bucket_data::iterator bucket_iterator;
        typedef typename bucket_data::const_iterator bucket_const_iterator;

        bucket_data data;
        //mutable boost::shared_mutex mutex;
        mutable std::shared_mutex mutex;
        
        bucket_const_iterator find_entry_for(Key const& key) const
        {
            return std::find_if(data.begin(), data.end(),
                [&](const bucket_value& item)
                {return item.first == key; });
        }
    public:
        Value value_for(Key const& key, Value const& default_value) const
        {
           // boost::shared_lock<boost::shared_mutex> lock(mutex);
            std::shared_lock<std::shared_mutex> lock(mutex);
            bucket_const_iterator found_entry = find_entry_for(key);
            return (found_entry == data.end()) ? default_value : found_entry->second;
        }

        void add_or_update_mapping(Key const& key, Value const& value)
        {
            //std::unique_lock<boost::shared_mutex> lock(mutex);
            std::unique_lock<std::shared_mutex> lock(mutex);
            bucket_const_iterator found_entry = find_entry_for(key);
            if (found_entry == data.end())
            {
                data.push_back(bucket_value(key, value));
            }
            else
            {
                auto data = const_cast<bucket_value&>(*found_entry);
                data.second = value;
            }
        }

        void remove_mapping(Key const& key)
        {
            //std::unique_lock<boost::shared_mutex> lock(mutex);
            std::unique_lock<std::shared_mutex> lock(mutex);
            bucket_const_iterator found_entry = find_entry_for(key);
            if (found_entry != data.end())
            {
                data.erase(found_entry);
            }
        }
    };

    std::vector<std::unique_ptr<bucket_type> > buckets;
    Hash hasher;

    bucket_type& get_bucket(Key const& key) const
    {
        std::size_t const bucket_index = hasher(key) % buckets.size();
        return *buckets[bucket_index];
    }

public:
    typedef Key key_type;
    typedef Value mapped_type;
    typedef Hash hash_type;

    ThreadsafeLookupTable(
        unsigned num_buckets = 19, Hash const& hasher_ = Hash()) :
        buckets(num_buckets), hasher(hasher_)
    {
        for (unsigned i = 0; i < num_buckets; ++i)
        {
            buckets[i].reset(new bucket_type);
        }
    }

    ThreadsafeLookupTable(ThreadsafeLookupTable const& other) = delete;
    ThreadsafeLookupTable& operator=(
        ThreadsafeLookupTable const& other) = delete;

    Value value_for(Key const& key,Value const& default_value = Value()) const
    {
        return get_bucket(key).value_for(key, default_value);
    }

    void add_or_update_mapping(Key const& key, Value const& value)
    {
        get_bucket(key).add_or_update_mapping(key, value);
    }

    void remove_mapping(Key const& key)
    {
        get_bucket(key).remove_mapping(key);
    }

    std::map<Key, Value> get_map() const
    {
        //std::vector<std::unique_lock<boost::shared_mutex> > locks;
        std::vector<std::unique_lock<std::shared_mutex> > locks;
        for (unsigned i = 0; i < buckets.size(); ++i)
        {
            locks.push_back(
                //std::unique_lock<boost::shared_mutex>(buckets[i].mutex));
                std::unique_lock<std::shared_mutex>(buckets[i].mutex));
        }
        std::map<Key, Value> res;
        for (unsigned i = 0; i < buckets.size(); ++i)
        {
            for (auto& it = buckets[i].data.begin();
                it != buckets[i].data.end();
                ++it)
            {
                res.insert(*it);
            }
        }
        return res;
    }
};


int main()
{
    ThreadsafeLookupTable<int, int> hashTable;
    hashTable.add_or_update_mapping(1, 2);
    auto value = hashTable.value_for(1);
    std::cout << value << std::endl;
    hashTable.remove_mapping(1);
    value = hashTable.value_for(1);
    if (value == 0) {
        std::cout << "remove" << std::endl;
    }
}

