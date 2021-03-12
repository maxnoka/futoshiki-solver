//
//  ConstraintSatisfactionProblem.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef ConstraintSatisfactionProblem_hpp
#define ConstraintSatisfactionProblem_hpp

#include "InequalityConstraint.hpp"

#include <vector>
#include <map>
#include <memory>

namespace Csp {

class Cell;

class ConstraintSatisfactionProblem {
public:
    ConstraintSatisfactionProblem() = default;
    ConstraintSatisfactionProblem(std::vector<int> initValues);
    
    ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other);
    ConstraintSatisfactionProblem& operator =(const ConstraintSatisfactionProblem& other);
    
    //return false if invalid constraint
    bool AddInequalityConstraint(
        int lhsCellIdx,
        InequalityConstraint::InequalityOperator op,
        int rhsCellidx
    );
    
    bool Solve(bool checkSolutionUnique);
    
#ifdef DEBUG
    virtual void dPrint() const;
#endif
private:
    std::map< int, std::shared_ptr<Cell> > m_cells;
    std::vector<std::shared_ptr<Constraint>> m_constraints;
}; // ConstraintSatisfactionProblem

} // ::Csp

#endif /* ConstraintSatisfactionProblem_hpp */
