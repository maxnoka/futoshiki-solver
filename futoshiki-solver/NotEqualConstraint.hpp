//
//  NotEqualityConstraint.hpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 13/11/2020.
//

#ifndef NotEqualityConstraint_hpp
#define NotEqualityConstraint_hpp

#include <stdio.h>

#include "Constraint.hpp"
#include "Cell.hpp"

class NotEqualConstraint : public Constraint {
public:
    NotEqualConstraint(Cell* sourceCell, std::vector<Cell*> targetCells);
    
    std::vector<Cell*> getTargetCells() const {return targetCells;}
    
    void addComplimentConstraint(Constraint* complimentConstraintToAdd);
    
    bool apply() override;
    bool checkValid() override;
    
private:
    virtual void debugPrint() const override;
    void setCompliments(Constraint* complimentConstraint);
    
    std::vector<Cell*> targetCells;
    std::vector<Constraint*> complimentConstraints;
};

#endif /* NotEqualityConstraint_hpp */
