//
//  ConstraintSatisfactionProblem.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 04/11/2020.
//

#include "ConstraintSatisfactionProblem.hpp"

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(std::vector<Cell> initial_cells) :
    m_cells(initial_cells)
{ }

void ConstraintSatisfactionProblem::addConstraint(ConstraintOperator co,
                                                            unsigned long sourceCellIndex,
                                                            unsigned long targetCellIndex, ConstraintDirection direc) {
    
    Cell& sourceCell = m_cells[sourceCellIndex];
    Cell& targetCell = m_cells[targetCellIndex];

    auto [forwardConstraint, reverseConstraint] = Constraint::genConstraintPair(co, &sourceCell, &targetCell, direc);
    
    sourceCell.addConstraint(forwardConstraint);
    targetCell.addConstraint(reverseConstraint);
    
    return;
}

int ConstraintSatisfactionProblem::getCellValue(unsigned long cellIdx) const {
    return m_cells[cellIdx].getVal();
}

Cell ConstraintSatisfactionProblem::getCell(unsigned long cellIdx) const {
    return m_cells[cellIdx];
}

std::vector<Cell> ConstraintSatisfactionProblem::getCells() const {
    return m_cells;
}

std::vector<Cell> ConstraintSatisfactionProblem::solve() {
    return m_cells;
}
