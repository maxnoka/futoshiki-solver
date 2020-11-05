//
//  FutoshikiGame.hpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#ifndef FutoshikiGame_hpp
#define FutoshikiGame_hpp

#include <stdio.h>
#include <vector>

#include "ConstraintSatisfactionProblem.hpp"
#include "Cell.hpp"
#include "Constraint.hpp"

class FutoshikiGame {
public:
    FutoshikiGame(int gameSize);
    FutoshikiGame(int numRows, int numCols);
    
    FutoshikiGame(std::vector<std::vector<Cell>> initial_values);
    FutoshikiGame(std::vector<std::vector<int>> initial_values);
    
    void addInequalityConstraint(ConstraintOperator co,
                       std::tuple<unsigned long, unsigned long> sourceCellCoords,
                       std::tuple<unsigned long, unsigned long> targetCellCoords,
                       ConstraintDirection direc);
    
    void debugPrint();
    void printBoard();
    
    
    Cell getCell(unsigned long colIdx, unsigned long rowIdx) const;
    int getCellValue(unsigned long colIdx, unsigned long rowIdx) const;
private:
    ConstraintSatisfactionProblem m_Csp;
    
    std::vector<std::string> printValues();
    
    void addColConstraints();
    void addRowConstraints();
    
    unsigned long numRows;
    unsigned long numCols;
};

#endif /* FutoshikiGame_hpp */
