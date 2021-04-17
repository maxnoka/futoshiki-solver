//
//  CspSolver.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 17/04/2021.
//

#ifndef CspSolver_hpp
#define CspSolver_hpp

#include "Constraint.hpp"
#include "ConstraintSatisfactionProblem.hpp"

#include <vector>

namespace Csp {

class CspSolver {
public:
    CspSolver(ConstraintSatisfactionProblem&& startingPoint);
    
    SolveSolution DeterministicSolve();
    // also does guessing
    SolveSolution Solve();
    SolveSolution SolveRandom();
    SolveSolution SolveUnique();
    
    Solve();
    SolveUnique();
    SolveDeterministic();
    
    SolveStep();
    
private:
    ConstraintSatisfactionProblem m_startingPoint;
    
    using std::vector<Guesses> GuessSequence;
    using std::pair<ConstraintSatisfactionProblem, GuessSequence> SolveAttempt;
    
    SolveAttempt m_working;
    std::vector<SolveAttempt> m_foundSolutions;
    
    ConstraintSatisfactionProblem m_workingCopy;
    
    unsigned int m_numGuesses;
    std::vector< std::shared_ptr<Constraint> >::iterator constraintIt;
} // CspSolver

} // ::Csp

#endif /* CspSolver_hpp */
