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
    InequalityConstraint(
        const std::string& id,
        const std::weak_ptr<Cell>& lhsCell,
        Operator op,
        const std::weak_ptr<Cell>& rhsCell,
        ConstraintSatisfactionProblem* csp
    );
    InequalityConstraint(
        const InequalityConstraint& other,
        const std::weak_ptr<Cell>& newlhsCell,
        const std::weak_ptr<Cell>& newrhsCell,
        ConstraintSatisfactionProblem* newCsp
    );
    InequalityConstraint() = delete;
    
    // uses the lookup for deep copy:
    // 1) updates the cell pointers in the constraints
    // 2) also updates the constraint pointers in the new cells
    InequalityConstraint* Clone(
        std::map< const Cell*, std::shared_ptr<Cell>* >& newCellLookup
        , ConstraintSatisfactionProblem* newCsp
    ) final;
    
    ~InequalityConstraint() override = default;
    
    // return false if the constraint turned out not to be valid
    bool Apply() final;
    bool Valid() final;
    
    bool SetSolvedIfPossible() override;

    std::string dPrint(bool log) const final;
    
    std::vector<std::string> GetCellIds() const final;
    crow::json::wvalue Serialize() const final;

private:
    DISALLOW_COPY_AND_ASSIGN(InequalityConstraint);
    
    std::weak_ptr<Cell> m_lhsCell;
    std::weak_ptr<Cell> m_rhsCell;
}; // InequalityConstraint

} // ::Csp

#endif /* InequalityConstraint_hpp */
