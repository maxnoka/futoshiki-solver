//
//  InequalityConstraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include <futoshiki/InequalityConstraint.hpp>
#include <futoshiki/Cell.hpp>
#include <futoshiki/ConstraintSatisfactionProblem.hpp>
#include <futoshiki/utils/Utils.hpp>
#include <futoshiki/utils/easylogging++.h>

namespace Csp {

InequalityConstraint::InequalityConstraint(
    const std::string& id,
    const std::weak_ptr<Cell>& lhsCell,
    Operator op,
    const std::weak_ptr<Cell>& rhsCell,
    ConstraintSatisfactionProblem* csp
)
    : Constraint(id, op, csp)
    , m_lhsCell(lhsCell)
    , m_rhsCell(rhsCell)
{
    if(!Valid()) {
        m_provenInvalid = true;
    }
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
    
    ss << Constraint::dPrint(false);
    
    auto lhs = m_lhsCell.lock();
    auto rhs = m_rhsCell.lock();
    if (!lhs || !rhs) {
        LOG(ERROR) << "Not all cells that this constraint operates on are set.";
        return "";
    }
    
    ss << lhs->dPrint(false) << " " << m_operator << " " << rhs->dPrint(false);
    
    if (log) {
        VLOG(1) << ss.str();
    }
    return ss.str();
}

bool InequalityConstraint::Apply() {
    assertm(IsActive(), "Should only apply active constraints.");
    assertm(!m_provenInvalid, "Should not try to apply constraints that are already proven invalid");
    
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
        m_provenInvalid = true;
        LOG(WARNING) << "Could not apply constraint, it was not valid";
    }
    
    m_relatedCellsChanged = false;
    
    if(!m_solved) { // transition to inactive not yet reported when the cell reported back to the constraint
        ReportBecameInactive();
    }
    
    return constraintWasValid;
}

bool InequalityConstraint::Valid() {
    switch (m_operator) {
        case Operator::LessThan:
            m_provenInvalid = ! ( m_lhsCell.lock()->MinPossible() < m_rhsCell.lock()->MaxPossible() );
            break;
        case Operator::GreaterThan:
            m_provenInvalid = ! ( m_lhsCell.lock()->MaxPossible() > m_rhsCell.lock()->MinPossible() );
            break;
        default:
            assertm(false, "invalid constraint operator for inequality constraint");
            break;
    }
    return !m_provenInvalid;
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
