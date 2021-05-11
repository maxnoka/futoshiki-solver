//
//  main.cpp
//  MainFutoshiki (used for development adn testing)
//
//  Created by Maximilian Noka on 03/04/2021.
//

#include <futoshiki/Futoshiki.hpp>
#include <futoshiki/utils/easylogging++.h>

int main(int argc, const char * argv[]) {
    START_EASYLOGGINGPP(argc, argv);
    // Load configuration from file
    el::Configurations conf("/Users/MaxNoka/OneDrive/Projects/futoshiki/futoshiki/conf/logging.conf");
    // Reconfigure single logger
    el::Loggers::reconfigureLogger("default", conf);
    el::Loggers::setVerboseLevel(0);
    
    return 0;
}
