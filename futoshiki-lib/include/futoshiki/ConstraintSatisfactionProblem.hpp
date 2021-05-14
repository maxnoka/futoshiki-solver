//
//  ConstraintSatisfactionProblem.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef ConstraintSatisfactionProblem_hpp
#define ConstraintSatisfactionProblem_hpp

#include "CspSolver.hpp"
#include "InequalityConstraint.hpp"
#include "EqualityConstraint.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
#include <crow.h>
#pragma clang diagnostic pop

#include <vector>
#include <map>
#include <memory>
#include <set>

namespace Csp {

class Cell;

struct Guess {
    unsigned long cellKey;
    int val;
    
    crow::json::wvalue Serialize() const {
        auto out = crow::json::wvalue();
        out["cell"] = cellKey;
        out["val"] = val;
        return out;
    }
};

class ConstraintSatisfactionProblem {
    template <typename CSP, typename Sfinae> friend class CspSolver;
    // friend class CspSolver;
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
    
    ConstraintSatisfactionProblem(
        std::vector<Cell>&& initCells
    );
    
    ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other);
    ConstraintSatisfactionProblem& operator =(const ConstraintSatisfactionProblem& other);
    
    //return false if invalid constraint
    bool AddInequalityConstraint(
        unsigned long lhsCellIdx,
        Constraint::Operator op,
        unsigned long rhsCellidx,
        const std::string& idPrefix = "cnst"
    );
    bool AddEqualityConstraint(
        const std::vector<unsigned long>& cellIndeces,
        Constraint::Operator op,
        const std::string& idPrefix = "cnst"
    );
    
    void ReportIfCellNewlySolved();
    void ReportIfConstraintNewlySolved();
    
    void ReportIfConstraintBecomesActive();
    void ReportIfConstraintBecomesInactive();
    
    bool IsCompletelySolved() { return m_completelySolved; }
    bool ProvenInValid() { return m_provenValid;}
    
    unsigned long FindCellIdx(const std::string& cellId);
    
    virtual crow::json::wvalue Serialize() const;
    crow::json::wvalue SerializeCsp() const;
    std::vector<crow::json::wvalue> SerializeCells() const;
    std::vector<crow::json::wvalue> SerializeConstraints() const;
    
    virtual void dPrint(bool printCells) const;
    
    // void MakeGuess(const Guess& guess);
    
protected:
    std::map< unsigned long, std::shared_ptr<Cell> > m_cells;
    std::vector< std::shared_ptr<Constraint> > m_constraints;
    
    bool AddEqualityConstraint(
        const std::vector< std::weak_ptr<Cell> >& cellPointers,
        Constraint::Operator op,
        const std::string& idPrefix = "cnst"
    );
    
    std::vector<unsigned long> RemainingCellKeys() const;

    std::set<int> m_defaultPossibleValues;
    
private:
    
    // returns a set of guess which are mutually exclusive
    // and exhaustive:
    // - no two guesses can result in the same completeSolve
    // - if none of the guesses resultr in a completeSolve,
    //   then such a thing does not exist
    std::vector<Guess> GetGuesses(bool random) const;
    
    bool m_completelySolved;
    bool m_provenValid;
    unsigned long m_numSolvedCells;
    unsigned long m_numSolvedConstraints;
    unsigned long m_numActiveConstraints;
    
    unsigned long m_numCells;
}; // ConstraintSatisfactionProblem

} // ::Csp

#endif /* ConstraintSatisfactionProblem_hpp */
