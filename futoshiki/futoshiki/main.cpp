//
//  main.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "LatinSquare.hpp"
#include "InequalityConstraint.hpp"
#include "EqualityConstraint.hpp"
#include "Cell.hpp"

#include "utils/easylogging++.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
 #include <crow.h>
#pragma clang diagnostic pop

#include <iostream>
#include <optional>

INITIALIZE_EASYLOGGINGPP

// #define WEBSERVER

int main(int argc, const char * argv[]) {
    START_EASYLOGGINGPP(argc, argv);
    // Load configuration from file
    el::Configurations conf("/Users/MaxNoka/OneDrive/Projects/futoshiki/futoshiki/futoshiki/conf/logging.conf");
    // Reconfigure single logger
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::setVerboseLevel(1);
    
    // TODO: next step is futoshiki I think!
    // also need to do the guessing and exhaustive search methods
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
            Csp::Cell::kUnsolvedSymbol
        },
        {
            Csp::Cell::kUnsolvedSymbol,
            Csp::Cell::kUnsolvedSymbol
        }
    }
    );
    
    csp.SolveUnique();
    csp.dPrint(true);
    
#ifdef WEBSERVER
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
#endif // WEBSERVER
    
    return 0;
}
