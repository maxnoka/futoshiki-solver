//
//  Cell.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef Cell_hpp
#define Cell_hpp

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
#include <crow/json.h>
#pragma clang diagnostic pop

#include <memory>
#include <vector>
#include <set>
#include <map>
#include <utility>

namespace Csp {

class Constraint;
class ConstraintSatisfactionProblem;

class Cell {
public:
    static constexpr auto kUnsolvedSymbol = 0;
    
    // assumes the possible values vector is sorted
    Cell(int initVal, const std::string& id, const std::set<int>& possibleValues, ConstraintSatisfactionProblem* csp);
    Cell() = delete;
    Cell(const Cell& other) = delete;
    Cell& operator =(const Cell&) = delete;
    // only shallow copy available, use UpdateConstraintPointers once the constraints are copied as well;
    Cell(const Cell& other, ConstraintSatisfactionProblem*);
    
    void UpdateConstraintPointers(const std::map< const Constraint*, std::shared_ptr<Constraint>* >& newConstraintLookup);
    
    void AddConstraint(const std::shared_ptr<Constraint>& constraintToAdd);
    
    // cannot forward delcare nested classes
    bool HasAppliedConstraint(const int op);
    
    // return false if constraint was invalid
    // true otherwise
    bool EnforceLessThan(int lessThanThis);
    bool EnforceGreaterThan(int greaterThanThis);
    // 1. true iff constraint was valid
    // 2. true iff a value was removed
    std::pair<bool, bool> EliminateVals(const std::set<int>& toRemove);
    void SetVal(const int val);
    
    int MinPossible() const { return *m_possibleValues.begin(); }
    int MaxPossible() const { return *m_possibleValues.rbegin(); }
    
    bool IsSolved() const { return m_val != kUnsolvedSymbol; }
    int Value() const { return m_val; }
    std::string Id() const {return m_id; }
    // for constraint classes (EqualityConstaint need access to this reference)
    // should be ok, as the lifetime of the constraints and the cells are the same (CSP lifetime)
    // TODO: this maybe requires some more thinking (what's the point of all the weak and shared
    // pointers if i'm just going to end up doing this
    const std::set<int>& GetPossibleValuesRef() { return m_possibleValues; }
    
    crow::json::wvalue Serialize() const;
    
    std::string dPrint(bool log) const;
    
private:
    // also reports if newly solved to the parent csp
    bool SetIfPossible();
    void ReportChangeToConstraints();
    
    int m_val;
    std::string m_id;
    std::set<int> m_possibleValues;
    std::vector<std::weak_ptr<Constraint>> m_appliedConstraints;
    ConstraintSatisfactionProblem* m_csp;
    
}; // Cell

} // ::Csp

#endif /* Cell_hpp */
