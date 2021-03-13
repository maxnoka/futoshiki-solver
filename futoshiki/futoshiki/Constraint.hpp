//
//  Constraint.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef Constraint_hpp
#define Constraint_hpp

#include "Cell.hpp"

#include <memory>
#include <map>

namespace Csp {

class Constraint {
public:
    Constraint() = delete;
    Constraint(int id)
    : m_solved(false) // up to the derived class to check this
    , m_relatedCellsChanged(true)
    , m_id(id)

    { }
    
    virtual ~Constraint() = default;
    
    // uses the lookup for deep copy:
    // * updates the cell pointers in the constraints
    // * allocated with new
    virtual Constraint* Clone(
        std::map< const Cell*, std::shared_ptr<Cell>* > newCellLookup
    ) = 0;
    
    // returns the number of cells solved by applying the constraint
    virtual bool Apply() = 0;
    virtual bool Valid() const = 0;
    
    void ReportChanged() { m_relatedCellsChanged = true; }
    
    int Id() const {return m_id; }
    // true if all related cells are alread solved, so there's no more progress made in
    // applying it
    bool IsSolved() const { return m_solved; }
    
#ifdef DEBUG
    virtual void dPrint() const = 0;
#endif //DEBUG
    
protected:
    bool m_solved;
    // false if none of the cells to which the constraint pertains
    // have reported through ReportChanged() since the last time we did Apply()
    bool m_relatedCellsChanged;
    
private:
    int m_id;
};

} // ::Csp

#endif /* Constraint_hpp */
