//
//  ConstraintSatisfactionProblem.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 04/11/2020.
//

#include "ConstraintSatisfactionProblem.hpp"
#include "SingleTargetConstraint.hpp"
#include "NotEqualConstraint.hpp"

#include <algorithm>
#include <random>

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

std::vector<int> ConstraintSatisfactionProblem::getRemainingCellIdxs() {
    struct numOptionsComparison {
        numOptionsComparison(ConstraintSatisfactionProblem* csp)
            : m_csp(csp)
        {};
        
        bool operator() (int i, int j) {
            return m_csp->getCell(i).getPossibleVals() > m_csp->getCell(j).getPossibleVals();
        }
        
        ConstraintSatisfactionProblem* m_csp;
    };
    
    std::vector<int> remainingCellIdxs;
    for (int testIdx = 0; testIdx < m_cells.size(); ++testIdx) {
        if (m_cells[testIdx].isUnsolved()) {
            remainingCellIdxs.push_back(testIdx);
        }
    }
    
    numOptionsComparison comparer(this);
    std::sort(remainingCellIdxs.begin(), remainingCellIdxs.end(), comparer);
    return remainingCellIdxs;
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

std::tuple<std::vector<Cell>, bool> ConstraintSatisfactionProblem::solveByGuessing(bool checkUnique) {
    auto cellsToGuessIdxs = getRemainingCellIdxs(); // sorted with the cells with fewest remaining options first
    
    std::vector<Cell> lastSol;
    bool alreadyFoundASolution = false;
    
    for (auto cellToGuessIdx : cellsToGuessIdxs) {
        // check all possible options in one guessable cell
        std::set<int> temp = m_cells[cellToGuessIdx].getPossibleVals();
        std::vector<int> possible_guesses;
        std::copy(temp.begin(), temp.end(), std::back_inserter(possible_guesses));
        std::shuffle(possible_guesses.begin(), possible_guesses.end(),
                            std::mt19937{std::random_device{}()});
        for (auto guess : possible_guesses) {
            ConstraintSatisfactionProblem branchCsp(*this);
            try {
                branchCsp.getCell(cellToGuessIdx).setVal(guess);
                branchCsp.m_numUnsolved -= 1;
                auto [oneSol, proovedUnique] = branchCsp.solve(checkUnique);
                // if we found multiple solution in that branch no need to continue checking
                // similarly if we don't checkUnique at all
                // finally if we found a solution, i.e. didn't throw, then we can confirm not unique
                if (!proovedUnique || !checkUnique || alreadyFoundASolution) {
                    // it could be that multiple guesses lead to the same solution:
                    struct cellValueComparison {
                        bool operator() (const Cell& A, const Cell& B) {
                            return A.getVal() == B.getVal();
                        }
                    } comp;
                    
                    if (alreadyFoundASolution && std::equal(lastSol.begin(), lastSol.end(), oneSol.begin(), comp)) {
                        continue;
                    }
                    return std::make_tuple(oneSol, false);
                }
                lastSol = oneSol; // to keep for later potentially
                alreadyFoundASolution = true;
            } catch (std::runtime_error) {} // branch didn't work, try the next
        }
    }
    // we made it all the way to the end and only one guess in one cell worked
    if (alreadyFoundASolution) {
        return std::make_tuple(lastSol, true);
    }
    // todo add the functionality here to check the other guesses
    throw std::runtime_error("none of the guesses worked, very bad");
    
}

void ConstraintSatisfactionProblem::generate() {
    bool proovedUnique = false;
    while(!proovedUnique) {
        std::vector<Cell> result;
        ConstraintSatisfactionProblem branchCsp(*this);
        std::tie(result, proovedUnique) = branchCsp.solve(true);
        
        if (!proovedUnique) { // now change the starting point by adding a constraint
            // get random remaining cell
            auto cellsToGuessIdxs = getRemainingCellIdxs(); // sorted with the cells with fewest remaining options first
            std::shuffle(cellsToGuessIdxs.begin(), cellsToGuessIdxs.end(),
                                std::mt19937{std::random_device{}()});
            auto cellToGuessIdx = cellsToGuessIdxs[0];
            m_cells[cellToGuessIdx].setVal(result[cellToGuessIdx].getVal());
            m_numUnsolved -= 1;
        }
    }
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

std::tuple<std::vector<Cell>, bool> ConstraintSatisfactionProblem::solve(bool checkUnique) {
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
            bool proovedUnique;
            std::tie(m_cells, proovedUnique) = solveByGuessing(checkUnique);
            return std::make_tuple(m_cells, proovedUnique);
        }
        ++numIts;
    }
    // solved without guessing, so unique solution
    return std::make_tuple(m_cells, true);
}
