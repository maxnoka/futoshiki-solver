//
//  EasyLoggingCrowLogHandler.hpp
//  WebFutoshiki
//
//  Created by Maximilian Noka on 11/05/2021.
//

#ifndef EasyLoggingCrowLogHandler_hpp
#define EasyLoggingCrowLogHandler_hpp

#include <crow/logging.h>

class EasyLoggingCrowLogHandler : public crow::ILogHandler {
public:
    void log(std::string message, crow::LogLevel level) override;
};

#endif /* EasyLoggingCrowLogHandler_hpp */
