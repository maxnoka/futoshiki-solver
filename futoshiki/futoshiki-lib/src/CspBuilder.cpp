//
//  CspBuilder.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 07/05/2021.
//

#include <futoshiki/CspBuilder.hpp>

namespace Csp {

Futoshiki MakeFutoshikiFromJson(const crow::json::rvalue& json) {
    if (json.t() != crow::json::type::List) {
        throw std::runtime_error("invalid json");
    }
    auto gridSize = json.size();
    
    std::vector<std::vector<Cell>> cells(gridSize);
    
    std::transform(
        json.begin(),
        json.end(),
        cells.begin(),
        [gridSize](const crow::json::rvalue& row){
            if (row.t() != crow::json::type::List) {
                throw std::runtime_error("invalid json");
            }
            if (row.size() != gridSize) {
                throw std::runtime_error("invalid json");
            }
            
            std::vector<Cell> cellRow;
            std::transform(
                row.begin(),
                row.end(),
                std::back_inserter(cellRow),
                [](const crow::json::rvalue& cellJson) {
                    return Cell{cellJson};
                }
            );
            return cellRow;
        }
    );
    
    return Futoshiki(std::move(cells));
}

}
