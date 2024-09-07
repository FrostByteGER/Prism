#pragma once

#include <unordered_map>
#include <atomic>

// Source taken and modified from: https://gpfault.net/posts/mapping-types-to-values.txt.html
template <class ValueType>
class TypeMap
{
    // Internally, we'll use a hash table to store mapping from type
    // IDs to the values.
    typedef std::unordered_map<int, ValueType> internalMap;
public:
    typedef typename internalMap::iterator iterator;
    typedef typename internalMap::const_iterator const_iterator;
    typedef typename internalMap::value_type value_type;

    [[nodiscard]] const_iterator begin() const { return map.begin(); }
    [[nodiscard]] const_iterator end() const { return map.end(); }
    iterator begin() { return map.begin(); }
    iterator end() { return map.end(); }

    // Finds the value associated with the type "Key" in the type map.
    template <class Key>
    iterator find() { return map.find(getTypeId<Key>()); }

    // Same as above, const version
    template <class Key>
    [[nodiscard]] const_iterator find() const { return map.find(getTypeId<Key>()); }

    // Associates a value with the type "Key"
    template <class Key>
    void put(ValueType&& value)
    {
        map[getTypeId<Key>()] = std::forward<ValueType>(value);
    }

    void clear() noexcept
    {
        map.clear();
    }

private:
    template <class Key>
    static int getTypeId()
    {
        static const int id = lastTypeId++;
        return id;
    }

    static std::atomic_int lastTypeId;
    internalMap map;
};

template <class ValueType>
std::atomic_int TypeMap<ValueType>::lastTypeId(0);
