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
#include <set>

namespace Csp {

class Cell;

class ConstraintSatisfactionProblem {
public:
    ConstraintSatisfactionProblem() = default;
    ConstraintSatisfactionProblem(const std::vector<int>& initValues, const std::set<int>& defaultPossibleValues);
    
    ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other);
    ConstraintSatisfactionProblem& operator =(const ConstraintSatisfactionProblem& other);
    
    //return false if invalid constraint
    bool AddInequalityConstraint(
        unsigned long lhsCellIdx,
        InequalityConstraint::InequalityOperator op,
        unsigned long rhsCellidx
    );
    
    void ReportIfNewlySolved();
    
    bool Solve(bool checkSolutionUnique);
    
#ifdef DEBUG
    virtual void dPrint() const;
#endif
private:
    unsigned long m_numSolved;
    unsigned long m_numCells;
    std::set<int> m_defaultPossibleValues;
    std::map< unsigned long, std::shared_ptr<Cell> > m_cells;
    std::vector<std::shared_ptr<Constraint>> m_constraints;
}; // ConstraintSatisfactionProblem

} // ::Csp

#endif /* ConstraintSatisfactionProblem_hpp */
