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
#include <set>
#include <algorithm>
#include <cassert>
#include <random>
#include <iterator>

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

template <class T>
bool isUnique(std::vector<T> v) {
  std::sort(v.begin(), v.end());
  return std::unique(v.begin(), v.end()) == v.end();
}

template<typename SetReferenceType>
struct SetReferenceMember {
    SetReferenceMember(SetReferenceType& setRef)
        : m_ref(setRef)
    {}
    
    std::reference_wrapper< SetReferenceType > m_ref;
    
    friend bool operator<(
        const SetReferenceMember& lhs,
        const SetReferenceMember& rhs
    ) {
        return lhs.m_ref.get() < rhs.m_ref.get();
    };
};

template<typename SetReferenceType>
struct SetPointerMember {
    SetPointerMember(SetReferenceType* setPointer)
        : m_ref(setPointer)
    {}
    
    SetReferenceType* m_ref;
    
    friend bool operator<(
        const SetPointerMember& lhs,
        const SetPointerMember& rhs
    ) {
        return *lhs.m_ref < *rhs.m_ref;
    };
};

template<typename ContainerType>
struct ReferenceContainer {
    ReferenceContainer()
        : m_container()
    { }
    
    // have had issues with multiset, where the container does not update
    // itself with when the references in the container changed.
    void Update() {
        ContainerType tmp = m_container;
        m_container = tmp;
    }
    
    ContainerType m_container;
};

template <typename T>
bool Vector2dIsRectangular(const std::vector<std::vector<T>>& vector2D) {
    unsigned long dim2(vector2D[0].size());
    for ( auto& row : vector2D) {
        if (row.size() != dim2) { return false; }
    }
    return true;
}

template <typename T>
bool Vector2dIsSquare(const std::vector<std::vector<T>>& vector2D) {
    return Vector2dIsRectangular(vector2D)
        && vector2D.size() == vector2D.front().size();
}

template <typename T>
std::vector<T> FlattenVector2d(const std::vector<std::vector<T>>& v) {
    std::size_t total_size = 0;
    for (const auto& sub : v)
        total_size += sub.size(); // I wish there was a transform_accumulate
    std::vector<T> result;
    result.reserve(total_size);
    for (const auto& sub : v)
        result.insert(result.end(), sub.begin(), sub.end());
    return result;
}

// generate set sequence like 1, 2, 3, ..., size
std::set<int> GenSetSequence(unsigned long size);

template<typename Iter, typename RandomGenerator>
Iter SelectRandomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<long> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter SelectRandomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return SelectRandomly(start, end, gen);
}

template <typename T, typename U>
bool CanTypeFitValue(const U value) {
    const intmax_t botT = intmax_t(std::numeric_limits<T>::min() );
    const intmax_t botU = intmax_t(std::numeric_limits<U>::min() );
    const uintmax_t topT = uintmax_t(std::numeric_limits<T>::max() );
    const uintmax_t topU = uintmax_t(std::numeric_limits<U>::max() );
    return !( (botT > botU && value < static_cast<U> (botT)) || (topT < topU && value > static_cast<U> (topT)) );
}

} // ::Utils

#endif /* Utils_hpp */
