//
//  CspSolver.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 17/04/2021.
//

#ifndef CspSolver_hpp
#define CspSolver_hpp

// #include "Constraint.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
#include <crow.h>
#pragma clang diagnostic pop

#include <vector>
#include <stack>

namespace Csp {

struct Guess;
class ConstraintSatisfactionProblem;

template<typename CSP, typename Sfinae = void>
class CspSolver;

template<typename CSP>
using EnableIfPolicy = typename std::enable_if_t< std::is_base_of<ConstraintSatisfactionProblem, CSP>::value >;

template <typename CSP>
class CspSolver <CSP, EnableIfPolicy<CSP> > {
    static constexpr unsigned int kMaxGuessDepth = 4;
public:
    CspSolver(CSP&& startingPoint);
    
    struct SolveSolution {
        bool completeSolve;
        bool valid; // if invalid, then completeSolve is not defined
        
        enum class ReasonType {
            ManagedToSolve = 0,
            ConstraintCannotBeSatisfied,
            NoGuessesWorked,
            NotUnique,
            GuessDepthExceeded,
            NotYetSolved,
        };
        
        struct Reason {
            ReasonType reasonType;
            crow::json::wvalue details;
        };
        
        crow::json::wvalue ToJson() {
            crow::json::wvalue out;
            out["solved"] = completeSolve;
            std::stringstream ss;
            ss << *this;
            out["outcome"] = ss.str();
            out["details"] = std::move(reason.details);
            
            return out;
        };
        
        Reason reason;
    };
    
    friend std::ostream& operator<< (std::ostream& os, const SolveSolution& sol) {
        if (sol.completeSolve) {
            os << "Solved.";
        }
        else {
            os << "Could not solve. ";
        }
        if (!sol.valid) {
            os << "Not valid. Reason: ";
        }
        switch (sol.reason.reasonType) {
            case SolveSolution::ReasonType::NotYetSolved:
                os << "Not finished solving.";
                break;
            case SolveSolution::ReasonType::ManagedToSolve:
                break;
            case SolveSolution::ReasonType::ConstraintCannotBeSatisfied:
                os << "Constraint could not be satisfied.";
                break;
            case SolveSolution::ReasonType::NoGuessesWorked:
                os << "No options for a cell lead to valid solutions.";
                break;
            case SolveSolution::ReasonType::NotUnique:
                os << "Not uniquely solvable.";
                break;
            case SolveSolution::ReasonType::GuessDepthExceeded:
                os << "Not proveably uniquely solveable in < " << kMaxGuessDepth << " guesses.";
        }
        return os;
    };
    
private:
    using GuessSequence = std::vector<Guess>;
    struct SolveAttempt {
        std::unique_ptr<CSP> csp;
        GuessSequence seq;
        SolveAttempt() = default;
        SolveAttempt(const SolveAttempt& other)
            : csp(std::make_unique<CSP>(*other.csp))
            , seq(other.seq)
        { }
        SolveAttempt& operator=(SolveAttempt& other)
        {
            std::swap(csp, other.csp);
            std::swap(seq, other.seq);
            return *this;
        }
    };
    
public:
    SolveSolution SolveDeterministic();
    SolveSolution Solve();
    SolveSolution SolveRandom();
    SolveSolution SolveUnique();
    // SolveSolution SolveStep();
    
    const std::vector<SolveAttempt>& GetSolutions() const { return m_foundSolutions; }
    
private:
    // adds on top of the working branch and sets m_working
    void MakeGuess(const Guess& guess);
    SolveSolution SolveWorking(bool random, bool checkUnique);
    SolveSolution Solve(bool random);
    
    std::unique_ptr<CSP> m_startingPoint;
    std::stack<SolveAttempt> m_workingBranch;
    // the top of the working branch
    CSP* m_working;
    std::vector<SolveAttempt> m_foundSolutions;
    
    // std::vector< std::shared_ptr<Constraint> >::iterator constraintIt;
}; // CspSolver

} // ::Csp

#endif /* CspSolver_hpp */
