//
//  main.cpp
//  MainFutoshiki
//
//  Created by Maximilian Noka on 03/04/2021.
//

#include <futoshiki/CspSolver.hpp>
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
    for (int i=0; i < 1; ++i) {
        auto generatedCsp = Csp::Futoshiki::Generate(6);
        generatedCsp.dPrint(false);
        auto solver = Csp::CspSolver<Csp::LatinSquare>(std::move(generatedCsp));
        auto solveRes = solver.SolveUnique();
        solver.GetSolutions().front().csp.get()->dPrint(false);
        LOG(INFO) << "num guesses: " << solver.GetSolutions().front().seq.size();
    }
    // TODO:
    
    return 0;
}
