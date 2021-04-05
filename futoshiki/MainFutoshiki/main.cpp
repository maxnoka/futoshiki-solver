//
//  main.cpp
//  MainFutoshiki
//
//  Created by Maximilian Noka on 03/04/2021.
//

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
    el::Loggers::setVerboseLevel(1);
    
    // TODO: next step is futoshiki I think!
    // and generator...
    // and some nice visualizations
    // and interfacing with the web-app
    // nice idea: the chess moves as basis for which cells are not allowed to be equal
    // a bubble / graph like visualization of the CSP that you can maniuplate,
    // and how the solver traverses the graph somehow
    // would also like to get the "this is the reason why its invalid"
    // heuristic function for guesses...
    
    auto csp = Csp::LatinSquare(
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
    auto res = csp.SolveUnique();
    csp.dPrint(true);
    
    LOG(INFO) << res;
    LOG(INFO) << res.reason.details.dump();
    
    return 0;
}
