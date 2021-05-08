//
//  main.cpp
//  MainFutoshiki
//
//  Created by Maximilian Noka on 03/04/2021.
//

#include <futoshiki/CspSolver.hpp>
#include <futoshiki/Futoshiki.hpp>
#include <futoshiki/CspBuilder.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wcomma"
#include <crow.h>
#pragma clang diagnostic pop

#include <futoshiki/utils/easylogging++.h>

namespace {
    constexpr auto kMaxPuzzleSizeGenerate = 6;
}

class EasyLoggingCrowLogHandler : public crow::ILogHandler {
public:
    void log(std::string message, crow::LogLevel level) override {
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
};

int main(int argc, const char * argv[]) {
     START_EASYLOGGINGPP(argc, argv);
     el::Configurations conf("/Users/MaxNoka/OneDrive/Projects/futoshiki/futoshiki/conf/logging.conf");
     el::Loggers::reconfigureLogger("default", conf);
     el::Loggers::setVerboseLevel(0);;
    
    auto logHandler = std::make_unique<EasyLoggingCrowLogHandler>();
    crow::logger::setHandler(logHandler.get());
    crow::SimpleApp app;
    
    CROW_ROUTE(app, "/generate")
    .methods("POST"_method)
    ([](const crow::request& req)
    {
        auto in = crow::json::load(req.body);
        if (!in || !in.has("size") || !in.has("type") ) {
            return crow::response(400);
        }
        if (in["size"].t() != crow::json::type::Number) {
            return crow::response(400);
        }
        if (in["type"].t() != crow::json::type::String) {
            return crow::response(400);
        }
        
        auto puzzleTypeString = in["type"].s();
        if (puzzleTypeString != "futoshiki") {
            return crow::response(400);
        }
        auto size = in["size"].i();
        if (size < 2 || size > kMaxPuzzleSizeGenerate) {
            return crow::response(400);
        }
        
        auto generatedCsp = Csp::Futoshiki::Generate(size);
        return crow::response{ generatedCsp.Serialize() };
    });
    
    CROW_ROUTE(app, "/solve")
    .methods("POST"_method)
    ([](const crow::request& req)
    {
        auto in = crow::json::load(req.body);
        if (   !in
            || !in.has("grid_size")
            || !in.has("num_cells")
            || !in.has("cells")
            || !in.has("constraints")
        ) {
            return crow::response(400);
        }
        if (in["grid_size"].t() != crow::json::type::Number) {
            return crow::response(400);
        }
        auto gridSize = in["grid_size"].i();
        
        if (in["num_cells"].t() != crow::json::type::Number) {
            return crow::response(400);
        }
        auto numCells = in["num_cells"].i();
        
        if (in["cells"].t() != crow::json::type::List) {
            return crow::response(400);
        }
        auto rows = in["cells"];
        
        if (in["constraints"].t() != crow::json::type::List) {
            return crow::response(400);
        }
        auto constraints = in["constraints"];
        
        if (rows.size() != gridSize // numRows
            || (gridSize * gridSize) != numCells
        ) {
            return crow::response(400);
        }
        
        try {
            auto csp = Csp::MakeFutoshikiFromJson(rows, constraints);  // can throw
            auto solver = Csp::CspSolver<Csp::Futoshiki>(std::move(csp));
            auto res = solver.SolveUnique();
            
            return crow::response {res.ToJson()};
        }
        catch (...) {
            return crow::response(400);
        }
    });
    
    app.port(18080).run();
    
    return 0;
}
