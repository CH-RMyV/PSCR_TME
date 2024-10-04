#pragma once
#include <vector>
#include <forward_list>

namespace tme2
{
    
    template <typename K, typename V>
    class Hashmap
    {
        class iterator
        {

        };

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
        size_t elements;
        size_t max_fill;

        public:
            Hashmap(size_t capa=1024):
                bucket(capa),
                capacity(capa),
                elements(0)
                {max_fill = (capacity/5)*4;};

            V* get(const K &key)
            {
                size_t h = std::hash<K>()(key)%capacity;
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
                if(elements >= max_fill)
                {
                    grow();
                }
                size_t h = std::hash<K>()(key)%capacity;
                for(auto& entry:bucket[h])
                {
                    if(entry._key == key)
                    {
                        entry._value = value;
                        return true;
                    }
                }
                bucket[h].push_front(Entry(key, value));
                elements++;
                return false;
            }

            void grow()
            {
                Hashmap tmp = Hashmap(capacity*2);
                for(auto& list:bucket)
                {
                    for(auto& ent:list)
                    {
                        tmp.put(ent._key, ent._value);
                    }
                }
                bucket = tmp.bucket;
                capacity*=2;
            }

            size_t get_elements()
            {
                return elements;
            }


    };
}
// Fin namespace tme2