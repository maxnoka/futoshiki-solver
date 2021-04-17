#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <futoshiki/Futoshiki.hpp>
#include <futoshiki/LatinSquare.hpp>
#include <futoshiki/ConstraintSatisfactionProblem.hpp>
#include <futoshiki/InequalityConstraint.hpp>
#include <futoshiki/EqualityConstraint.hpp>
#include <futoshiki/Cell.hpp>

#include <futoshiki/utils/easylogging++.h>

TEST_CASE( "2x2 complete solve", "[latin]" ) {
    auto csp = Csp::LatinSquare(
        {
            {
                1,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            }
        }
    );
    auto res = csp.Solve();
    
    REQUIRE(res.completeSolve);
}

TEST_CASE( "2x2 Multiple Solutions (solve)", "[latin]" ) {
    auto csp = Csp::LatinSquare(
        {
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            }
        }
    );
    
    auto res = csp.Solve();
    REQUIRE(res.completeSolve);
}

TEST_CASE( "2x2 Multiple Solutions (solve unique)", "[latin]" ) {
    auto csp = Csp::LatinSquare(
        {
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            }
        }
    );
    auto res = csp.SolveUnique();
    REQUIRE(!res.completeSolve);
    REQUIRE(res.reason.reasonType == Csp::ConstraintSatisfactionProblem::SolveSolution::ReasonType::NotUnique);
}

TEST_CASE( "2x2 Turns out Invalid", "[latin]" ) {
    auto csp = Csp::LatinSquare (
        {
            {
                Csp::Cell::kUnsolvedSymbol,
                1
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            }
        }
    );

    csp.AddInequalityConstraint({0, 1}, Csp::Constraint::Operator::GreaterThan, {1, 1});
    auto res = csp.DeterministicSolve();

    REQUIRE(!res.completeSolve);
    REQUIRE(!res.valid);
    REQUIRE(res.reason.reasonType == Csp::ConstraintSatisfactionProblem::SolveSolution::ReasonType::ConstraintCannotBeSatisfied);
}

TEST_CASE( "2x2 Generate", "[futoshiki]" ) {
    auto csp = Csp::Futoshiki(
        {
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            }
        }
    );
    auto generatedRes = csp.Generate();
    auto solveRes = csp.SolveUnique();
    auto generatedReqGuess = crow::json::load(generatedRes.reason.details.dump())["requiredGuessDepth"].i();
    auto solveReqGuess = crow::json::load(solveRes.reason.details.dump())["requiredGuessDepth"].i();
    
    REQUIRE(generatedReqGuess == solveReqGuess);
}

