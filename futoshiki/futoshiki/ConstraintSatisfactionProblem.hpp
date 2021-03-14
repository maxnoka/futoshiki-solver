//
//  ConstraintSatisfactionProblem.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef ConstraintSatisfactionProblem_hpp
#define ConstraintSatisfactionProblem_hpp

#include "InequalityConstraint.hpp"
#include "EqualityConstraint.hpp"

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
    bool AddEqualityConstraint(
        const std::vector<unsigned long>& cellIndeces,
        EqualityConstraint::EqualityOperator op
    );
    
    void ReportIfCellNewlySolved();
    void ReportIfConstraintNewlySolved();
    
    void ReportIfConstraintBecomesActive();
    void ReportIfConstraintBecomesInactive();
    
    struct SolveSolution {
        bool completeSolve;
        bool valid; // if invalid, then completeSolve is not defined
    };
    
    SolveSolution DeterministicSolve();
    
    bool IsCompletelySolved() { return m_completelySolved; }
    bool ProvenInValid() { return m_provenValid;}
    
#ifdef DEBUG
    virtual void dPrint(bool printCells) const;
#endif
private:
    bool m_completelySolved;
    bool m_provenValid;
    unsigned long m_numSolvedCells;
    unsigned long m_numSolvedConstraints;
    unsigned long m_numActiveConstraints;
    
    unsigned long m_numCells;
    std::set<int> m_defaultPossibleValues;
    std::map< unsigned long, std::shared_ptr<Cell> > m_cells;
    std::vector<std::shared_ptr<Constraint>> m_constraints;
}; // ConstraintSatisfactionProblem

} // ::Csp

#endif /* ConstraintSatisfactionProblem_hpp */
