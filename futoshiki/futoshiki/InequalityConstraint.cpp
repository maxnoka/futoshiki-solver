//
//  InequalityConstraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "InequalityConstraint.hpp"
#include "Cell.hpp"

#include "utils/Utils.hpp"

namespace Csp {

InequalityConstraint::InequalityConstraint(
    int id,
    const std::weak_ptr<Cell>& lhsCell,
    InequalityOperator op,
    const std::weak_ptr<Cell>& rhsCell
)
    : Constraint(id)
    , m_lhsCell(lhsCell)
    , m_rhsCell(rhsCell)
    , m_operator(op)
{
    SetSolvedIfPossible();
}

bool InequalityConstraint::SetSolvedIfPossible() {
    if (m_lhsCell.lock()->IsSolved() && m_rhsCell.lock()->IsSolved()) {
        m_solved = true;
    }
    return false;
}


InequalityConstraint* InequalityConstraint::Clone(
    std::map< const Cell*,
    std::shared_ptr<Cell>* > newCellLookup
) {
    auto clonedConstraint = new InequalityConstraint(
        Id(),
        *newCellLookup.at(m_lhsCell.lock().get()),
        m_operator,
        *newCellLookup.at(m_rhsCell.lock().get()));
    return clonedConstraint;
}

#ifdef DEBUG
void InequalityConstraint::dPrint() const {
    auto lhs = m_lhsCell.lock();
    auto rhs = m_rhsCell.lock();
    if (!lhs || !rhs) {
        std::cout << "WARN: not all cells that this constraint operates on are set";
    }
    
    std::cout << lhs->dPrint(false) << " " << m_operator << " " << rhs->dPrint(false) << ")\n";
}
#endif //DEBUG

bool InequalityConstraint::Apply() {
    if (m_solved || !m_relatedCellsChanged) {
        return true;
    }
    
    bool constraintWasValid = true;
    switch (m_operator) {
        case InequalityOperator::LessThan: {
            constraintWasValid = m_lhsCell.lock()->EnforceLessThan( m_rhsCell.lock()->MaxPossible() );
            constraintWasValid &= m_rhsCell.lock()->EnforceGreaterThan( m_lhsCell.lock()->MinPossible() );
            SetSolvedIfPossible();
            break;
        }
        case InequalityOperator::GreaterThan: {
            constraintWasValid = m_lhsCell.lock()->EnforceGreaterThan( m_rhsCell.lock()->MinPossible() );
            constraintWasValid &= m_rhsCell.lock()->EnforceLessThan( m_lhsCell.lock()->MaxPossible() );
            SetSolvedIfPossible();
            break;
        }
        default:
            assertm(false, "invalid constraint operator for inequality constraint");
            return false;
    }
    
    if (!constraintWasValid) {
        std::cerr << "WARN: Could not apply constraint, it was not valid";
    }
    
    m_relatedCellsChanged = false;
    
    return constraintWasValid;
}

bool InequalityConstraint::Valid() const {
    switch (m_operator) {
        case InequalityOperator::LessThan:
            return m_lhsCell.lock()->MinPossible() < m_rhsCell.lock()->MaxPossible();
        case InequalityOperator::GreaterThan:
            return m_lhsCell.lock()->MaxPossible() > m_rhsCell.lock()->MinPossible();
        default:
            assertm(false, "invalid constraint operator for inequality constraint");
            return false;
    }
}

} // ::Csp
