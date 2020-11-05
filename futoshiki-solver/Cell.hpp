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

#include "Constraint.hpp"

const int blankSymbol = 0;  // to symbolize that a cell does not have a set value

class Cell {
public:
    Cell();
    Cell(int val);
    
    int getVal() const;
    std::vector<Constraint> getCellConstraints() const;
    
    void set(int newVal);
    
    void addConstraint(Constraint constraintToAdd);
    
    std::istream& set(std::istream& is);
    
    std::string getId() const;
    void debugPrint();
    
    friend std::ostream& operator<<(std::ostream& os, const Cell& cell);
    friend std::istream& operator>>(std::istream& is, Cell& cell);
    
private:
    std::string uniqueId;
    int val;
    std::vector<Constraint> cellConstraints;
};

#endif /* Node_hpp */
