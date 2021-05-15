//
//  main.cpp
//  MainFutoshiki (used for development adn testing)
//
//  Created by Maximilian Noka on 03/04/2021.
//

#include <futoshiki/LatinSquare.hpp>
#include <futoshiki/utils/easylogging++.h>

INITIALIZE_EASYLOGGINGPP

int main(int argc, const char * argv[]) {
    START_EASYLOGGINGPP(argc, argv);

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

    LOG(INFO) << res;
    
    return 0;
}
