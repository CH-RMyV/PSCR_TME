#pragma once
#include <vector>
#include <forward_list>

namespace tme2
{
    
    template <typename K, typename V>
    class Hashmap
    {

        class Entry
        {
            public:
            const K _key;
            V _value;
            Entry(K k, V v):
                _key(k),
                _value(v)
                {};
        };

        typedef std::vector<std::forward_list<Entry>> buckets_t; 

        buckets_t bucket;
        size_t capacity;

        public:
            Hashmap(size_t capa=256):
                bucket(capa),
                capacity(capa)
                {};
            //Hashmap(const Hashmap&);

            V* get(const K &key)
            {
                size_t h = std::hash<K>()(key)%capacity;
                std::forward_list<Entry> loc = bucket[h];
                for(auto& entry:bucket[h])
                {
                    if(entry._key == key)
                    {
                        return &entry._value;
                    }
                }
                return nullptr;
            }

            bool put(const K &key, const V &value)
            {
                size_t h = std::hash<K>()(key)%capacity;
                std::forward_list<Entry> loc = bucket[h];
                for(auto& entry:bucket[h])
                {
                    if(entry._key == key)
                    {
                        entry._value = value;
                        return true;
                    }
                }
                /*for(auto it=loc.begin(); it!=loc.end(); ++it)
                {
                    if(it->_key == key)
                    {
                        std::cout<<"key found"<<std::endl;
                        it->_value = value;
                        return true;
                    }
                }*/
                bucket[h].push_front(Entry(key, value));
                return false;
            }


    };
}
// Fin namespace tme2