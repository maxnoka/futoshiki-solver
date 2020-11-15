//
//  Node.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include <algorithm>
#include <iterator>

#include "Cell.hpp"
#include "Constraint.hpp"
#include "utils.hpp"

namespace {
int kUnsolvedSymbol = 0; // to symbolize that a cell does not have a set value
}

Cell::Cell(std::set<int> possibleValues) :
    val(kUnsolvedSymbol),
    possibleValues(possibleValues),
    uniqueId(Utils::uuid::generate_uuid_v4())
{}

Cell::Cell(int val) :
    val(val),
    uniqueId(Utils::uuid::generate_uuid_v4()),
    possibleValues({val})
{}

int Cell::getVal() const {
    return val;
}

void Cell::clearConstraints() {
    cellConstraints.clear();
}

std::vector<Constraint*> Cell::getCellConstraints() const {
    return cellConstraints;
}

void Cell::addConstraint(Constraint* constraintToAdd) {
    cellConstraints.push_back(constraintToAdd);
    return;
}

bool Cell::setIfPossible() {
    if (possibleValues.size()==1) {
        setVal(*possibleValues.begin());
        return true;
    }
    return false;
}

void Cell::setVal(int newVal) {
    val = newVal;
    if (newVal != kUnsolvedSymbol) possibleValues = {newVal};
}

bool Cell::applyNotEqualConstraint(std::set<int> noLongerPossible) {
    std::set<int> setDifference;
    
    std::set_difference(std::begin(possibleValues), std::end(possibleValues), // the first vector...
                            std::begin(noLongerPossible), std::end(noLongerPossible), // ...minus the second...
                            std::inserter(setDifference, setDifference.begin()));     // ...is stored into here
    possibleValues = setDifference;
    if (possibleValues.size() == 0) { throw std::runtime_error("no more options left! impossible");}
    return setIfPossible();
}


bool Cell::applyLessThanConstraint(int mustBeLessThanThis) {
    std::set<int>::reverse_iterator rit = possibleValues.rbegin();
    
    // remove everything greater than and including mustBeLessThanThis
    while(rit != possibleValues.rend() && *rit >= mustBeLessThanThis) { rit++;}
    
    possibleValues.erase(rit.base(), possibleValues.end());
    if (possibleValues.size() == 0) { throw std::runtime_error("no more options left! impossible");}
    
    return setIfPossible();
}

bool Cell::applyGreaterThanConstraint(int mustBeGreaterThanThis) {
    std::set<int>::iterator it = possibleValues.begin();
    
    // remove everything less than and including mustBeGreaterThanThis
    while(it != possibleValues.end() && *it <= mustBeGreaterThanThis) { it++;}
    
    possibleValues.erase(possibleValues.begin(), it);
    if (possibleValues.size() == 0) { throw std::runtime_error("no more options left! impossible");}
    
    return setIfPossible();
}

int Cell::getMinPossibleVal() {
    return *possibleValues.begin();
}

int Cell::getMaxPossibleVal() {
    return *possibleValues.rbegin();
}

bool Cell::isUnsolved() const {
    if (val == kUnsolvedSymbol) return true;
    else return false;
}

std::string Cell::getId() const {
    return uniqueId;
}

void Cell::debugPrint() {
    std::cout << "id: " << uniqueId << std::endl;
    for (std::set<int>::iterator it=possibleValues.begin(); it!=possibleValues.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << std::endl;
    for (auto constraint : cellConstraints) {
        constraint->debugPrint();
    }
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
    if (cell.isUnsolved()) os << " ";
    else os << cell.getVal();
    
    return os;
}
