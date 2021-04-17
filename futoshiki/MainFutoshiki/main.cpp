//
//  main.cpp
//  MainFutoshiki
//
//  Created by Maximilian Noka on 03/04/2021.
//

#include <futoshiki/Futoshiki.hpp>
#include <futoshiki/LatinSquare.hpp>
#include <futoshiki/ConstraintSatisfactionProblem.hpp>
#include <futoshiki/InequalityConstraint.hpp>
#include <futoshiki/EqualityConstraint.hpp>
#include <futoshiki/Cell.hpp>

#include <futoshiki/utils/easylogging++.h>

int main(int argc, const char * argv[]) {
    START_EASYLOGGINGPP(argc, argv);
    // Load configuration from file
    el::Configurations conf("/Users/MaxNoka/OneDrive/Projects/futoshiki/futoshiki/conf/logging.conf");
    // Reconfigure single logger
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::setVerboseLevel(0);
    
    // TODO: next step is futoshiki I think!
    // and generator...
    // and some nice visualizations
    // and interfacing with the web-app
    // nice idea: the chess moves as basis for which cells are not allowed to be equal
    // a bubble / graph like visualization of the CSP that you can maniuplate,
    // and how the solver traverses the graph somehow
    // would also like to get the "this is the reason why its invalid"
    // heuristic function for guesses...

    for (int i = 0; i < 100; ++i) {

        auto csp = Csp::Futoshiki(
            {
                {
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol
                },
                {
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol
                },
                {
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol
                },
                {
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol
                },
                {
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol,
                    Csp::Cell::kUnsolvedSymbol
                }
            }
        );
        
        auto generatedRes = csp.Generate();
        auto solveRes = csp.SolveUnique();
        LOG(INFO) << "xxx ----------------";
        crow::json::rvalue hi = crow::json::load(generatedRes.reason.details.dump());
        LOG(INFO) << "xxx: " << hi["requiredGuessDepth"].i();
        LOG(INFO) << "xxx: " << solveRes.reason.details.dump();
    }
     
    /*
    auto csp = Csp::Futoshiki(
        {
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            },
            {
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol,
                Csp::Cell::kUnsolvedSymbol
            }
        }
    );
    
    auto res = csp.DeterministicSolve();
    csp.MakeGuess({0, 3});
    csp.DeterministicSolve();
    csp.MakeGuess({6, 1});
    csp.DeterministicSolve();
    csp.MakeGuess({12, 1});
    csp.DeterministicSolve();
    csp.MakeGuess({18, 5});
    csp.DeterministicSolve();
    csp.MakeGuess({24, 5});
    csp.DeterministicSolve();
    csp.MakeGuess({7, 4});
    csp.DeterministicSolve();
    csp.MakeGuess({11, 5}); // 7c
    csp.DeterministicSolve();
    csp.MakeGuess({19, 4});
    csp.DeterministicSolve();
    csp.dPrintGrid();
    csp.MakeGuess({23, 2});
    csp.DeterministicSolve();
    csp.dPrintGrid();
    */
    
    return 0;
}
