//
//  SingleTargetConstraint.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 13/11/2020.
//

#include "SingleTargetConstraint.hpp"

SingleTargetConstraint::SingleTargetConstraint(ConstraintOperator co, Cell* sourceCell, Cell* targetCell, ConstraintDirection direc) :
    Constraint(co, sourceCell),
    targetCell(targetCell),
    direction(direc)
{}

// source < target
std::pair<SingleTargetConstraint*, SingleTargetConstraint*> SingleTargetConstraint::genConstraintPair(ConstraintOperator co,
                                                                                                    Cell* sourceCell, Cell* targetCell,
                                                                                                    ConstraintDirection direc) {
    
    SingleTargetConstraint* forwardConstraint = new SingleTargetConstraint(co, sourceCell, targetCell, direc);
    SingleTargetConstraint* revereseConstraint = new SingleTargetConstraint(reverseOperator(co), targetCell, sourceCell, reverseDirection(direc));
    
    revereseConstraint->setCompliment(forwardConstraint);
    forwardConstraint->setCompliment(revereseConstraint);
    
    return std::pair<SingleTargetConstraint*, SingleTargetConstraint*> (forwardConstraint, revereseConstraint);
}

bool SingleTargetConstraint::apply() {
    if (!sourceCell->isUnsolved()) return false; // cell is solved already, don't do anything
    else {
        switch (co) {
            case ConstraintOperator::isLessThan_CO:
                return sourceCell->applyLessThanConstraint(targetCell->getMaxPossibleVal());
                break;
            case ConstraintOperator::isGreaterThan_CO:
                return sourceCell->applyGreaterThanConstraint(targetCell->getMinPossibleVal());
                break;
            default:
                return false;
                //throw std::logic_error("do not know how to handle this constraint");
        }
    }
}

void SingleTargetConstraint::debugPrint() const {
    std::cout << this << getTargetCell()->getId() << std::endl;
}

void SingleTargetConstraint::setCompliment(Constraint* complimentConstraint) {
    complimentConstraint = complimentConstraint;
}

ConstraintDirection SingleTargetConstraint::reverseDirection(ConstraintDirection toBeReversed) {
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
