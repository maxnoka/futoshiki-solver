//
//  ConstraintSatisfactionProblem.hpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 04/11/2020.
//

#ifndef ConstraintSatisfactionProblem_hpp
#define ConstraintSatisfactionProblem_hpp

#include "Cell.hpp"

#include <stdio.h>

class ConstraintSatisfactionProblem {
public:
    ConstraintSatisfactionProblem() = default;
    ConstraintSatisfactionProblem(std::vector<Cell> initial_cells);
    
    void addConstraint(ConstraintOperator co,
                                 unsigned long sourceCellIndex, unsigned long targetCellIndex,
                                 ConstraintDirection direc);
    
    std::vector<Cell> solve();
    
    int getCellValue(unsigned long cellIdx) const;
    Cell getCell(unsigned long cellIdx) const;
    
    std::vector<Cell> getCells() const;
    
private:
    std::vector<Cell> m_cells;
    
};

#endif /* ConstraintSatisfactionProblem_hpp */
