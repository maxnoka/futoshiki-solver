//
//  CspSolver.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 17/04/2021.
//

#include <futoshiki/CspSolver.hpp>

#include <futoshiki/ConstraintSatisfactionProblem.hpp>
#include <futoshiki/Futoshiki.hpp>

#include <futoshiki/utils/easylogging++.h>

namespace Csp {

template <typename CSP>
CspSolver<CSP, EnableIfPolicy<CSP>>::CspSolver(CSP&& startingPoint)
    : m_startingPoint(std::make_unique<CSP>(startingPoint))
    , m_workingBranch()
    , m_working()
    , m_foundSolutions()
{
    SolveAttempt initialWorking;
    initialWorking.csp = std::make_unique<CSP>(startingPoint);
    
    m_workingBranch.push(std::move(initialWorking));
    m_working = m_workingBranch.top().csp.get();
}

template <typename CSP>
typename CspSolver<CSP, EnableIfPolicy<CSP>>::SolveSolution
CspSolver<CSP, EnableIfPolicy<CSP>>::SolveDeterministic() {
    VLOG(2) << "Starting deterministic solve ";
    while(m_working->m_numActiveConstraints > 0) {
        for (auto& constraint : m_working->m_constraints) {
            if (constraint->IsActive()) {
                if(!constraint->Apply()) {
                    VLOG(2) << "Constraint turned out to be invalid";
                    return {
                        false,
                        false,
                        {SolveSolution::ReasonType::ConstraintCannotBeSatisfied, constraint->Serialize()}
                    }; // invalid
                }
            }
        }
    }
    
    for (auto& constraint : m_working->m_constraints) {
        if (constraint->ShouldStillCheckValid()) {
            if(!constraint->Valid()) {
                VLOG(2) << "Constraint turned out to be invalid";
                return {
                    false,
                    false,
                    {SolveSolution::ReasonType::ConstraintCannotBeSatisfied, constraint->Serialize()}
                }; // invalid
            }
            constraint->SetChecked();
        }
    }
    
    VLOG(2) << "Finished deterministic solve (" << (m_working->m_completelySolved ? "SOLVED" : "UNSOLVED") << ")";
    m_working->m_provenValid = true;
    return {
        m_working->m_completelySolved,
        m_working->m_provenValid,
        {SolveSolution::ReasonType::ManagedToSolve}
    };
}

template <typename CSP>
typename CspSolver<CSP, EnableIfPolicy<CSP>>::SolveSolution
CspSolver<CSP, EnableIfPolicy<CSP> >::SolveWorking(bool random, bool checkUnique) {
    auto depthGuess = m_workingBranch.size() - 1;
    
    // Try to solve as is
    auto deterministicRes = SolveDeterministic();
    if (!deterministicRes.valid) {
        if (depthGuess > 0) {
            auto& lastGuess = m_workingBranch.top().seq.back();
            VLOG(2) << "Guess " << lastGuess.Serialize().dump()
                << " (" << depthGuess << ") was not valid";
        }
        else {
            VLOG(2) << "Guess {}" << " (" << depthGuess << ") was not valid";
        }
        return deterministicRes;
    }
    if (deterministicRes.completeSolve) {
        m_foundSolutions.push_back( m_workingBranch.top() );
        
        if (depthGuess > 0) {
            auto& lastGuess = m_workingBranch.top().seq.back();
            VLOG(2) << "Guess " << lastGuess.Serialize().dump()
            << " (" << depthGuess << ") produced solution";
        }
        else {
            VLOG(2) << "Guess {}" << " (" << depthGuess << ") produced solution";
        }
        deterministicRes.reason.details["requiredGuessDepth"] = depthGuess;
        return deterministicRes;
    }
    
    // Require guess
    ++depthGuess;
    VLOG(2) << "Require guess. Depth to " << depthGuess;
    if (checkUnique && depthGuess > kMaxGuessDepth) {
        VLOG(2) << "Require guess, but max guess depth exceeded: "
                  << depthGuess << "/" << kMaxGuessDepth << ".";
        return {
            false,
            false,
            {SolveSolution::ReasonType::GuessDepthExceeded, {} }
        };
    }
    
    auto guesses = m_working->GetGuesses(random);
    for (auto guess : guesses) {
        VLOG(2) << "Trying Guess " << guess.Serialize().dump()
            << " (" << depthGuess << ")";
        
        MakeGuess(guess); // Updates the working csp
        auto branchRes = SolveWorking(random, checkUnique);
        m_workingBranch.pop();

        // if this is the case then we do not bother searching further
        if (checkUnique && !branchRes.valid) {
            if (branchRes.reason.reasonType == SolveSolution::ReasonType::GuessDepthExceeded) {
                return branchRes;
            }
        }
        else if (branchRes.completeSolve) {
            if (!checkUnique) {
                return branchRes;
            }
            
            if (m_foundSolutions.size() > 1) {
                VLOG(2) << "Found two solutions. Not unique (" << depthGuess << ")";
                crow::json::wvalue reasonJson;
                reasonJson[0] = m_foundSolutions.front().csp->Serialize();
                reasonJson[1] = m_foundSolutions.back().csp->Serialize();
                m_foundSolutions.pop_back();
                m_foundSolutions.pop_back();
                return {
                    false,
                    false,
                    {SolveSolution::ReasonType::NotUnique, std::move(reasonJson) }
                };
            }
        }
    }

    // assertm(checkUnique, "should not have gotten to this point when not checking uniqueness");
    if (m_foundSolutions.size() > 0) {
        assertm(m_foundSolutions.size() == 1, "should not get to this point with more than one solution");
        VLOG(2) << "Found only one solution. Proven unique (" << depthGuess << ")";
        crow::json::wvalue reasonJson;
        reasonJson["requiredGuessDepth"] = depthGuess;
        return { true, true, {SolveSolution::ReasonType::ManagedToSolve, std::move(reasonJson)} };
    }
    
    VLOG(2) << "No quesses worked. Proven invalid.";
    crow::json::wvalue reasonJson;
    size_t numGuesses = guesses.size();
    for (unsigned int i = 0; i < numGuesses; ++i) {
        reasonJson[i] = guesses[i].Serialize();
    }
    return {
        false,
        false,
        {SolveSolution::ReasonType::NoGuessesWorked, std::move(reasonJson) }
    };
}

template <typename CSP>
typename CspSolver<CSP, EnableIfPolicy<CSP>>::SolveSolution
CspSolver<CSP, EnableIfPolicy<CSP>>::SolveRandom() {
    LOG(INFO) << "Solving randomly...";
    auto res = SolveWorking(true, false);
    if (!res.valid) {
        LOG(INFO) << "Finished solving. Not valid";
        LOG(INFO) << res;
    }
    if (res.completeSolve) {
        LOG(INFO) << "Finished solving. Found solution.";
        res.reason.details["requiredGuessDepth"] = m_foundSolutions.front().seq.size();
    }
    return res;
}

template <typename CSP>
typename CspSolver<CSP, EnableIfPolicy<CSP>>::SolveSolution
CspSolver<CSP, EnableIfPolicy<CSP>>::Solve() {
    LOG(INFO) << "Solving...";
    auto res = SolveWorking(false, false);
    if (!res.valid) {
        LOG(INFO) << res;
    }
    if (res.completeSolve) {
        LOG(INFO) << "Finished solving. Found solution.";
        res.reason.details["requiredGuessDepth"] = m_foundSolutions.front().seq.size();
    }
    return res;
}

template <typename CSP>
typename CspSolver<CSP, EnableIfPolicy<CSP>>::SolveSolution
CspSolver<CSP, EnableIfPolicy<CSP>>::SolveUnique() {
    LOG(INFO) << "Solving uniquely...";
    auto res = SolveWorking(false, true);
    if (!res.valid) {
        LOG(INFO) << res;
    }
    else if (res.completeSolve) {
        LOG(INFO) << "Finished solving. Found solution.";
        res.reason.details["requiredGuessDepth"] = m_foundSolutions.front().seq.size();
    }
    return res;
}

template <typename CSP>
void CspSolver<CSP, EnableIfPolicy<CSP>>::MakeGuess(const Guess& guess) {
    auto branch = m_workingBranch.top();
    branch.seq.push_back(guess);
    branch.csp->m_cells.at(guess.cellKey)->SetVal(guess.val);
    m_workingBranch.push( std::move(branch) );
    m_working = m_workingBranch.top().csp.get();
}

template class CspSolver<ConstraintSatisfactionProblem>;
template class CspSolver<LatinSquare>;
template class CspSolver<Futoshiki>;
} // ::Csp
