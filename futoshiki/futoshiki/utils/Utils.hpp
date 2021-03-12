//
//  Utils.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef Utils_hpp
#define Utils_hpp

#include <tuple>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))


namespace Utils {

// from https://www.reedbeta.com/blog/python-like-enumerate-in-cpp17/
template <typename T,
          typename TIter = decltype(std::begin(std::declval<T>())),
          typename = decltype(std::end(std::declval<T>()))>
constexpr auto enumerate(T && iterable)
{
    struct iterator
    {
        size_t i;
        TIter iter;
        bool operator != (const iterator & other) const { return iter != other.iter; }
        void operator ++ () { ++i; ++iter; }
        auto operator * () const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper
    {
        T iterable;
        auto begin() { return iterator{ 0, std::begin(iterable) }; }
        auto end() { return iterator{ 0, std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}

template<typename K, typename V>
std::map<K, V> ZipVectorsToMap(const std::vector<K>& keys, const std::vector<V>& values) {
    assertm(keys.size() == values.size(), "should only try to zip together vectors of the same length");
    
    std::map<K, V> out;
    std::transform(keys.begin(), keys.end(), values.begin(), std::inserter(out, out.end()), [](K key, V value) {
        return std::make_pair(key, value);
    });
    
    return out;
}

} // ::Utils

#endif /* Utils_hpp */
