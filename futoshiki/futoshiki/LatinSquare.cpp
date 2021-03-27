//
//  LatinSquare.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 27/03/2021.
//

#include "LatinSquare.hpp"

#include "Utils.hpp"

namespace Csp {

LatinSquare::LatinSquare(
    const std::vector< std::vector<int> >& initValues
)
    : SquareCsp( initValues, Utils::GenSetSequence(initValues.size()) )
{
    if (!AddEqualityConstraintToAllRowsAndCols(Constraint::Operator::NotEqualTo)) {
        throw std::runtime_error("initial values do not satisfy latin square constraint");
    }
}

}  // ::Csp
