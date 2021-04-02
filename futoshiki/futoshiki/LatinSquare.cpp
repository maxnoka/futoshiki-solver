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

crow::json::wvalue LatinSquare::Serialize() const {
    auto out = crow::json::wvalue();
    
    out["num_cells"] = m_cells.size();
    out["grid_size"] = m_size;
    out["cells"] = SerializeGrid(); // different compared to CSP
    auto constraints = SerializeConstraints();

    // the row and column constraints are assumed, so we don't report them
    std::vector<crow::json::wvalue> outConstraints;
    std::for_each(
        m_constraints.cbegin(),
        m_constraints.cend(),
        [&outConstraints](const std::shared_ptr<Constraint>& pConstraints) {
            auto serial = pConstraints->Serialize();
            std::string id = serial["constraint_id"].dump();
            if (id.substr(1, 3) != "row" && id.substr(1, 3) != "col"){
                outConstraints.emplace_back( std::move(serial) );
            }
        }
    );
    
    out["constraints"] = std::move(outConstraints);
    
    return out;
}

}  // ::Csp
