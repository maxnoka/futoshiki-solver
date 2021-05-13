//
//  EasyLoggingCrowLogHandler.cpp
//  WebFutoshiki
//
//  Created by Maximilian Noka on 11/05/2021.
//

#include "EasyLoggingCrowLogHandler.hpp"

#include <futoshiki/utils/easylogging++.h>

void EasyLoggingCrowLogHandler::log(std::string message, crow::LogLevel level) {
    static constexpr auto kCrowLogPrefixLength = 33;
    message = message.substr(kCrowLogPrefixLength);
    switch (level) {
        case crow::LogLevel::Debug:
            LOG(DEBUG) << message;
            return;
        case crow::LogLevel::Info:
            LOG(INFO) << message;
            return;
        case crow::LogLevel::Warning:
            LOG(WARNING) << message;
            return;
        case crow::LogLevel::Error:
            LOG(ERROR) << message;
            return;
        case crow::LogLevel::Critical:
            LOG(FATAL) << message;
            return;
    }
}
