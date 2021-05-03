//
//  main.cpp
//  MainFutoshiki
//
//  Created by Maximilian Noka on 03/04/2021.
//

#include <futoshiki/CspSolver.hpp>
#include <futoshiki/Futoshiki.hpp>

// #include <futoshiki/utils/easylogging++.h>
#include <crow.h>

int main(int argc, const char * argv[]) {
    // START_EASYLOGGINGPP(argc, argv);
    // Load configuration from file
    // el::Configurations conf("/Users/MaxNoka/OneDrive/Projects/futoshiki/futoshiki/conf/logging.conf");
    // Reconfigure single logger
    // el::Loggers::reconfigureLogger("default", conf);
    // el::Loggers::setVerboseLevel(0);;
    
    crow::SimpleApp app;
    CROW_ROUTE(app, "/json")
    ([]{
        crow::json::wvalue x;
        x["message"] = "Hello, World!";
        return x;
    });
    
    app.port(18080).run();
    
    return 0;
}
