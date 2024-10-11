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
        typedef std::forward_list<Entry> list_t;

        buckets_t bucket;
        size_t capacity;
        size_t elements;
        size_t max_fill;

        class iterator
        {
            private:
                buckets_t::iterator vit;    //pointeur vers la liste actuelle
                list_t::iterator lit;       //pointeur vers l'élément de la liste actuelle
                buckets_t::iterator back;   //dernière liste dans bucket

            public:
                iterator(buckets_t &bucket):
                    vit(bucket.begin()),
                    back(bucket.end()),
                    lit(vit->begin())
                    {};
                
                iterator(const iterator &o):
                    vit(o.vit),
                    back(o.back),
                    lit(o.lit)
                    {};

                iterator& operator++()
                {
                    ++lit;
                    if(lit==vit->end())
                    {
                        for(; vit->empty() && vit != back; ++vit);
                        lit = vit->begin();
                    }

                    return lit;
                }

                bool operator!=(const iterator &o)
                {
                    return vit!=o.vit || lit!=o.lit;
                }

                Entry& operator*()
                {
                    return &lit;
                }
        };

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