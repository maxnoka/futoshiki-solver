//
//  Constraint.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef Constraint_hpp
#define Constraint_hpp

#include "Cell.hpp"

#include "utils/Utils.hpp"

#include <memory>
#include <map>
#include <iostream>

namespace Csp {

class Constraint {
public:
    Constraint() = delete;
    Constraint(int id, ConstraintSatisfactionProblem* csp)
    : m_solved(false) // up to the derived class to check this
    , m_relatedCellsChanged(true)
    , m_id(id)
    , m_csp(csp)
    { }
    
    virtual ~Constraint() = default;
    
    // uses the lookup for deep copy:
    // * updates the cell pointers in the constraints
    // * allocated with new
    virtual Constraint* Clone(
        std::map< const Cell*, std::shared_ptr<Cell>* >& newCellLookup,
        ConstraintSatisfactionProblem* newCsp
    ) = 0;
    
    // returns the number of cells solved by applying the constraint
    virtual bool Apply() = 0;
    virtual bool Valid() const = 0;
    
    // cells reporting to the constraints
    void ReportChanged() {
        assertm(!m_solved, "cells changed, but constraint was already marked as solved\n");
        const bool previouslyActive = IsActive();
        m_relatedCellsChanged = true;
        if (SetSolvedIfPossible()) {
            if (previouslyActive) {
                ReportBecameInactive();
            }
        }
        else {
            if(!previouslyActive) {
                ReportBecameActive();
            }
        }
    }
    
    int Id() const {return m_id; }
    // true if all related cells are alread solved, so there's no more progress made in
    // applying it
    bool IsSolved() const { return m_solved; }
    bool IsActive() const { return !m_solved && m_relatedCellsChanged; }
    
    virtual bool SetSolvedIfPossible() = 0;
    
#ifdef DEBUG
    virtual void dPrint() const = 0;
#endif //DEBUG
    
protected:
    bool m_solved;
    
    ConstraintSatisfactionProblem* m_csp;
    
    // false if none of the cells to which the constraint pertains
    // have reported through ReportChanged() since the last time we did Apply()
    bool m_relatedCellsChanged;
    
    // if the cell becomes inactive or active, report this to the CSP
    // occcurs either when:
    //   become inactive: was active but then SetSolvedIfPossible (either through report by other cell, or apply) and then can setsolved
    //   become active: was inactive, but then relatedCellsChanged
    void ReportBecameInactive() const;
    void ReportBecameActive() const;
    
private:
    int m_id;
};

} // ::Csp

#endif /* Constraint_hpp */
