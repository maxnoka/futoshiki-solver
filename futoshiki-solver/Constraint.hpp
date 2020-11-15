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
    std::string printConstraint() const;
    ConstraintOperator getOperator() const;
    virtual bool apply() = 0;
    virtual void debugPrint() const = 0;
    
    virtual ConstraintDirection getDirection() const;
    
    bool isSatisfied();
    
    static ConstraintOperator reverseOperator(ConstraintOperator toBeReversed);
    
    friend std::ostream& operator<<(std::ostream& os, const Constraint& constraint);

protected:
    Constraint(ConstraintOperator co, Cell* sourceCell);
    Cell* sourceCell;
    ConstraintOperator co;
};

#endif /* Constraint_hpp */
