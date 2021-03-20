//
//  Cell.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "Cell.hpp"

#include "Constraint.hpp"
#include "ConstraintSatisfactionProblem.hpp"

#include "utils/Utils.hpp"

#include <iostream>
#include <sstream>

namespace Csp {

Cell::Cell(int initVal, int id, const std::set<int>& possibleValues, ConstraintSatisfactionProblem* csp)
    : m_val(kUnsolvedSymbol) // set using SetIfPossible bellow
    , m_id(id)
    , m_possibleValues() // set below depending on if the cell is set
    , m_appliedConstraints()
    , m_csp(csp)
{
    if (initVal != kUnsolvedSymbol) {
        assertm(possibleValues.count(initVal) != 0, "input values must be possible values");
        m_possibleValues = { initVal };
        SetIfPossible();
    }
    else {
        m_possibleValues = possibleValues;
    }
}

Cell::Cell(const Cell& other, ConstraintSatisfactionProblem* newCsp)
    : m_val(other.m_val)
    , m_id(other.m_id)
    , m_possibleValues(other.m_possibleValues)
    , m_appliedConstraints(other.m_appliedConstraints)
    , m_csp(newCsp)
{ }

void Cell::UpdateConstraintPointers(const std::map< const Constraint*, std::shared_ptr<Constraint>* >& newConstraintLookup) {
    auto oldConstraints = m_appliedConstraints;
    m_appliedConstraints.clear();
    
    for (auto& oldConstraint : oldConstraints) {
        m_appliedConstraints.push_back(*newConstraintLookup.at(oldConstraint.lock().get()));
    }
}

void Cell::AddConstraint(const std::shared_ptr<Constraint>& constraintToAdd) {
    m_appliedConstraints.push_back(constraintToAdd);
}

bool Cell::EnforceLessThan(int lessThanThis) {
    if (IsSolved()) {
        if (m_val > lessThanThis) {
            std::cerr << "WARN: Cannot enforce less than " << lessThanThis << "(val " << m_val << ")\n";
            return false;
        }
        return true;
    }
    
    std::set<int>::reverse_iterator rit = m_possibleValues.rbegin();
    
    // remove everything greater than and including mustBeLessThanThis
    while(rit != m_possibleValues.rend() && *rit >= lessThanThis) {
        rit++;
    }
    
    if (rit != m_possibleValues.rbegin()) {
        m_possibleValues.erase(rit.base(), m_possibleValues.end());
        
        if (m_possibleValues.size() == 0) {
            std::cerr << "WARN: Cannot enforce less than " << lessThanThis << "\n";
            return false;
        }
        
        SetIfPossible();
        
        ReportChangeToConstraints();
    }
    
    return true;
}

bool Cell::EnforceGreaterThan(int greaterThanThis) {
    if (IsSolved()) {
        if (m_val < greaterThanThis) {
            std::cerr << "WARN: Cannot enforce greater than " << greaterThanThis << "(val " << m_val << ")\n";
            return false;
        }
        return true;
    }
    
    std::set<int>::iterator it = m_possibleValues.begin();
    
    // remove everything less than and including mustBeGreaterThanThis
    while(it != m_possibleValues.end() && *it <= greaterThanThis) {
        it++;
    }
    
    if (it != m_possibleValues.begin()) {
        m_possibleValues.erase(m_possibleValues.begin(), it);
        
        if (m_possibleValues.size() == 0) {
            std::cerr << "WARN: Cannot enforce greater than " << greaterThanThis << "\n";
            return false;
        }
        
        SetIfPossible();
        
        ReportChangeToConstraints();
    }
    
    return true;
}

std::pair<bool, bool> Cell::EliminateVals(const std::set<int>& toRemove) {
    bool removedAny = false;
    for (auto val : toRemove) {
        removedAny |= m_possibleValues.erase(val) > 0;
    }
    
    if (m_possibleValues.empty()) {
        std::cerr << "no more possible values left for this cell";
        return std::make_pair(false, removedAny);
    }
    
    if (removedAny) {
        SetIfPossible();
        ReportChangeToConstraints();
    }
    return std::make_pair(true, removedAny);
}

bool Cell::SetIfPossible() {
    assertm(m_possibleValues.size() != 0, "no possible values left for this cell");
    
    if (m_val != kUnsolvedSymbol) {
        std::cerr << "WARN: tried to set cell that was already solved\n";
        return false;
    }
    
    if (m_possibleValues.size() == 1) {
        m_val = *m_possibleValues.begin();
        m_csp->ReportIfCellNewlySolved();
        /* NOTE
        for (auto& constraint : m_appliedConstraints) {
            constraint.lock()->SetSolvedIfPossible();
        }
        */
        return true;
    }
    
    return false;
}

void Cell::ReportChangeToConstraints() {
    for (auto& constraint : m_appliedConstraints) {
        constraint.lock()->ReportChanged();
    }
}

#ifdef DEBUG
std::string Cell::dPrint(bool toCout) const {
    std::stringstream ss;
    
    ss << "cell_" << m_id << " (";
    
    if (IsSolved()) {
        ss << m_val;
    }
    else {
        for (const auto value : m_possibleValues) {
            ss << value << ", ";
        }
    }
    ss << ")";
    
    if (toCout) {
        std::cout << ss.str() << "\n";
    }
    
    return ss.str();
}
#endif

crow::json::wvalue Cell::Serialize() const {
    auto out = crow::json::wvalue();
    
    out["cell_id"] = m_id;
    out["val"] = m_val;
    
    std::vector<int> outPossibleVals(m_possibleValues.size());
    std::copy(m_possibleValues.begin(), m_possibleValues.end(), outPossibleVals.begin());
    out["possible-vals"] = outPossibleVals;
    
    return out;
}

} // ::Csp

