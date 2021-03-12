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
#include <map>

namespace Csp {

class Constraint;

class Cell {
public:
    Cell(int initVal, int id);
    Cell() = delete;
    Cell(const Cell& other) = default; // only shallow copy available, use UpdateConstraintPointers once the constraints are copied as well;
    Cell& operator =(const Cell&) = default;
    
    void UpdateConstraintPointers(std::map< const Constraint*, std::shared_ptr<Constraint>* > newConstraintLookup);
    
    void AddConstraint(std::shared_ptr<Constraint> constraintToAdd);
    
    int Value() const { return m_val; }
    int Id() const {return m_id; }
    
private:
    int m_val;
    int m_id;
    std::vector<std::weak_ptr<Constraint>> m_appliedConstraints;
    
}; // Cell

} // ::Csp

#endif /* Cell_hpp */
