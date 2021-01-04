//
//  NotEqualityConstraint.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 13/11/2020.
//

#include "NotEqualConstraint.hpp"
#include <set>

NotEqualConstraint::NotEqualConstraint(Cell* sourceCell, std::vector<Cell*> targetCells) :
    Constraint(ConstraintOperator::isNotEqualTo_CO, sourceCell),
    targetCells(targetCells)
{ }

bool NotEqualConstraint::checkValid() {
    std::multiset<std::set<int>> targetPossibleValues;
    for (auto targetCell : targetCells) {
        targetPossibleValues.insert(targetCell->getPossibleVals());
    }
    
    std::set<int> valuesNoLongerPossible;
    for (auto possibleValCombination : targetPossibleValues){
        if (targetPossibleValues.count(possibleValCombination) == possibleValCombination.size()) {
            valuesNoLongerPossible.insert(possibleValCombination.begin(), possibleValCombination.end());
        }
    }
    
    if (!sourceCell->isUnsolved()) {
        if (valuesNoLongerPossible.count(sourceCell->getVal())!=0) {
            return false;
        }
    }
    return true;
}

bool NotEqualConstraint::apply() {
    std::multiset<std::set<int>> targetPossibleValues;
    for (auto targetCell : targetCells) {
        targetPossibleValues.insert(targetCell->getPossibleVals());
    }
    
    std::set<int> valuesNoLongerPossible;
    for (auto possibleValCombination : targetPossibleValues){
        if (targetPossibleValues.count(possibleValCombination) == possibleValCombination.size()) {
            valuesNoLongerPossible.insert(possibleValCombination.begin(), possibleValCombination.end());
        }
    }
    
    return sourceCell->applyNotEqualConstraint(valuesNoLongerPossible);
}

void NotEqualConstraint::debugPrint() const {
    std::cout << this << std::endl;
    for (auto targetCell : targetCells) {
        std::cout << "\t" << targetCell->getId() << std::endl;
    }
    
}

void NotEqualConstraint::addComplimentConstraint(Constraint* complimentConstraintToAdd) {
    complimentConstraints.push_back(complimentConstraintToAdd);
}
