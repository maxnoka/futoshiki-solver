//
//  main.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "ConstraintSatisfactionProblem.hpp"
#include "InequalityConstraint.hpp"
#include "EqualityConstraint.hpp"
#include "Cell.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
 #include <crow.h>
#pragma clang diagnostic pop

#include <iostream>
#include <optional>

int main(int argc, const char * argv[]) {
    // TODO: next step is futoshiki I think!
    // should probably start with "square csp" then "latin square" as the first derived classes
    // also need to do the guessing and exhaustive search methods
    // and generator...
    // and some nice visualizations
    // and interfacing with the web-app
    // nice idea: the chess moves as basis for which cells are not allowed to be equal
    // a bubble / graph like visualization of the CSP that you can maniuplate,
    // and how the solver traverses the graph somehow
    // would also like to get the "this is the reason why its invalid"
    // heuristic function for guesses...
    
    auto csp = Csp::ConstraintSatisfactionProblem({Csp::Cell::kUnsolvedSymbol, Csp::Cell::kUnsolvedSymbol, Csp::Cell::kUnsolvedSymbol, Csp::Cell::kUnsolvedSymbol, Csp::Cell::kUnsolvedSymbol, 3}, {1, 2, 3});
    csp.AddEqualityConstraint({0, 1, 5}, Csp::EqualityConstraint::EqualityOperator::NotEqualTo); // 0, 1, are not equal to 3 => (1, 2)
    csp.AddInequalityConstraint(0, Csp::InequalityConstraint::InequalityOperator::GreaterThan, 1);
    csp.AddEqualityConstraint({0, 1, 2}, Csp::EqualityConstraint::EqualityOperator::NotEqualTo);
    csp.AddInequalityConstraint(3, Csp::InequalityConstraint::InequalityOperator::GreaterThan, 4);
    
    
    csp.DeterministicSolve();
    csp.dPrint(true);
    
    crow::SimpleApp app;
    CROW_ROUTE(app, "/")
        .name("hello")
    ([&csp](){
        auto response = crow::response{csp.Serialize()};
        response.add_header("Access-Control-Allow-Origin", "*");
        return response;
    });
    
    CROW_ROUTE(app, "/add_json")
        .methods("POST"_method)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);
        long long sum = x["a"].i()+x["b"].i();
        std::ostringstream os;
        os << sum;
        
        auto response = crow::response{os.str()};
        response.add_header("Access-Control-Allow-Origin", "*");
        return response;
    });
    
    // ignore all log
    crow::logger::setLogLevel(crow::LogLevel::Debug);
    //crow::logger::setHandler(std::make_shared<ExampleLogHandler>());

    app.port(18080)
    .multithreaded()
    .run();
    
    return 0;
}
