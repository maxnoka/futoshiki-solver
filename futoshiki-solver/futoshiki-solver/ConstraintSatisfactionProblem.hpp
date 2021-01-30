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

struct addConstraintInformation {
    ConstraintOperator co;
    unsigned long sourceCellIndex;
    unsigned long targetCellIndex;
    ConstraintDirection direc;
    
    addConstraintInformation(ConstraintOperator co,
                             unsigned long sourceCellIndex, unsigned long targetCellIndex,
                             ConstraintDirection direc) :
        co(co), sourceCellIndex(sourceCellIndex), targetCellIndex(targetCellIndex), direc(direc) {}
};

class ConstraintSatisfactionProblem {
public:
    ConstraintSatisfactionProblem() = default;
    ConstraintSatisfactionProblem(std::vector<Cell> initial_cells);
    
    ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other);
    
    void addConstraint(ConstraintOperator co,
                                 unsigned long sourceCellIndex, unsigned long targetCellIndex,
                                 ConstraintDirection direc);
    
    void addCoupledNotEqualConstraint(std::vector<unsigned long> cellIndeces);
    
    std::tuple<std::vector<Cell>, bool> solve(bool checkUnique);
    bool isValid() const;
    
    void debugPrint();
    bool isSolved() const;
    
    int getCellValue(unsigned long cellIdx) const;
    Cell& getCell(unsigned long cellIdx);
    
    std::vector<Cell> getCells() const;
    
    unsigned long m_numUnsolved;
    
private:
    std::vector<Cell> m_cells;
    
    std::tuple<std::vector<Cell>, bool> solveByGuessing(bool checkUnique);
    int getCellWithFewestRemainingOptions();
    std::vector<int> getRemainingCellIdxs();
    
    unsigned long m_numCells;
    
    // required for deep copying
    std::vector<addConstraintInformation> addedSingleTargetConstraints;
    std::vector<std::vector<unsigned long>> addCoupledNotEqualConstraints;
    
};

#endif /* ConstraintSatisfactionProblem_hpp */
