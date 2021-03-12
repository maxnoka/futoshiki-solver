//
//  InequalityConstraint.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef InequalityConstraint_hpp
#define InequalityConstraint_hpp

#include "Constraint.hpp"

#include "utils/MacroUtils.h"

#include <memory>
#include <iostream>

namespace Csp {

class Cell;

class InequalityConstraint : public Constraint {
public:
    
    enum class InequalityOperator {
        LessThan = 0,
        GreaterThan,
    };
    
    friend std::ostream& operator<<(std::ostream& os, const InequalityOperator& op) {
        os << (op == InequalityOperator::LessThan ? "<" : ">");
        return os;
    }

    InequalityConstraint(
        int id,
        const std::weak_ptr<Cell>& lhsCell,
        InequalityOperator op,
        const std::weak_ptr<Cell>& rhsCell
    );
    InequalityConstraint() = delete;
    
    // uses the lookup for deep copy:
    // 1) updates the cell pointers in the constraints
    // 2) also updates the constraint pointers in the new cells
    InequalityConstraint* Clone(
        std::map< const Cell*, std::shared_ptr<Cell>* > newCellLookup
    ) final;
    
    ~InequalityConstraint() override = default;
    
    bool Apply() final { return false; }
    bool Valid() final { return true; }
    
#ifdef DEBUG
    void dPrint() final;
#endif //DEBUG
    
private:
    DISALLOW_COPY_AND_ASSIGN(InequalityConstraint);
    
    std::weak_ptr<Cell> m_lhsCell;
    std::weak_ptr<Cell> m_rhsCell;
    
    InequalityOperator m_operator;
    
}; // InequalityConstraint

} // ::Csp

#endif /* InequalityConstraint_hpp */
