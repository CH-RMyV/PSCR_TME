#pragma once
#include <vector>
#include <forward_list>

namespace tme2
{
    typedef std::vector<std::forward_list<Entry>> buckets_t;
    template <typename K, typename V>
    class Hashmap
    {
        class Entry
        {
            public:
            const K key;
            V value;
            Entry(K k, V v):
                key(k),
                value(v)
                {};
        };

        buckets_t bucket;
        size_t capacity;

        public:
            Hashmap(size_t capa=256):
                bucket(capa),
                capacity(capa)
                {};
            Hashmap(const Hashmap&);

            V* get(const K &key)
            {
                size_t loc = std::hash<K>()(key)%capacity;
                
            }


    };
}
// Fin namespace tme2