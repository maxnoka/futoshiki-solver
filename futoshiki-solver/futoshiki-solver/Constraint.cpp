//
//  Constraint.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include "Constraint.hpp"
#include "Cell.hpp"

namespace {
    constexpr unsigned int switch_pair(unsigned int i, unsigned int j) {
       return (i << 16) + j;
    }
}

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

ConstraintOperator Constraint::stringToOperator(std::string s) {
    if (s == ">") {
        return ConstraintOperator::isGreaterThan_CO;
    }
    else if (s == "<") {
        return ConstraintOperator::isLessThan_CO;
    }
    else {
        throw std::invalid_argument("not a valid operator string");
    }
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

ConstraintDirection Constraint::determineConstraintDirection(std::tuple<unsigned int, unsigned int> sourceCellCoords,
                                                             std::tuple<unsigned int, unsigned int> targetCellCoords) {
    auto diffX = std::get<0>(sourceCellCoords) - std::get<0>(targetCellCoords);
    auto diffY = std::get<1>(sourceCellCoords) - std::get<1>(targetCellCoords);
    
    switch (switch_pair(diffX, diffY)) {
        case switch_pair(0, -1):
            return ConstraintDirection::Right;
        case switch_pair(0, 1):
            return ConstraintDirection::Left;
        case switch_pair(-1, 0):
            return ConstraintDirection::Down;
        case switch_pair(1, 0):
            return ConstraintDirection::Up;
        default:
            return ConstraintDirection::Complex;
    }
}
