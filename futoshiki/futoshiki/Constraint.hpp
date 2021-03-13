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
        : m_id(id)
    { }
    
    virtual ~Constraint() = default;
    
    // uses the lookup for deep copy:
    // * updates the cell pointers in the constraints
    // * allocated with new
    virtual Constraint* Clone(
        std::map< const Cell*, std::shared_ptr<Cell>* > newCellLookup
    ) = 0;
    
    virtual bool Apply() = 0;
    virtual bool Valid() const = 0;
    
    int Id() const {return m_id; }
    
#ifdef DEBUG
    virtual void dPrint() = 0;
#endif //DEBUG
private:
    int m_id;
};

} // ::Csp

#endif /* Constraint_hpp */
