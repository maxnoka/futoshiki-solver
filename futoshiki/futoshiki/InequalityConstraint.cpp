//
//  InequalityConstraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "InequalityConstraint.hpp"
#include "Cell.hpp"

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
{ }

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
void InequalityConstraint::dPrint() {
    auto lhs = m_lhsCell.lock();
    auto rhs = m_rhsCell.lock();
    if (!lhs || !rhs) {
        std::cout << "WARN: not all cells that this constraint operates on are set";
    }
    
    std::cout
        << "cell_" << lhs->Id() << " (" << lhs->Value() << ")"
        << " " << m_operator << " "
        << "cell_" << rhs->Id() << " (" << rhs->Value() << ")"
        << "\n";
}
#endif //DEBUG

} // ::Csp
