//
//  Node.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include "Cell.hpp"
#include "utils.hpp"

Cell::Cell() : val(0), uniqueId(Utils::uuid::generate_uuid_v4())
{}

Cell::Cell(int val) : val(val), uniqueId(Utils::uuid::generate_uuid_v4())
{}

int Cell::getVal() const {
    return val;
}

std::vector<Constraint> Cell::getCellConstraints() const {
    return cellConstraints;
}

void Cell::addConstraint(Constraint constraintToAdd) {
    cellConstraints.push_back(constraintToAdd);
    return;
}

void Cell::set(int newVal) {
    val = newVal;
}

std::istream& Cell::set(std::istream& is) {
    is >> val;
    return is;
}

std::string Cell::getId() const {
    return uniqueId;
}

void Cell::debugPrint() {
    std::cout << "id: " << uniqueId << std::endl;
    for (auto constraint : cellConstraints) {
        std::cout << constraint << constraint.getTargetCell()->getId() << std::endl;
    }
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
    int cellVal = cell.getVal();
    
    if (cellVal == blankSymbol) os << " ";
    else os << cell.getVal();
    
    return os;
}

std::istream& operator>>(std::istream& is, Cell& cell) {
    return cell.set(is);
}

