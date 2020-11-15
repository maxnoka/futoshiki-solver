//
//  SingleTargetConstraint.hpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 13/11/2020.
//

#ifndef SingleTargetConstraint_hpp
#define SingleTargetConstraint_hpp

#include <stdio.h>

#include "Constraint.hpp"
#include "Cell.hpp"

class SingleTargetConstraint : public Constraint {
public:
    static std::pair<SingleTargetConstraint*, SingleTargetConstraint*> genConstraintPair(ConstraintOperator co,
                                                        Cell* sourceCell, Cell* targetCell,
                                                        ConstraintDirection direc);
    
    static ConstraintDirection reverseDirection(ConstraintDirection toBeReversed);
    
    ConstraintDirection getDirection() const override {return direction;}
    Cell* getTargetCell() const {return targetCell;}
    
    bool apply() override;

private:
    SingleTargetConstraint(ConstraintOperator co, Cell* targetCell);
    SingleTargetConstraint(ConstraintOperator co, Cell* sourceCell, Cell* targetCell, ConstraintDirection direc);
    
    bool applyGreaterThanConstraint();
    bool applyLessThanConstraint();
    
    virtual void debugPrint() const override;
    
    void setCompliment(Constraint* complimentConstraint);
    
    Cell* targetCell;
    ConstraintDirection direction;
    Constraint* complimentConstraint;
};

#endif /* SingleTargetConstraint_hpp */
