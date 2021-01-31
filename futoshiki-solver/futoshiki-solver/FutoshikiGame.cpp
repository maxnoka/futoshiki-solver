//
//  FutoshikiGame.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include <sstream>
#include <stdexcept>
#include <sstream>
#include <random>

#include "utils.hpp"

#include "FutoshikiGame.hpp"
#include "Constraint.hpp"

namespace {
    auto constexpr maxSolveSize = 7;
}

FutoshikiGame::FutoshikiGame(int gameSize) :
m_Csp(std::vector<Cell>(gameSize*gameSize, {Utils::genSetSequence(gameSize)})),
    numRows(gameSize),
    numCols(gameSize)
{
    addColConstraints();
    addRowConstraints();
}

FutoshikiGame::FutoshikiGame(int numRows, int numCols) :
m_Csp(std::vector<Cell>(numRows*numCols, {Utils::genSetSequence(std::max(numRows, numCols))})),
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
    
    auto cells = std::vector<std::vector<Cell>>(numRows, std::vector<Cell>(numCols, {Utils::genSetSequence(std::max(numRows, numCols))}));
    
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j){
            cells[i][j].setVal(initial_values[i][j]);
        }
    }
    
    m_Csp = ConstraintSatisfactionProblem(Utils::flattenVector2d<Cell>(cells));
    addColConstraints();
    addRowConstraints();
}

FutoshikiGame::FutoshikiGame(std::string gridString, std::string constaintStrings)
    : numRows()
    , numCols()
{
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j){
 //           cells[i][j].setVal(initial_values[i][j]);
        }
    }
}

bool FutoshikiGame::solve(bool checkUnique) {
    if (numCols > maxSolveSize || numRows > maxSolveSize) {
        return false;
    }
    auto [sol, proovedUnique] = m_Csp.solve(checkUnique);
    return proovedUnique;
}

bool FutoshikiGame::isValid() const {
    return m_Csp.isValid();
}

void FutoshikiGame::generate() {
    std::mt19937 gen{std::random_device{}()};
    
    std::vector<std::tuple<std::tuple<unsigned long, unsigned long>, std::tuple<unsigned long, unsigned long>>> possibleConstraintsToAdd;
    
    // the constraints between cells of the same row
    for (int rowIdx=0; rowIdx < numRows; ++rowIdx) {
        for (int colIdx=0; colIdx < numCols - 1; ++colIdx) {
            possibleConstraintsToAdd.push_back(std::make_tuple(std::make_tuple(rowIdx, colIdx), std::make_tuple(rowIdx, colIdx+1)));
        }
    }
    
    for (int rowIdx=0; rowIdx < numRows - 1; ++rowIdx) {
        for (int colIdx=0; colIdx < numCols; ++colIdx) {
            possibleConstraintsToAdd.push_back(std::make_tuple(std::make_tuple(rowIdx, colIdx), std::make_tuple(rowIdx+1, colIdx)));
        }
    }
    
    std::shuffle(possibleConstraintsToAdd.begin(), possibleConstraintsToAdd.end(),
                        std::mt19937{std::random_device{}()});
    
    bool proovedUnique = false;
    while(!proovedUnique) {
        std::vector<Cell> result;
        ConstraintSatisfactionProblem branchCsp(m_Csp);
        std::tie(result, proovedUnique) = branchCsp.solve(true);
        
        if (!proovedUnique) { // now change the starting point by adding a constraint
            std::vector<double> weights{50,50};
            std::discrete_distribution<int> dist(std::begin(weights), std::end(weights));
        
            if (dist(gen)) { // add a number to the starting board
                // get random remaining cell
                auto cellsToGuessIdxs = m_Csp.getRemainingCellIdxs(); // sorted with the cells with fewest remaining options first
                std::shuffle(cellsToGuessIdxs.begin(), cellsToGuessIdxs.end(),
                                    std::mt19937{std::random_device{}()});
                auto cellToGuessIdx = cellsToGuessIdxs[0];
                m_Csp.getCell(cellToGuessIdx).setVal(result[cellToGuessIdx].getVal());
                m_Csp.m_numUnsolved -= 1;
            }
            else { // add a constraint to the starting point
                auto constraintToAdd = possibleConstraintsToAdd.back();
                possibleConstraintsToAdd.pop_back();
                
                auto sourceCellCoords = std::get<0>(constraintToAdd);
                auto targetCellCoords = std::get<1>(constraintToAdd);
                
                unsigned long sourceCellIndex = std::get<0>(sourceCellCoords) * numCols + std::get<1>(sourceCellCoords);
                unsigned long targetCellIndex = std::get<0>(targetCellCoords) * numCols + std::get<1>(targetCellCoords);
                
                ConstraintOperator co = result[targetCellIndex].getVal() < result[sourceCellIndex].getVal() ? ConstraintOperator::isGreaterThan_CO : ConstraintOperator::isLessThan_CO;
                addInequalityConstraint(co, std::get<0>(constraintToAdd), std::get<1>(constraintToAdd));
            }
        }
    }
}

// a single constraint results in to two constraints:
// e.g. if [source] < [target], then it also implies [target] > [source]
void FutoshikiGame::addInequalityConstraint(ConstraintOperator co,
                   std::tuple<unsigned long, unsigned long> sourceCellCoords,
                   std::tuple<unsigned long, unsigned long> targetCellCoords) {
    
    unsigned long sourceCellIndex = std::get<0>(sourceCellCoords) * numCols + std::get<1>(sourceCellCoords);
    unsigned long targetCellIndex = std::get<0>(targetCellCoords) * numCols + std::get<1>(targetCellCoords);
    
    auto direc = Constraint::determineConstraintDirection(sourceCellCoords, targetCellCoords);
    
    m_Csp.addConstraint(co, sourceCellIndex, targetCellIndex, direc);
    return;
}

void FutoshikiGame::addRowConstraints() {
    // add row inequality constraints
    for (int rowIdx = 0; rowIdx < numRows; ++rowIdx) {
        std::vector<unsigned long> coupledCellIndeces;
        for (int sourceCellColIdx = 0; sourceCellColIdx < numCols; ++sourceCellColIdx) {
            coupledCellIndeces.push_back(rowIdx * numCols + sourceCellColIdx);
        }
        m_Csp.addCoupledNotEqualConstraint(coupledCellIndeces);
    }
}

void FutoshikiGame::addColConstraints() {
    // add row inequality constraints
    for (int colIdx = 0; colIdx < numRows; ++colIdx) {
        std::vector<unsigned long> coupledCellIndeces;
        for (int sourceCellRowlIdx = 0; sourceCellRowlIdx < numCols; ++sourceCellRowlIdx) {
            coupledCellIndeces.push_back(sourceCellRowlIdx * numCols + colIdx);
        }
        m_Csp.addCoupledNotEqualConstraint(coupledCellIndeces);
    }
}

int FutoshikiGame::getCellValue(unsigned long colIdx, unsigned long rowIdx) const {
    return m_Csp.getCellValue(rowIdx * numCols + colIdx);
}

Cell FutoshikiGame::getCell(unsigned long colIdx, unsigned long rowIdx) {
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
            std::vector<Constraint*> cellConstraints = getCell(j, i).getCellConstraints();
            for (Constraint* c : cellConstraints) {
                // avoid double printing by only print constrains down and right
                std::stringstream constraintSymbol;
                constraintSymbol << *c;
                switch (c->getDirection()) {
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

std::string FutoshikiGame::serializeGrid() {
    std::stringstream ss;
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j){
            ss << getCell(j, i) << ',';
        }
        ss.seekp(-1, std::ios_base::end);
        ss << '\n';
    }
    return ss.str();
}
    
std::string FutoshikiGame::serializeConstraints() {
    std::stringstream ss;
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j){
            std::vector<Constraint*> cellConstraints = getCell(j, i).getCellConstraints();
            for (Constraint* c : cellConstraints) {
                // avoid double printing by only print constrains down and right
                switch (c->getDirection()) {
                    case ConstraintDirection::Right:
                        ss << i << ',' << j << ':';
                        ss << i << ',' << j + 1 << ':';
                        ss << *c;
                        ss << '\n';
                        break;
                    case ConstraintDirection::Down:
                        ss << i << j << ',' << ':';
                        ss << i + 1 << ',' << j << ':';
                        ss << *c;
                        ss << '\n';
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    return ss.str();
}

std::vector<std::vector<int> > FutoshikiGame::deserializeGrid(std::string board) {
    std::vector<std::vector<int>> initialValues;
    
    auto ss = std::stringstream{board};
    for (std::string line; std::getline(ss, line, '\n');) {
        std::vector<int> lineValues;
        size_t last = 0; size_t next = 0;
        while ((next = line.find(",", last)) != std::string::npos) {
            lineValues.emplace_back(stoi(line.substr(last, next-last)));
            last = next + 1;
        }
        lineValues.emplace_back(stoi(line.substr(last)));
        initialValues.emplace_back(lineValues);
    }
    return initialValues;
}

void FutoshikiGame::addInequalityConstraints(std::string serializedConstraints) {
    auto ss = std::stringstream{serializedConstraints};
    for (std::string line; std::getline(ss, line, '\n');) {
        auto next = line.find(",", 0);
        auto sourceCoordsX = stoi(line.substr(0, next));
        auto last = next + 1;
        next = line.find(":", last);
        auto sourceCoordsY = stoi(line.substr(last, next));
        last = next + 1;
        next = line.find(",", last);
        auto targetCoordsX = stoi(line.substr(last, next));
        last = next + 1;
        next = line.find(":", last);
        auto targetCoordsY = stoi(line.substr(last, next));
        last = next + 1;
        auto operatorString = line.substr(last);
        
        addInequalityConstraint(Constraint::stringToOperator(operatorString),
                                std::make_tuple(sourceCoordsX, sourceCoordsY),
                                std::make_tuple(targetCoordsX, targetCoordsY));
    }
}
