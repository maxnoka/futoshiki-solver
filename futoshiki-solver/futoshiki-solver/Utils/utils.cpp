//
//  utils.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 02/11/2020.
//

#include "utils.hpp"
#include "../Cell.hpp"

#include <random>
#include <sstream>

namespace Utils {
    
template <typename T>
bool vector2dIsRectangular(std::vector<std::vector<T>> vector2D) {
    unsigned long dim2(vector2D[0].size());
    for ( auto row : vector2D) {
        if (row.size() != dim2) { return false; }
    }
    return true;
}
template bool vector2dIsRectangular<int>(std::vector<std::vector<int>> vector2D);
template bool vector2dIsRectangular<Cell>(std::vector<std::vector<Cell>> vector2D);

std::set<int> genSetSequence(unsigned long size) {
    std::set<int> s;
    for (int i = 1; i <= size; ++i)
        s.insert(s.end(), i);
    return s;
}

template <typename T>
std::vector<T> flattenVector2d(const std::vector<std::vector<T>>& v) {
    std::size_t total_size = 0;
    for (const auto& sub : v)
        total_size += sub.size(); // I wish there was a transform_accumulate
    std::vector<T> result;
    result.reserve(total_size);
    for (const auto& sub : v)
        result.insert(result.end(), sub.begin(), sub.end());
    return result;
}
template std::vector<Cell> flattenVector2d(const std::vector<std::vector<Cell>>& v);

namespace uuid {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }
}

}
