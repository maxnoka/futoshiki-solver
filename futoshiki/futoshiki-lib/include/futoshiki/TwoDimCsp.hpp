//
//  TwoDimCsp.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 26/03/2021.
//

#ifndef TwoDimCsp_hpp
#define TwoDimCsp_hpp

#include "ConstraintSatisfactionProblem.hpp"

namespace Csp {

class TwoDimCsp : public ConstraintSatisfactionProblem {
public:
    // Using (x, y), where x is the column, y is the row
    // zero indexed
    using CellCoords = std::pair<unsigned long, unsigned long>;
    
    TwoDimCsp(
        const std::vector< std::vector<int> >& initValues,
        const std::set<int>& defaultPossibleValues
    );
    
    TwoDimCsp(
        unsigned long columns,
        unsigned long rows,
        const std::set<int>& defaultPossibleValues
    );
    
    //return false if invalid constraint
    bool AddInequalityConstraint(
        const CellCoords& lhsCellCoords,
        Constraint::Operator op,
        const CellCoords& rhsCellCoords
    );
    bool AddEqualityConstraint(
        const std::vector<CellCoords>& cellCoords,
        Constraint::Operator op
    );
    
    bool AddEqualityConstraintToRow(
        unsigned long rowIdx,
        Constraint::Operator op
    );
    bool AddEqualityConstraintToCol(
        unsigned long colIdx,
        Constraint::Operator op
    );
    bool AddEqualityConstraintToAllRows(
        Constraint::Operator op
    );
    bool AddEqualityConstraintToAllCols(
        Constraint::Operator op
    );
    bool AddEqualityConstraintToAllRowsAndCols(
        Constraint::Operator op
    );
    
    virtual void dPrint(bool printCells) const override;
    void dPrintGrid() const;
    
    virtual crow::json::wvalue Serialize() const override;
    crow::json::wvalue SerializeGrid() const;

    unsigned long m_numRows;
    unsigned long m_numCols;

protected:
    bool ValidCoords(const CellCoords& cords);
    unsigned long CoordsToIndex(const CellCoords& cords);
    
private:
    std::vector< std::vector< std::weak_ptr<Cell> > > GetGrid() const;
    std::vector< std::weak_ptr<Cell> > GetRow(unsigned long rowIdx) const;
    std::vector< std::weak_ptr<Cell> > GetCol(unsigned long colIdx) const;
    
}; // TwoDimCsp

} // ::Csp

#endif /* TwoDimCsp_hpp */
