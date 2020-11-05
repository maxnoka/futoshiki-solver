//
//  utils.hpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 02/11/2020.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <vector>

namespace Utils {

template <typename T>
bool vector2dIsRectangular(std::vector<std::vector<T>> vector2D);

template <typename T>
std::vector<T> flattenVector2d(const std::vector<std::vector<T>>& v);

namespace uuid {
std::string generate_uuid_v4();
}

}

#endif /* utils_hpp */
