//
//  EqualityConstraint.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 13/03/2021.
//

#ifndef EqualityConstraint_hpp
#define EqualityConstraint_hpp

#include "Constraint.hpp"
#include "Cell.hpp"

#include "utils/MacroUtils.h"

#include <iostream>
#include <vector>
#include <memory>

#include "utils/Utils.hpp"

namespace Csp {

class Cell;

class EqualityConstraint : public Constraint {
public:
    
    enum class EqualityOperator {
        EqualTo = 0,
        NotEqualTo,
    };
    
    friend std::ostream& operator<<(std::ostream& os, const EqualityOperator& op) {
        os << (op == EqualityOperator::EqualTo ? "=" : "!=");
        return os;
    }

    EqualityConstraint(
        int id,
        const std::vector< std::weak_ptr<Cell> >& cells,
        EqualityOperator op,
        ConstraintSatisfactionProblem* csp
    );
    EqualityConstraint() = delete;
    
    // uses the lookup for deep copy:
    // 1) updates the cell pointers in the constraints
    // 2) also updates the constraint pointers in the new cells
    EqualityConstraint* Clone(
        std::map< const Cell*, std::shared_ptr<Cell>* >& newCellLookup,
        ConstraintSatisfactionProblem* newCsp
    ) final;
    
    ~EqualityConstraint() override = default;
    
    // return false if the constraint turned out not to be valid
    bool Apply() final;
    bool Valid() const final;

    bool SetSolvedIfPossible() final;

    
#ifdef DEBUG
    void dPrint() const final;
#endif //DEBUG

private:
    DISALLOW_COPY_AND_ASSIGN(EqualityConstraint);
    // bool: if the not equal condition turned out to be valid
    bool EvalMutuallyExclusiveNotEqualConditions();
    
    std::vector< std::weak_ptr<Cell> > m_cells;
    // multiset of references to the available values set of all the cells
    // need to call .Update() before accessing the multiset
    Utils::ReferenceContainer< std::multiset< Utils::SetReferenceMember<const std::set<int>> > > m_availableValues;
    
    EqualityOperator m_operator;
    
}; // InequalityConstraint

} // ::Csp


#endif /* EqualityConstraint_hpp */
