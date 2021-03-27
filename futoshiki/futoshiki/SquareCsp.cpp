//
//  SquareCsp.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 27/03/2021.
//

#include "SquareCsp.hpp"

namespace Csp {

SquareCsp::SquareCsp(
    const std::vector< std::vector<int> >& initValues,
    const std::set<int>& defaultPossibleValues
)
    : TwoDimCsp(initValues, defaultPossibleValues)
    , m_size(initValues.size())
{
    if (!Utils::Vector2dIsSquare(initValues)) {
        throw std::invalid_argument("require square cell vector for square CSP");
    }
}

}  // ::Csp
