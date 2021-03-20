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
#include <cassert>
#include <sstream>

namespace Csp {

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(const std::vector<int>& initValues, const std::set<int>& defaultPossibleValues)
    : m_numSolvedCells(0) // reported back to us when we construct the cells
    , m_numSolvedConstraints(0)
    , m_numActiveConstraints(0)
    , m_provenValid(false)
    , m_completelySolved(false)
    , m_numCells(initValues.size())
    , m_defaultPossibleValues(defaultPossibleValues)
    , m_cells()
    , m_constraints()
{
    for (auto [cellIdx, initValue] : Utils::enumerate(initValues)) {
        m_cells.emplace( std::make_pair(cellIdx, std::make_shared<Cell>(initValue, cellIdx, m_defaultPossibleValues, this)) );
    }
}

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other)
    : m_numSolvedCells(other.m_numSolvedCells)
    , m_numSolvedConstraints(other.m_numSolvedConstraints)
    , m_numActiveConstraints(other.m_numActiveConstraints)
    , m_provenValid(other.m_provenValid)
    , m_completelySolved(other.m_completelySolved)
    , m_numCells(other.m_numCells)
    , m_defaultPossibleValues(other.m_defaultPossibleValues)
    , m_cells()
    , m_constraints()
{
    // shallow copy all the cells to begin with
    for (auto& cell : other.m_cells) {
        m_cells.emplace( std::make_pair(cell.first, std::make_shared<Cell>(*cell.second.get(), this)) );
    }
    
    // Create the old to new cell lookup vectors
    std::vector<const Cell*> oldCellKeys(other.m_cells.size());
    std::transform(
        other.m_cells.cbegin(),
        other.m_cells.cend(),
            oldCellKeys.begin(),[](const std::pair< const unsigned long, std::shared_ptr<Cell> >& pair) -> const Cell* { return pair.second.get(); }
    );
    std::vector< std::shared_ptr<Cell>* > newCellValues(m_cells.size());
    std::transform(
        m_cells.begin(),
        m_cells.end(),
        newCellValues.begin(),[](std::pair< const unsigned long, std::shared_ptr<Cell> >& pair) -> std::shared_ptr<Cell>* { return &pair.second; }
    );
    // for both the below:
    // key: old
    // value: new
    std::map<const Cell*, std::shared_ptr<Cell>* > newCellLookup =
        Utils::ZipVectorsToMap(oldCellKeys, newCellValues);
    
    // Deep copy the constraints: updates the cell pointers to the new cells with the lookup map
    for (const auto& otherConstraint : other.m_constraints) {
        m_constraints.emplace_back(otherConstraint->Clone(newCellLookup, this));
    }
    
    // Create the old to new constraint lookup vectors
    std::vector<const Constraint*> oldConstraintKeys(other.m_constraints.size());
    std::transform(
        other.m_constraints.cbegin(),
        other.m_constraints.cend(),
        oldConstraintKeys.begin(),[](const std::shared_ptr<Constraint>& c) -> const Constraint* { return c.get(); }
    );
    std::vector<std::shared_ptr<Constraint>*> newConstraintValues(m_constraints.size());
    std::transform(
        m_constraints.begin(),
        m_constraints.end(),
            newConstraintValues.begin(),[](std::shared_ptr<Constraint>& c) -> std::shared_ptr<Constraint>* { return &c; }
    );
    std::map< const Constraint*, std::shared_ptr<Constraint>* > newConstraintLookup =
        Utils::ZipVectorsToMap(oldConstraintKeys, newConstraintValues);
    
    // update the constraint pointers in the new cells
    for (const auto& cell : m_cells) {
        cell.second->UpdateConstraintPointers(newConstraintLookup);
    }
}

ConstraintSatisfactionProblem& ConstraintSatisfactionProblem::operator =(const ConstraintSatisfactionProblem& other) {
    if (this == &other) {
        return *this;
    }
    
    auto tmp = ConstraintSatisfactionProblem(other);
    
    std::swap(m_defaultPossibleValues, tmp.m_defaultPossibleValues);
    std::swap(m_cells, tmp.m_cells);
    std::swap(m_constraints, tmp.m_constraints);
    std::swap(m_numSolvedCells, tmp.m_numSolvedCells);
    std::swap(m_numSolvedConstraints, tmp.m_numSolvedConstraints);
    std::swap(m_numActiveConstraints, tmp.m_numActiveConstraints);
    std::swap(m_provenValid, tmp.m_provenValid);
    std::swap(m_completelySolved, tmp.m_completelySolved);
    std::swap(m_numCells, tmp.m_numCells);
    
    return *this;
}

#ifdef DEBUG
void ConstraintSatisfactionProblem::dPrint(bool printCells) const {
    std::cout << "---- CSP ----\n";
    if (printCells) {
        std::cout << "Debug Print CSP Cells: \n";
        for (auto [cellIdx, cell] : m_cells) {
            cell->dPrint(true);
        }
    }
    
    std::cout << "Debug Print CSP Constraints: \n";
    for (auto constraint : m_constraints) {
        constraint->dPrint();
    }
    std::cout << "---- END ----\n";
}
#endif

bool ConstraintSatisfactionProblem::AddInequalityConstraint(
    unsigned long lhsCellIdx,
    InequalityConstraint::InequalityOperator op,
    unsigned long rhsCellIdx
) {
    bool lhsValid = lhsCellIdx < m_cells.size();
    bool rhsValid = rhsCellIdx < m_cells.size();
    
    if (!lhsValid || !rhsValid) {
        std::cerr << "ERR: Invalid cell indeces (out of range). Cannot add inequality constraint.\n";
        return false;
    }
    
    if (lhsCellIdx == rhsCellIdx) {
        std::cerr << "ERR: Invalid cell indeces (equal). Cannot add inequality constraint.\n";
        return false;
    }
    
    auto& lhsCell = m_cells.at(lhsCellIdx);
    auto& rhsCell = m_cells.at(rhsCellIdx);
    
    auto constraint = std::make_shared<InequalityConstraint>(m_constraints.size(), lhsCell, op, rhsCell, this);
    
    if (!constraint->Valid()) {
        std::cout << "ERR: Tried to add invalid constraint. Cannot add inequality constraint.\n";
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

bool ConstraintSatisfactionProblem::AddEqualityConstraint(
    const std::vector<unsigned long>& cellIndeces,
    EqualityConstraint::EqualityOperator op
) {
    for (auto cellIndex : cellIndeces) {
        if ( cellIndex >= m_cells.size() ) {
            std::cerr << "ERR: Invalid cell indeces (out of range). Cannot add equality constraint.\n";
            return false;
        }
    }
    
    if (!Utils::isUnique(cellIndeces)) {
        std::cerr << "ERR: Invalid cell indeces (equal). Cannot add equality constraint.\n";
        return false;
    }
    
    std::vector< std::weak_ptr<Cell> > cellPointers;
    std::transform(cellIndeces.cbegin(), cellIndeces.cend(), std::back_inserter(cellPointers),
        [this](unsigned long cellIndex) {
            return m_cells.at(cellIndex);
        }
    );
    
    auto constraint = std::make_shared<EqualityConstraint>(m_constraints.size(), cellPointers, op, this);
    
    if (!constraint->Valid()) {
        std::cout << "ERR: Tried to add invalid constraint. Cannot add inequality constraint.\n";
        return false;
    }
    
    m_constraints.push_back(
        std::static_pointer_cast<Constraint>(
            std::move(constraint)
        )
    );
    
    for (auto& pCell : cellPointers) {
        pCell.lock()->AddConstraint(m_constraints.back());
    }

    return true;
}

void ConstraintSatisfactionProblem::ReportIfCellNewlySolved() {
    ++m_numSolvedCells;
    
    assertm(m_numSolvedCells <= m_numCells, "number of solved cells should be less than the number cells");
    
    if (m_numSolvedCells == m_numCells) {
        m_completelySolved = true;
#if DEBUG_CSP
        std::cout << "All cells solved\n";
#endif // DEBUG_CSP
    }
}

void ConstraintSatisfactionProblem::ReportIfConstraintNewlySolved() {
    ++m_numSolvedConstraints;
    
    assertm(m_numSolvedConstraints <= m_constraints.size(), "number of solved constraints should be less than the number of constraints");
    
    if (m_numSolvedConstraints == m_constraints.size()) {
#if DEBUG_CSP
        std::cout << "All constraints solved\n";
#endif // DEBUG_CSP
    }
}

void ConstraintSatisfactionProblem::ReportIfConstraintBecomesActive() {
    // TODO: there must be a better way here: the +1 is necessary, as the when adding a constraint we call this before it is added to the vector
    // clearly the assertm can be made stricter after when not adding constraints for the first time
    assertm(m_numActiveConstraints < m_constraints.size() + 1, "can only have as many active constraints as there are constraints");
    ++m_numActiveConstraints;
}

void ConstraintSatisfactionProblem::ReportIfConstraintBecomesInactive() {
    assertm(m_numActiveConstraints > 0, "cannot have fewer than zero active constraints");
    --m_numActiveConstraints;

#if DEBUG_CSP
    if (m_numActiveConstraints == 0) {
        std::cout << "No more active constraints.\n";
    }
#endif // DEBUG_CSP
}

ConstraintSatisfactionProblem::SolveSolution ConstraintSatisfactionProblem::DeterministicSolve() {
#if DEBUG_CSP
    std::cout << "\t--- Starting deterministic solve --- \n";
#endif // DEBUG_CSP
    while(m_numActiveConstraints > 0) {
        for (auto& constraint : m_constraints) {
            if (constraint->IsActive()) {
                if(!constraint->Apply()) {
                    std::cerr << "ERR: constraint turned out to be invalid";
                    return {false, false}; // invalid
                }
            }
        }
    }
#if DEBUG_CSP
    std::cout << "\t--- Finished deterministic solve (" << (m_completelySolved ? "SOLVED" : "UNSOLVED") << ") ---\n\n";
#endif // DEBUG_CSP
    m_provenValid = true;
    return {m_completelySolved, m_provenValid}; // valid
}

crow::json::wvalue ConstraintSatisfactionProblem::Serialize() const {
    auto out = crow::json::wvalue();
    
    out["num_cells"] = m_cells.size();
    
    std::vector<crow::json::wvalue> cellsArray;
    std::transform(
        m_cells.cbegin(),
        m_cells.cend(),
        back_inserter(cellsArray),
        [](const std::pair< const unsigned long, std::shared_ptr<Cell> >& pair) {
            return pair.second->Serialize();
        }
    );
    out["cells"] = std::move(cellsArray);
    
    std::vector<crow::json::wvalue> constraintsArray;
    std::transform(
        m_constraints.cbegin(),
        m_constraints.cend(),
        back_inserter(constraintsArray),
        [](const std::shared_ptr<Constraint>& pConstraints) {
            return pConstraints->Serialize();
        }
    );
    out["constraints"] = std::move(constraintsArray);
    
    return out;
}

} // ::Csp


