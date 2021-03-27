//
//  InequalityConstraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "InequalityConstraint.hpp"
#include "Cell.hpp"
#include "ConstraintSatisfactionProblem.hpp"

#include "utils/Utils.hpp"
#include "utils/easylogging++.h"

namespace Csp {

InequalityConstraint::InequalityConstraint(
    int id,
    const std::weak_ptr<Cell>& lhsCell,
    Operator op,
    const std::weak_ptr<Cell>& rhsCell,
    ConstraintSatisfactionProblem* csp
)
    : Constraint(id, op, csp)
    , m_lhsCell(lhsCell)
    , m_rhsCell(rhsCell)
{
    if(!SetSolvedIfPossible()) {
        m_csp->ReportIfConstraintBecomesActive();
    }
}

bool InequalityConstraint::SetSolvedIfPossible() {
    if (m_lhsCell.lock()->IsSolved() && m_rhsCell.lock()->IsSolved()) {
        m_solved = true;
        m_csp->ReportIfConstraintNewlySolved();
        return true;
    }
    return false;
}


InequalityConstraint* InequalityConstraint::Clone(
    std::map< const Cell*, std::shared_ptr<Cell>* >& newCellLookup,
    ConstraintSatisfactionProblem* newCsp
) {
    auto clonedConstraint = new InequalityConstraint(
        Id(),
        *newCellLookup.at(m_lhsCell.lock().get()),
        m_operator,
        *newCellLookup.at(m_rhsCell.lock().get()),
        newCsp
    );
    return clonedConstraint;
}

std::string InequalityConstraint::dPrint(bool log) const {
    std::stringstream ss;
    
    ss << (m_solved ? "SOLVED" : "NOT SOLVED")
        << (m_relatedCellsChanged ? "* " : "  ") << "\n";
    
    auto lhs = m_lhsCell.lock();
    auto rhs = m_rhsCell.lock();
    if (!lhs || !rhs) {
        LOG(ERROR) << "Not all cells that this constraint operates on are set.";
        return "";
    }
    
    ss << lhs->dPrint(false) << " " << m_operator << " " << rhs->dPrint(false) << "\n";
    
    if (log) {
        VLOG(1) << ss.str();
    }
    return ss.str();
}

bool InequalityConstraint::Apply() {
    assertm(IsActive(), "ERR: should only apply active constraints.");
    
    bool constraintWasValid = true;
    switch (m_operator) {
        case Operator::LessThan: {
            constraintWasValid = m_lhsCell.lock()->EnforceLessThan( m_rhsCell.lock()->MaxPossible() );
            constraintWasValid &= m_rhsCell.lock()->EnforceGreaterThan( m_lhsCell.lock()->MinPossible() );
            break;
        }
        case Operator::GreaterThan: {
            constraintWasValid = m_lhsCell.lock()->EnforceGreaterThan( m_rhsCell.lock()->MinPossible() );
            constraintWasValid &= m_rhsCell.lock()->EnforceLessThan( m_lhsCell.lock()->MaxPossible() );
            break;
        }
        default:
            assertm(false, "invalid constraint operator for inequality constraint");
            return false;
    }
    
    if (!constraintWasValid) {
        LOG(WARNING) << "Could not apply constraint, it was not valid";
    }
    
    m_relatedCellsChanged = false;
    
    if(!m_solved) { // transition to inactive not yet reported when the cell reported back to the constraint
        ReportBecameInactive();
    }
    
    return constraintWasValid;
}

bool InequalityConstraint::Valid() const {
    switch (m_operator) {
        case Operator::LessThan:
            return m_lhsCell.lock()->MinPossible() < m_rhsCell.lock()->MaxPossible();
        case Operator::GreaterThan:
            return m_lhsCell.lock()->MaxPossible() > m_rhsCell.lock()->MinPossible();
        default:
            assertm(false, "invalid constraint operator for inequality constraint");
            return false;
    }
}

std::vector<std::string> InequalityConstraint::GetCellIds() const {
    return { m_lhsCell.lock()->Id(), m_rhsCell.lock()->Id()};
}

crow::json::wvalue InequalityConstraint::Serialize() const {
    auto out = crow::json::wvalue();
    
    std::vector<std::string> outCellsIds = GetCellIds();
    out["cells"] = outCellsIds;
    
    out["constraint_id"] = m_id;
    
    std::stringstream ss;
    ss << m_operator;
    out["operator"] = ss.str();
    
    out["type"] = "inequality";
    
    return out;
}

} // ::Csp
