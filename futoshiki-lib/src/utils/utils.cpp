//
//  Utils.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include <futoshiki/utils/Utils.hpp>

namespace Utils {

std::set<int> GenSetSequence(unsigned long size) {
    std::set<int> s;
    for (int i = 1; i <= size; ++i)
        s.insert(s.end(), i);
    return s;
}

} // ::Utils
