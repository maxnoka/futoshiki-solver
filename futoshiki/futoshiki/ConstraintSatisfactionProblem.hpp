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

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
#include <crow/json.h>
#pragma clang diagnostic pop

#include <vector>
#include <map>
#include <memory>
#include <set>

#ifndef DEBUG_CSP
#define DEBUG_CSP 0
#endif

namespace Csp {

class Cell;

class ConstraintSatisfactionProblem {
public:
    ConstraintSatisfactionProblem() = default;
    ConstraintSatisfactionProblem(
        const std::vector<int>& initValues,
        const std::set<int>& defaultPossibleValues
    );
    
    // if you want to define the cell ids
    ConstraintSatisfactionProblem(
        const std::vector< std::pair<std::string, int> >& initValues,
        const std::set<int>& defaultPossibleValues
    );
    
    ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other);
    ConstraintSatisfactionProblem& operator =(const ConstraintSatisfactionProblem& other);
    
    //return false if invalid constraint
    bool AddInequalityConstraint(
        unsigned long lhsCellIdx,
        Constraint::Operator op,
        unsigned long rhsCellidx
    );
    bool AddEqualityConstraint(
        const std::vector<unsigned long>& cellIndeces,
        Constraint::Operator op
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
    // also does guessing
    SolveSolution Solve();
    SolveSolution SolveUnique();
    
    struct Guess {
        unsigned long cellKey;
        int val;
    };
    
    bool IsCompletelySolved() { return m_completelySolved; }
    bool ProvenInValid() { return m_provenValid;}
    
    crow::json::wvalue Serialize() const;
    
#ifdef DEBUG
    virtual void dPrint(bool printCells) const;
#endif
    
protected:
    std::map< unsigned long, std::shared_ptr<Cell> > m_cells;
    std::vector< std::shared_ptr<Constraint> > m_constraints;
    
    bool AddEqualityConstraint(
        const std::vector< std::weak_ptr<Cell> >& cellPointers,
        Constraint::Operator op
    );
    
private:
    // returns a set of guess which are mutually exclusive
    // and exhaustive:
    // - no two guesses can result in the same completeSolve
    // - if none of the guesses resultr in a completeSolve,
    //   then such a thing does not exist
    void MakeGuess(const Guess& guess);
    std::vector<Guess> GetGuesses() const;
    
    std::vector<unsigned long> RemainingCellKeys() const;
    
    bool m_completelySolved;
    bool m_provenValid;
    unsigned long m_numSolvedCells;
    unsigned long m_numSolvedConstraints;
    unsigned long m_numActiveConstraints;
    
    unsigned long m_numCells;
    std::set<int> m_defaultPossibleValues;
}; // ConstraintSatisfactionProblem

} // ::Csp

#endif /* ConstraintSatisfactionProblem_hpp */
