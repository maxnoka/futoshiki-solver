//
//  Futoshiki.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 09/04/2021.
//

#include <futoshiki/Futoshiki.hpp>
#include <futoshiki/utils/Utils.hpp>
#include <futoshiki/utils/easylogging++.h>

#include <random>

namespace Csp {

namespace {

std::vector<std::pair<TwoDimCsp::CellCoords, TwoDimCsp::CellCoords> >
GenAdjacentCellPairs(const unsigned long numRows, const unsigned long numCols) {
    std::vector<std::pair<TwoDimCsp::CellCoords, TwoDimCsp::CellCoords> > out;
    for (unsigned long rowIdx=0; rowIdx < numRows; ++rowIdx) {
        for (unsigned long colIdx=0; colIdx < numCols - 1; ++colIdx) {
            out.push_back(std::make_pair(std::make_pair(rowIdx, colIdx), std::make_pair(rowIdx, colIdx+1)));
        }
    }
    
    for (unsigned long rowIdx=0; rowIdx < numRows - 1; ++rowIdx) {
        for (unsigned long colIdx=0; colIdx < numCols; ++colIdx) {
            out.push_back(std::make_pair(std::make_pair(rowIdx, colIdx), std::make_pair(rowIdx+1, colIdx)));
        }
    }
    
    return out;
}
    
}

bool Futoshiki::AddRandomConstraint(
    const Futoshiki& refSolution
) {
    
    std::vector<double> weights{50,50};
    std::discrete_distribution<int> dist(std::begin(weights), std::end(weights));
    std::mt19937 gen{std::random_device{}()};
    
    if (dist(gen)) {
        bool interestingConstraint = true;
        do {
            auto remainingCells = RemainingCellKeys();
            auto unsolvedCell = Utils::SelectRandomly(remainingCells.begin(), remainingCells.end());
            
            bool redundantConstraint =
                m_cells.at(*unsolvedCell)->Value() == *m_defaultPossibleValues.begin() &&
                m_cells.at(*unsolvedCell)->HasAppliedConstraint(static_cast<int>(Constraint::Operator::LessThan) );
            redundantConstraint = redundantConstraint ||
                ( m_cells.at(*unsolvedCell)->Value() == *m_defaultPossibleValues.rbegin() &&
                m_cells.at(*unsolvedCell)->HasAppliedConstraint(static_cast<int>(Constraint::Operator::GreaterThan) ) );
            
            if (!redundantConstraint) {
                m_cells.at(*unsolvedCell)->SetVal(
                    refSolution.m_cells.at(*unsolvedCell)->Value());
                interestingConstraint = true;
            }
            
        }
        while(!interestingConstraint);
    }
    else {
        bool interestingConstraint = false;
        do {
            auto possibleCellPairs = GenAdjacentCellPairs(m_numCols, m_numRows);
            auto cellCoords = Utils::SelectRandomly(possibleCellPairs.begin(), possibleCellPairs.end());
            auto lhsCellIdx = CoordsToIndex(cellCoords->first);
            auto rhsCellIdx = CoordsToIndex(cellCoords->second);

            auto op = refSolution.m_cells.at(lhsCellIdx)->Value() < refSolution.m_cells.at(rhsCellIdx)->Value()
                ? Constraint::Operator::LessThan
                : Constraint::Operator::GreaterThan;
            
            bool bothCellsAlreadySolved =
                m_cells.at(lhsCellIdx)->IsSolved() &&
                m_cells.at(rhsCellIdx)->IsSolved();
            
            bool redundantConstraint =
                m_cells.at(lhsCellIdx)->Value() == *m_defaultPossibleValues.begin() ||
                m_cells.at(lhsCellIdx)->Value() == *m_defaultPossibleValues.rbegin() ||
                m_cells.at(rhsCellIdx)->Value() == *m_defaultPossibleValues.begin() ||
                m_cells.at(rhsCellIdx)->Value() == *m_defaultPossibleValues.rbegin();
            
            interestingConstraint = !bothCellsAlreadySolved && !redundantConstraint;
            
            if (interestingConstraint) {
                AddInequalityConstraint(cellCoords->first, op, cellCoords->second);
            }
        }
        while(!interestingConstraint);
    }
    return true;
}

Futoshiki Futoshiki::Generate(unsigned long size) {
    LOG(INFO) << "Generating Futoshiki puzzle";
    
    Futoshiki out(size);
    
    LOG(INFO) << "Generating reference";
    auto solver = Csp::CspSolver<Futoshiki>(Futoshiki(out));
    auto res = solver.SolveRandom();
    assertm(res.completeSolve, "empty board should be randomly solveable");
    auto reference = solver.GetSolutions().front();
    
    LOG(INFO) << "Adding constraints until uniquely solveable";
    
    // have not proven that we need at least (size - 1) [note we add one at the
    // beginning of the do loop below] constraints for a futoshiki puzzle, but
    // I think we do
    for (int i = 0; i < size - 2; ++i) {
        LOG(INFO) << "Adding Constraint...";
        out.AddRandomConstraint(*reference.csp);
    }
    
    bool nowSolveable = false;
    do {
        LOG(INFO) << "Adding Constraint...";
        out.AddRandomConstraint(*reference.csp);
        
        Futoshiki copy(out);
        auto solver = Csp::CspSolver<Futoshiki>(std::move(copy));
        res = solver.SolveUnique();
        nowSolveable = res.completeSolve;
    }
    while (!nowSolveable);
    return out;
}

}  // ::Csp
