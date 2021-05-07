//
//  Cell.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include <futoshiki/Cell.hpp>
#include <futoshiki/Constraint.hpp>
#include <futoshiki/ConstraintSatisfactionProblem.hpp>
#include <futoshiki/utils/Utils.hpp>
#include <futoshiki/utils/easylogging++.h>

#include <iostream>
#include <sstream>

namespace Csp {

Cell::Cell(int initVal, const std::string& id, const std::set<int>& possibleValues, ConstraintSatisfactionProblem* csp)
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

Cell::Cell(const crow::json::rvalue& cellJson)
    : m_val(kUnsolvedSymbol) // set using SetIfPossible bellow
    , m_id()
    , m_possibleValues() // set below depending on if the cell is set
    , m_appliedConstraints()
    , m_csp(nullptr)
{
    if (cellJson.t() != crow::json::type::Object) {
        throw std::runtime_error("cannot construct cell. json needs to be of type object");
    }
    
    if (   !cellJson.has("val")
        || !cellJson.has("possible_vals")
        || !cellJson.has("cell_id")
    ) {
        throw std::runtime_error("cannot construct cell. "
            "Missing required fields (val, possible_vals, cell_id");
    }

    if (cellJson["val"].t() != crow::json::type::Number) {
        throw std::runtime_error("cannot construct cell. \"val\" field needs "
            "to be of type number");
    }
    auto initVal = cellJson["val"].i();

    if (cellJson["possible_vals"].t() != crow::json::type::List) {
        throw std::runtime_error("cannot construct cell. \"possible_vals\" field needs "
            "to be of type list");
    }
    if (cellJson["cell_id"].t() != crow::json::type::String) {
        throw std::runtime_error("cannot construct cell. \"cell_id\" field needs "
            "to be of type string");
    }
    
    for (auto& possibleVal : cellJson["possible_vals"]) {
        if (possibleVal.t() != crow::json::type::Number) {
            throw std::runtime_error("cannot construct cell. \"possibl_vals\" items "
                "to be of type number");
        }
        m_possibleValues.insert(possibleVal.i());
    }

    if (initVal != kUnsolvedSymbol) {
        if (m_possibleValues.count(initVal) == 0) {
            throw std::runtime_error("input values must be possible values");
        }
        m_possibleValues = { static_cast<int>(initVal) };
        SetIfPossible();
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

bool Cell::HasAppliedConstraint(const int op) {
    auto convertedOp = static_cast<Constraint::Operator>(op);
    for (auto& constraint : m_appliedConstraints) {
        if (constraint.lock()->GetOperator() == convertedOp) {
            return true;
        }
    }
    return false;
}

bool Cell::EnforceLessThan(int lessThanThis) {
    if (IsSolved()) {
        if (m_val > lessThanThis) {
            VLOG(2) << "Cannot enforce less than " << lessThanThis << "(val " << m_val << ")";
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
            VLOG(2) << "Cannot enforce less than " << lessThanThis;
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
            VLOG(2) << "Cannot enforce greater than " << greaterThanThis << "(val " << m_val << ")";
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
            VLOG(2) << "Cannot enforce greater than " << greaterThanThis;
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
        VLOG(2) << "no more possible values left for this cell";
        return std::make_pair(false, removedAny);
    }
    
    if (removedAny) {
        SetIfPossible();
        ReportChangeToConstraints();
    }
    return std::make_pair(true, removedAny);
}

void Cell::SetVal(const int val) {
    assertm(m_possibleValues.find(val) != m_possibleValues.end(),
            "should only set to a value which is possible");
    
    m_possibleValues.clear();
    m_possibleValues.insert(val);
    
    SetIfPossible();
    ReportChangeToConstraints();
}

bool Cell::SetIfPossible() {
    assertm(m_possibleValues.size() != 0, "no possible values left for this cell");
    
    if (m_val != kUnsolvedSymbol) {
        LOG(WARNING) << "Tried to set cell that was already solved";
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

std::string Cell::dPrint(bool log) const {
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
    
    if (log) {
        VLOG(2) << ss.str();
    }
    
    return ss.str();
}

crow::json::wvalue Cell::Serialize() const {
    auto out = crow::json::wvalue();
    
    out["cell_id"] = m_id;
    out["val"] = m_val;
    
    std::vector<int> outPossibleVals(m_possibleValues.size());
    std::copy(m_possibleValues.begin(), m_possibleValues.end(), outPossibleVals.begin());
    out["possible_vals"] = outPossibleVals;
    
    return out;
}

} // ::Csp

