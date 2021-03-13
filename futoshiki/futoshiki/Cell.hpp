//
//  Cell.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef Cell_hpp
#define Cell_hpp

#include <memory>
#include <vector>
#include <set>
#include <map>

namespace Csp {

class Constraint;

class Cell {
public:
    static constexpr auto kUnsolvedSymbol = 0;
    
    // assumes the possible values vector is sorted
    Cell(int initVal, int id, const std::set<int>& possibleValues);
    Cell() = delete;
    Cell(const Cell& other) = default; // only shallow copy available, use UpdateConstraintPointers once the constraints are copied as well;
    Cell& operator =(const Cell&) = default;
    
    void UpdateConstraintPointers(const std::map< const Constraint*, std::shared_ptr<Constraint>* >& newConstraintLookup);
    
    void AddConstraint(const std::shared_ptr<Constraint>& constraintToAdd);
    
    // return false if constraint was invalid
    // true otherwise
    bool EnforceLessThan(int lessThanThis);
    bool EnforceGreaterThan(int greaterThanThis);
    
    int MinPossible() const { return *m_possibleValues.begin(); }
    int MaxPossible() const { return *m_possibleValues.rbegin(); }
    
    bool IsSolved() const { return m_val != kUnsolvedSymbol; }
    int Value() const { return m_val; }
    int Id() const {return m_id; }
    
#ifdef DEBUG
    std::string dPrint(bool toCout) const;
#endif //DEBUG
    
private:
    bool SetIfPossible();
    void ReportChangeToConstraints();
    
    int m_val;
    int m_id;
    std::set<int> m_possibleValues;
    std::vector<std::weak_ptr<Constraint>> m_appliedConstraints;
    
}; // Cell

} // ::Csp

#endif /* Cell_hpp */
