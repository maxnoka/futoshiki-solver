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
    EqualityConstraint(
        const std::string& id,
        const std::vector< std::weak_ptr<Cell> >& cells,
        Operator op,
        ConstraintSatisfactionProblem* csp
    );
    EqualityConstraint(
        const EqualityConstraint& other,
        const std::vector< std::weak_ptr<Cell> >& newCells,
        ConstraintSatisfactionProblem* newCsp
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
    bool Valid() final;

    bool SetSolvedIfPossible() final;

    std::string dPrint(bool log) const final;

    std::vector<std::string> GetCellIds() const final;
    crow::json::wvalue Serialize() const final;
    
private:
    DISALLOW_COPY_AND_ASSIGN(EqualityConstraint);
    // bool: if the not equal condition turned out to be valid
    bool EvalMutuallyExclusiveNotEqualConditions();
    bool EvalOnlyOptions(); // this should only be used if the CSP is such that all cells in a not equal group have the same possible values and the number of available possile values = the number of cells in the group, e.g. for a latin square
    
    std::vector< std::weak_ptr<Cell> > m_cells;
    std::multiset< const std::set<int>*> m_availableValues;
}; // InequalityConstraint

} // ::Csp


#endif /* EqualityConstraint_hpp */
