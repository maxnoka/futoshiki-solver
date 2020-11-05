//
//  Constraint.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include "Constraint.hpp"

std::pair<Constraint, Constraint> Constraint::genConstraintPair(ConstraintOperator co,
                                                                Cell* sourceCell, Cell* targetCell,
                                                                ConstraintDirection direc) {
    
    Constraint forwardConstraint = Constraint(co, targetCell, direc);
    Constraint revereseConstraint = Constraint(reverseOperator(co), sourceCell, reverseDirection(direc));
    
    revereseConstraint.setCompliment(&forwardConstraint);
    forwardConstraint.setCompliment(&revereseConstraint);
    
    return std::pair<Constraint, Constraint> (forwardConstraint, revereseConstraint);
    
}

Constraint::Constraint(ConstraintOperator co, Cell* targetCell, ConstraintDirection direc) :
    co(co),
    targetCell(targetCell),
    direction(direc)
{}

ConstraintDirection Constraint::reverseDirection(ConstraintDirection toBeReversed) {
    switch (toBeReversed) {
        case ConstraintDirection::Left :
            return ConstraintDirection::Right;
        case ConstraintDirection::Right :
            return ConstraintDirection::Left;
        case ConstraintDirection::Up :
            return ConstraintDirection::Down;
        case ConstraintDirection::Down :
            return ConstraintDirection::Up;
        default :
            return ConstraintDirection::Complex;
    }
}

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

ConstraintDirection Constraint::getDirection() const {
    return direction;
}

ConstraintOperator Constraint::getOperator() const {
    return co;
}

Cell* Constraint::getTargetCell() const {
    return targetCell;
}

void Constraint::setCompliment(Constraint* complimentConstraint) {
    complimentConstraint = complimentConstraint;
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
