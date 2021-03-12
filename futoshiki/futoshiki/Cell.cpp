//
//  Cell.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "Cell.hpp"

namespace Csp {

Cell::Cell(int initVal, int id)
    : m_val(initVal)
    , m_id(id)
    , m_appliedConstraints()
{ }

void Cell::UpdateConstraintPointers(std::map< const Constraint*, std::shared_ptr<Constraint>* > newConstraintLookup) {
    auto oldConstraints = m_appliedConstraints;
    m_appliedConstraints.clear();
    
    for (auto& oldConstraint : oldConstraints) {
        m_appliedConstraints.push_back(*newConstraintLookup.at(oldConstraint.lock().get()));
    }
}

void Cell::AddConstraint(std::shared_ptr<Constraint> constraintToAdd) {
    m_appliedConstraints.push_back(constraintToAdd);
}

} // ::Csp

