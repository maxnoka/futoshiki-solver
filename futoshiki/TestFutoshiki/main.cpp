#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

#include <futoshiki/CspSolver.hpp>
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
    auto solver = Csp::CspSolver<Csp::LatinSquare>(std::move(csp));
    auto res = solver.Solve();
    
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
    
    auto solver = Csp::CspSolver<Csp::LatinSquare>(std::move(csp));
    auto res = solver.Solve();
    
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
    auto solver = Csp::CspSolver<Csp::LatinSquare>(std::move(csp));
    auto res = solver.SolveUnique();
    
    REQUIRE(!res.completeSolve);
    REQUIRE(res.reason.reasonType == Csp::CspSolver<Csp::LatinSquare>::SolveSolution::ReasonType::NotUnique);
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
    auto solver = Csp::CspSolver<Csp::LatinSquare>(std::move(csp));
    auto res = solver.SolveDeterministic();

    REQUIRE(!res.completeSolve);
    REQUIRE(!res.valid);
    REQUIRE(res.reason.reasonType == Csp::CspSolver<Csp::LatinSquare>::SolveSolution::ReasonType::ConstraintCannotBeSatisfied);
}

TEST_CASE( "2x2 Generate", "[futoshiki]" ) {
    auto generatedCsp = Csp::Futoshiki::Generate(2);
    
    auto solver = Csp::CspSolver<Csp::LatinSquare>(std::move(generatedCsp));
    auto solveRes = solver.SolveUnique();
    
    REQUIRE(solveRes.valid);
    REQUIRE(solveRes.completeSolve);
    
    /*
    auto generatedReqGuess = crow::json::load(generatedRes.reason.details.dump())["requiredGuessDepth"].i();
    auto solveReqGuess = crow::json::load(solveRes.reason.details.dump())["requiredGuessDepth"].i();
    REQUIRE(generatedReqGuess == solveReqGuess);
    */
}
