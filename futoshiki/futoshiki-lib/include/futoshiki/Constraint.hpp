//
//  Constraint.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#ifndef Constraint_hpp
#define Constraint_hpp

#include "Cell.hpp"

#include <futoshiki/utils/Utils.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
#include <crow/json.h>
#pragma clang diagnostic pop

#include <memory>
#include <map>
#include <iostream>

namespace Csp {

class Constraint {
public:
    enum class Operator {
        EqualTo = 0,
        NotEqualTo,
        LessThan,
        GreaterThan,
    };
    
    friend std::ostream& operator<<(std::ostream& os, const Operator& op) {
        switch (op) {
            case Operator::EqualTo:
                os << "=";
                break;
            case Operator::NotEqualTo:
                os << "!=";
                break;
            case Operator::LessThan:
                os << "<";
                break;
            case Operator::GreaterThan:
                os << ">";
                break;
        }
        return os;
    }
    
    static Operator ReverseOperator(const Operator& op) {
        switch (op) {
            case Operator::EqualTo:
                return Operator::NotEqualTo;
            case Operator::NotEqualTo:
                return Operator::EqualTo;
            case Operator::LessThan:
                return Operator::GreaterThan;
            case Operator::GreaterThan:
                return Operator::LessThan;
        }
    }
    

    
    Constraint() = delete;
    Constraint(const std::string& id, Operator op, ConstraintSatisfactionProblem* csp)
    : m_provenInvalid(false) // up to the derived class to check this
    , m_solved(false) // up to the derived class to check this
    , m_operator(op)
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
    virtual bool Valid() = 0;
    
    // cells reporting to the constraints
    void ReportChanged();
    
    std::string Id() const {return m_id; }
    // true if all related cells are alread solved, so there's no more progress made in
    // applying it
    bool IsSolved() const { return m_solved; }
    bool IsActive() const { return !m_solved && m_relatedCellsChanged; }
    // this state occurs if another constraint solved the last
    // usnolved relatedCell. We need to make sure that this is valid
    bool ShouldStillCheckValid() const {
        return m_solved && m_relatedCellsChanged;
    }
    void SetChecked() {
        assertm(ShouldStillCheckValid() && !m_provenInvalid, "Should only call this for cells in the SOLVED* state");
        m_relatedCellsChanged = false;
    }
    
    Operator GetOperator() const { return m_operator; }
    
    virtual bool SetSolvedIfPossible() = 0;
    
#ifdef DEBUG
    virtual std::string dPrint(bool log) const;
#endif //DEBUG
    
    virtual std::vector<std::string> GetCellIds() const = 0;
    virtual crow::json::wvalue Serialize() const = 0;
    
protected:
    bool m_provenInvalid;
    bool m_solved;
    Operator m_operator;
    
    ConstraintSatisfactionProblem* m_csp;
    
    std::string m_id;
    
    // false if none of the cells to which the constraint pertains
    // have reported through ReportChanged() since the last time we did Apply()
    bool m_relatedCellsChanged;
    
    // if the cell becomes inactive or active, report this to the CSP
    // occcurs either when:
    //   become inactive: was active but then SetSolvedIfPossible (either through report by other cell, or apply) and then can setsolved
    //   become active: was inactive, but then relatedCellsChanged
    void ReportBecameInactive() const;
    void ReportBecameActive() const;

};

} // ::Csp

#endif /* Constraint_hpp */
