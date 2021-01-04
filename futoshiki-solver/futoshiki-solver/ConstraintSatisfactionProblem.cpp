//
//  ConstraintSatisfactionProblem.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 04/11/2020.
//

#include "ConstraintSatisfactionProblem.hpp"
#include "SingleTargetConstraint.hpp"
#include "NotEqualConstraint.hpp"

namespace {
const int kMaxSolverIterations = 2000;
}

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(std::vector<Cell> initial_cells) :
    m_cells(initial_cells),
    m_numCells(initial_cells.size())
{
    m_numUnsolved = 0;
    for (auto cell : m_cells) {
        m_numUnsolved += cell.isUnsolved();
    }
}

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other) :
m_cells(other.m_cells),
m_numCells(other.m_numCells),
m_numUnsolved(other.m_numUnsolved)
{
    for (auto &cell :m_cells) {
        cell.clearConstraints();
    }
    
    for (auto c : other.addedSingleTargetConstraints) {
        addConstraint(c.co, c.sourceCellIndex, c.targetCellIndex, c.direc);
    }
    for (auto c : other.addCoupledNotEqualConstraints) {
        addCoupledNotEqualConstraint(c);
    }
}

void ConstraintSatisfactionProblem::addConstraint(ConstraintOperator co,
                                                            unsigned long sourceCellIndex,
                                                            unsigned long targetCellIndex, ConstraintDirection direc) {
    
    Cell& sourceCell = m_cells[sourceCellIndex];
    Cell& targetCell = m_cells[targetCellIndex];

    auto [forwardConstraint, reverseConstraint] = SingleTargetConstraint::genConstraintPair(co, &sourceCell, &targetCell, direc);
    
    sourceCell.addConstraint(forwardConstraint);
    targetCell.addConstraint(reverseConstraint);
    
    addedSingleTargetConstraints.push_back(addConstraintInformation(co, sourceCellIndex, targetCellIndex, direc));
    
    return;
}

void ConstraintSatisfactionProblem::addCoupledNotEqualConstraint(std::vector<unsigned long> cellIndeces) {
    // first create and add the constraints
    for (auto sourceIndex : cellIndeces) {
        Cell& sourceCell = m_cells[sourceIndex];
        
        std::vector<unsigned long> otherCellIndeces(cellIndeces);
        otherCellIndeces.erase(std::find(otherCellIndeces.begin(), otherCellIndeces.end(), sourceIndex));
        
        std::vector<Cell*> targetCells;
        for (auto targetIndex : otherCellIndeces) {
            targetCells.push_back(&m_cells[targetIndex]);
        }
        sourceCell.addConstraint(new NotEqualConstraint(&sourceCell, targetCells));
    }
    
    // now add the compliment links
    for (auto sourceIndex : cellIndeces) {
        NotEqualConstraint* theJustAddedConstraint = dynamic_cast<NotEqualConstraint*>(m_cells[sourceIndex].getCellConstraints().back());
        
        std::vector<unsigned long> otherCellIndeces(cellIndeces);
        otherCellIndeces.erase(std::find(otherCellIndeces.begin(), otherCellIndeces.end(), sourceIndex));
        for (auto targetIndex : otherCellIndeces) {
            theJustAddedConstraint->addComplimentConstraint(m_cells[targetIndex].getCellConstraints().back());
        }
    }
    
    addCoupledNotEqualConstraints.push_back(cellIndeces);
}

int ConstraintSatisfactionProblem::getCellValue(unsigned long cellIdx) const {
    return m_cells[cellIdx].getVal();
}

Cell& ConstraintSatisfactionProblem::getCell(unsigned long cellIdx) {
    return m_cells[cellIdx];
}

std::vector<Cell> ConstraintSatisfactionProblem::getCells() const {
    return m_cells;
}

bool ConstraintSatisfactionProblem::isSolved() const {
    if (m_numUnsolved == 0) return true;
    else return false;
}

void ConstraintSatisfactionProblem::debugPrint() {
    std::cout << "Debug Print ----------------------------------" << std::endl;
    for (auto currentCell : m_cells) {
        std::cout << "\tOne Cell";
        std::cout << "\t\tVal: " << currentCell << std::endl;
        currentCell.debugPrint();
    }
}

int ConstraintSatisfactionProblem::getCellWithFewestRemainingOptions() {
    int bestCellIdx = 0;
    unsigned long fewestOptions = 0;
    
    // get one that isn't already set and might confuse us
    for (int testIdx = 0; testIdx < m_cells.size(); ++testIdx) {
        if (m_cells[testIdx].isUnsolved()) {
            bestCellIdx = testIdx;
            fewestOptions = m_cells[testIdx].getPossibleVals().size();
            break;
        }
    }
        
    for (int testIdx = 0; testIdx < m_cells.size(); ++testIdx) {
        Cell testCell = m_cells[testIdx];
        if (testCell.isUnsolved() && testCell.getPossibleVals().size() < fewestOptions) {
            bestCellIdx = testIdx;
            fewestOptions = testCell.getPossibleVals().size();
        }
    }
    return bestCellIdx;
}

std::vector<Cell> ConstraintSatisfactionProblem::solveByGuessing() {
    int cellToGuessIdx = getCellWithFewestRemainingOptions();
    
    for (auto guess : m_cells[cellToGuessIdx].getPossibleVals()) {
        ConstraintSatisfactionProblem branchCsp(*this);
        try {
            branchCsp.getCell(cellToGuessIdx).setVal(guess);
            branchCsp.m_numUnsolved -= 1;
            return branchCsp.solve();
        } catch (std::runtime_error) {} // branch didn't work, try the next
    }
    throw std::runtime_error("none of the guesses worked, very bad");
    
}

bool ConstraintSatisfactionProblem::isValid() const {
    for (auto &cell : m_cells) {
        for (auto &constraint : cell.getCellConstraints()) {
            if (!constraint->checkValid()) {
                return false;
            };
        }
    }
    return true;
}

std::vector<Cell> ConstraintSatisfactionProblem::solve() {
    //int numCells = m_cells.size();
    //int numIts = 0;
    
     int numIts = 0;
    while (!isSolved() && numIts < kMaxSolverIterations) {
        int numSolvedThisIt = 0;
        for (auto &cell : m_cells) {
            if (cell.isUnsolved()) {
                bool solvedThisCell = false;
                for (auto &constraint : cell.getCellConstraints()) {
                    if (!solvedThisCell) {
                        solvedThisCell = constraint->apply();
                        numSolvedThisIt += solvedThisCell;
                        m_numUnsolved -= solvedThisCell;
                    }
                    else { // to test that all the other constraints are ok!
                        constraint->apply();
                    }
                }
            }
        }
        if (numSolvedThisIt == 0) {
            // std::cerr << "cannot solve without guessing " << std::endl;
            m_cells = solveByGuessing();
            return m_cells;
        }
        ++numIts;
    }
    return m_cells;
}
