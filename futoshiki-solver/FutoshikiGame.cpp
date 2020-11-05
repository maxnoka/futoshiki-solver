//
//  FutoshikiGame.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include <sstream>
#include <stdexcept>

#include "Utils/utils.hpp"

#include "FutoshikiGame.hpp"
#include "Constraint.hpp"


FutoshikiGame::FutoshikiGame(int gameSize) :
    m_Csp(std::vector<Cell>(gameSize*gameSize, 0)),
    numRows(gameSize),
    numCols(gameSize)
{
    addColConstraints();
    addRowConstraints();
}

FutoshikiGame::FutoshikiGame(int numRows, int numCols) :
    m_Csp(std::vector<Cell>(numRows*numCols, 0)),
    numRows(numRows),
    numCols(numCols)
{
    addColConstraints();
    addRowConstraints();
}

FutoshikiGame::FutoshikiGame(std::vector<std::vector<Cell>> initial_values) :
    m_Csp(Utils::flattenVector2d<Cell>(initial_values)),
    numRows(initial_values.size()),
    numCols(initial_values[0].size())
{
    if (!Utils::vector2dIsRectangular<Cell>(initial_values)) {
        throw std::invalid_argument("initial cell values vector is not square");
    }
    addColConstraints();
    addRowConstraints();
}

FutoshikiGame::FutoshikiGame(std::vector<std::vector<int>> initial_values) :
    numRows(initial_values.size()),
    numCols(initial_values[0].size())
{
    if (!Utils::vector2dIsRectangular<int>(initial_values)) {
        throw std::invalid_argument("initial cell values vector is not square");
    }
    
    auto cells = std::vector<std::vector<Cell>>(numRows, std::vector<Cell>(numCols, 0));
    
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j){
            cells[i][j] = Cell(initial_values[i][j]);
        }
    }
    
    m_Csp = ConstraintSatisfactionProblem(Utils::flattenVector2d<Cell>(cells));
    addColConstraints();
    addRowConstraints();
}

// a single constraint results in to two constraints:
// e.g. if [source] < [target], then it also implies [target] > [source]
void FutoshikiGame::addInequalityConstraint(ConstraintOperator co,
                   std::tuple<unsigned long, unsigned long> sourceCellCoords,
                   std::tuple<unsigned long, unsigned long> targetCellCoords, ConstraintDirection direc) {
    
    unsigned long sourceCellIndex = get<0>(sourceCellCoords) + get<1>(sourceCellCoords) * numCols;
    unsigned long targetCellIndex = get<0>(targetCellCoords) + get<1>(targetCellCoords) * numCols;
    
    m_Csp.addConstraint(co, sourceCellIndex, targetCellIndex, direc);
    return;
}

void FutoshikiGame::addRowConstraints() {
    // add row inequality constraints
    for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) {
        for (int sourceCellColIdx = 0; sourceCellColIdx < numCols -  1; ++sourceCellColIdx) {
            // note what we do here as to not double add constraints (only do combinations not permutations of pairs)
            for (int targetCellColIdx = sourceCellColIdx + 1; targetCellColIdx < numCols; ++targetCellColIdx) {
                unsigned long sourceCellIndex = rowIdx * numCols + sourceCellColIdx;
                unsigned long targetCellIndex = rowIdx * numCols + targetCellColIdx;
                m_Csp.addConstraint(ConstraintOperator::isNotEqualTo_CO, sourceCellIndex, targetCellIndex, ConstraintDirection::Complex);
            }
        }
    }
}

void FutoshikiGame::addColConstraints() {
    // add row inequality constraints
    for (int colIdx = 0; colIdx < numRows; ++colIdx) {
        for (int sourceCellRowlIdx = 0; sourceCellRowlIdx < numCols -  1; ++sourceCellRowlIdx) {
            // note what we do here as to not double add constraints (only do combinations not permutations of pairs)
            for (int targetCellRowIdx = sourceCellRowlIdx + 1; targetCellRowIdx < numCols; ++targetCellRowIdx) {
                unsigned long sourceCellIndex = sourceCellRowlIdx * numCols + colIdx;
                unsigned long targetCellIndex = targetCellRowIdx * numCols + colIdx;
                m_Csp.addConstraint(ConstraintOperator::isNotEqualTo_CO, sourceCellIndex, targetCellIndex, ConstraintDirection::Complex);
            }
        }
    }
}

int FutoshikiGame::getCellValue(unsigned long colIdx, unsigned long rowIdx) const {
    return m_Csp.getCellValue(rowIdx * numCols + colIdx);
}

Cell FutoshikiGame::getCell(unsigned long colIdx, unsigned long rowIdx) const {
    return m_Csp.getCell(rowIdx * numCols + colIdx);
}

void FutoshikiGame::debugPrint() {
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j){
            Cell currentCell = getCell(j, i);
            std::cout << "Cell[" << i << "," << j << "]: ";
            std::cout << "\tVal: " << currentCell << std::endl;
            currentCell.debugPrint();
        }
    }
}

std::vector<std::string> FutoshikiGame::printValues() {
    std::vector<std::string> stringGrid;

    for (int i = 0; i < numRows; ++i) {
        std::stringstream ss;
        for (int j = 0; j < numCols; ++j){
            ss << " " << getCell(j, i) << "  ";
        }
        stringGrid.push_back(ss.str());
        
        // add the vertical gaps
        std::stringstream ss2;
        for (int j = 0; j < numCols; ++j){
            ss2 << "    ";
        }
        stringGrid.push_back(ss2.str());
    }
    
    return stringGrid;
}

void FutoshikiGame::printBoard() {
    std::vector<std::string> stringGrid(printValues());
    
    // populate the constraints
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j){
            std::vector<Constraint> cellConstraints = getCell(j, i).getCellConstraints();
            for (Constraint c : cellConstraints) {
                // avoid double printing by only print constrains down and right
                std::stringstream constraintSymbol;
                constraintSymbol << c;
                switch (c.getDirection()) {
                    case ConstraintDirection::Right:
                        //     0     1     2     3
                        //    " " + "2" + " " + "<"
                        stringGrid[i*2].replace(j*4+3, 1, constraintSymbol.str());
                        break;
                    case ConstraintDirection::Down:
                        //     0     1     2     3
                        //    " " + "2" + " " + " "
                        //    " " + "<" + " " + " "
                        stringGrid[i*2 + 1].replace(j*4+1, 1, constraintSymbol.str());
                    default:
                        break;
                }
            }
        }
    }
    
    // write to std::out
    std::cout << "---- Board ----" << std::endl << std::endl;
    
    for (std::string aRow : stringGrid) {
        std::cout << "    ";
        std::cout << aRow << std::endl;
    }
    
    std::cout << "---------------" << std::endl;
}
