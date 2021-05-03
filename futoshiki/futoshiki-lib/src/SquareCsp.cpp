//
//  SquareCsp.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 27/03/2021.
//

#include <futoshiki/SquareCsp.hpp>

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

SquareCsp::SquareCsp(
    unsigned long size,
    const std::set<int>& defaultPossibleValues
)
    : TwoDimCsp(size, size, defaultPossibleValues)
    , m_size(size)
{ }

crow::json::wvalue SquareCsp::Serialize() const {
    auto out = crow::json::wvalue();
    
    out["num_cells"] = m_cells.size();
    out["grid_size"] = m_size;
    out["cells"] = SerializeGrid(); // different compared to CSP
    out["constraints"] = SerializeConstraints();
    
    return out;
}

}  // ::Csp
