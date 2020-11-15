//
//  Node.hpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#ifndef Node_hpp
#define Node_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>

#include "Constraint.hpp"


class Cell {
public:
    Cell(std::set<int> possibleValues);
    Cell(int val);
    
    int getVal() const;
    void setVal(int newVal);
    
    void addConstraint(Constraint* constraintToAdd);
    std::vector<Constraint*> getCellConstraints() const;
    
    bool setIfPossible();
    bool isUnsolved() const;
    
    bool hasSamePossibleValues(std::set<int> compareValues);
    std::set<int> getPossibleVals() const {return possibleValues;}
    int getMinPossibleVal();
    int getMaxPossibleVal();
    
    bool applyNotEqualConstraint(std::set<int> noLongerPossible);
    bool applyGreaterThanConstraint(int mustBeGreaterThanThis);
    bool applyLessThanConstraint(int mustBeLessThanThis);
    
    void clearConstraints();
    
    std::string getId() const;
    void debugPrint();
    
    friend std::ostream& operator<<(std::ostream& os, const Cell& cell);
    friend std::istream& operator>>(std::istream& is, Cell& cell);
    
private:

    std::set<int> possibleValues;
    
    std::string uniqueId;
    int val;
    std::vector<Constraint*> cellConstraints;
};

#endif /* Node_hpp */
