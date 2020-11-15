//
//  Constraint.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include "Constraint.hpp"
#include "Cell.hpp"

Constraint::Constraint(ConstraintOperator co, Cell* sourceCell) :
    co(co),
    sourceCell(sourceCell)
{}

ConstraintOperator Constraint::reverseOperator(ConstraintOperator toBeReversed) {
    switch (toBeReversed) {
        case ConstraintOperator::isLessThan_CO :
            return ConstraintOperator::isGreaterThan_CO;
        case ConstraintOperator::isGreaterThan_CO :
            return ConstraintOperator::isLessThan_CO;
        case ConstraintOperator::isNotEqualTo_CO :
            return ConstraintOperator::isNotEqualTo_CO;
    }
}

ConstraintOperator Constraint::getOperator() const {
    return co;
}

ConstraintDirection Constraint::getDirection() const {
    return ConstraintDirection::Complex;
}

std::ostream& operator<<(std::ostream& os, const Constraint& constraint) {
    switch (constraint.getOperator()) {
        case ConstraintOperator::isGreaterThan_CO:
            os << ">";
            break;
        case ConstraintOperator::isLessThan_CO:
            os << "<";
            break;
        case ConstraintOperator::isNotEqualTo_CO:
            os << " "; // don't print this one
            break;
    }
    return os;
}
