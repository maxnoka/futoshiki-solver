//
//  ConstraintSatisfactionProblem.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "ConstraintSatisfactionProblem.hpp"

#include "Constraint.hpp"
#include "Cell.hpp"

#include "utils/Utils.hpp"

#include <iostream>
#include <algorithm>
#include <utility>
#include <map>

namespace Csp {

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(std::vector<int> initValues)
    : m_cells()
    , m_constraints()
{
    for (auto [cellIdx, initValue] : Utils::enumerate(initValues)) {
        m_cells.emplace( std::make_pair(cellIdx, std::make_shared<Cell>(initValue, cellIdx)) );
    }
}

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other)
    : m_cells()
    , m_constraints()
{
    // shallow copy all the cells to begin with
    for (auto& cell : other.m_cells) {
        m_cells.emplace( std::make_pair(cell.first, std::make_shared<Cell>(*cell.second.get())) );
    }
    
    std::vector<const Constraint*> oldConstraintKeys(other.m_constraints.size());
    std::transform(
        other.m_constraints.cbegin(),
        other.m_constraints.cend(),
        oldConstraintKeys.begin(),[](const std::shared_ptr<Constraint>& c) -> const Constraint* { return c.get(); }
    );
    
    std::vector<const Cell*> oldCellKeys(other.m_cells.size());
    std::transform(
        other.m_cells.cbegin(),
        other.m_cells.cend(),
            oldCellKeys.begin(),[](const std::pair< const int, std::shared_ptr<Cell> >& pair) -> const Cell* { return pair.second.get(); }
    );
    std::vector< std::shared_ptr<Cell>* > newCellValues(m_cells.size());
    std::transform(
        m_cells.begin(),
        m_cells.end(),
        newCellValues.begin(),[](std::pair< const int, std::shared_ptr<Cell> >& pair) -> std::shared_ptr<Cell>* { return &pair.second; }
    );
    
    // for both the below:
    // key: old
    // value: new
    std::map<const Cell*, std::shared_ptr<Cell>* > newCellLookup =
        Utils::ZipVectorsToMap(oldCellKeys, newCellValues);
            
    // Deep copy the constraints: updates the cell pointers to the new cells with the lookup map
    for (const auto& otherConstraint : other.m_constraints) {
        m_constraints.emplace_back(otherConstraint->Clone(newCellLookup));
    }
    
    std::vector<std::shared_ptr<Constraint>*> newConstraintValues(m_constraints.size());
    std::transform(
        m_constraints.begin(),
        m_constraints.end(),
            newConstraintValues.begin(),[](std::shared_ptr<Constraint>& c) -> std::shared_ptr<Constraint>* { return &c; }
    );
    
    std::map< const Constraint*, std::shared_ptr<Constraint>* > newConstraintLookup =
        Utils::ZipVectorsToMap(oldConstraintKeys, newConstraintValues);
    
    for (const auto& cell : m_cells) {
        cell.second->UpdateConstraintPointers(newConstraintLookup);
    }
}

ConstraintSatisfactionProblem& ConstraintSatisfactionProblem::operator =(const ConstraintSatisfactionProblem& other) {
    if (this == &other) {
        return *this;
    }
    
    auto tmp = ConstraintSatisfactionProblem(other);
     
    std::swap(m_cells, tmp.m_cells);
    std::swap(m_constraints, tmp.m_constraints);
    
    return *this;
}

#ifdef DEBUG
void ConstraintSatisfactionProblem::dPrint() const {
    std::cout << "Debug Print CSP Cells: \n";
    for (auto [cellIdx, cell] : m_cells) {
        std::cout << "cell_" <<  cell->Id() << ": " << cell->Value() << "\n";
    }
    
    std::cout << "Debug Print CSP Constraints: \n";
    for (auto constraint : m_constraints) {
        constraint->dPrint();
    }
}

bool ConstraintSatisfactionProblem::AddInequalityConstraint(int lhsCellIdx, InequalityConstraint::InequalityOperator op, int rhsCellIdx) {
    auto& lhsCell = m_cells.at(lhsCellIdx);
    auto& rhsCell = m_cells.at(rhsCellIdx);
    
    auto constraint = std::make_shared<InequalityConstraint>(m_constraints.size(), lhsCell, op, rhsCell);
    
    if (!constraint->Valid()) {
        std::cout << "WARN: Tried to add invalid constraint";
        constraint->dPrint();
        return false;
    }
    
    m_constraints.push_back(
        std::static_pointer_cast<Constraint>(
            std::move(constraint)
        )
    );
    
    lhsCell->AddConstraint(m_constraints.back());
    rhsCell->AddConstraint(m_constraints.back());
    
    return true;
}

#endif

} // ::Csp


