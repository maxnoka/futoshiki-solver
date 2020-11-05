//
//  Constraint.hpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#ifndef Constraint_hpp
#define Constraint_hpp

#include <stdio.h>
#include <utility>
#include <string>
#include <iostream>

#include "Cell.fwd.h"

enum class ConstraintOperator {isLessThan_CO, isGreaterThan_CO, isNotEqualTo_CO};
enum class ConstraintDirection {Left, Right, Up, Down, Complex};

class Constraint {
public:
    static std::pair<Constraint, Constraint> genConstraintPair(ConstraintOperator co,
                                                        Cell* sourceCell, Cell* targetCell,
                                                        ConstraintDirection direc);

    std::string printConstraint() const;
    
    
    ConstraintDirection getDirection() const;
    ConstraintOperator getOperator() const;
    Cell* getTargetCell() const;

    bool isSatisfied();
    
    friend std::ostream& operator<<(std::ostream& os, const Constraint& constraint);
    
private:
    Constraint(ConstraintOperator co, Cell* targetCell);
    Constraint(ConstraintOperator co, Cell* targetCell, ConstraintDirection direc);
    
    void setCompliment(Constraint* complimentConstraint);
    
    
    static ConstraintDirection reverseDirection(ConstraintDirection toBeReversed);
    static ConstraintOperator reverseOperator(ConstraintOperator toBeReversed);
    
    ConstraintOperator co;
    Cell* targetCell;
    Constraint* complimentConstraint;
    ConstraintDirection direction;
};

#endif /* Constraint_hpp */
