#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

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
