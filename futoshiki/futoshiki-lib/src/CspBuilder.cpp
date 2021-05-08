//
//  CspBuilder.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 07/05/2021.
//

#include <futoshiki/CspBuilder.hpp>

namespace Csp {

Futoshiki MakeFutoshikiFromJson(const crow::json::rvalue& cellsJson, const crow::json::rvalue& constraintsJson) {
    if (cellsJson.t() != crow::json::type::List) {
        throw std::runtime_error("invalid json");
    }
    auto gridSize = cellsJson.size();
    
    std::vector<std::vector<Cell>> cells(gridSize);
    std::transform(
        cellsJson.begin(),
        cellsJson.end(),
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
    
    Futoshiki out{ std::move(cells) };
    if (!AddConstraintsFromJson(constraintsJson, &out)) {
        throw std::runtime_error("could not add constraints");
    }
    return out;
}

bool AddConstraintFromJson(const crow::json::rvalue& constraintJson, ConstraintSatisfactionProblem* pCsp) {
    if (constraintJson.t() != crow::json::type::Object) {
        return false;
    }
    
    if (   !constraintJson.has("operator")
        || !constraintJson.has("type")
        || !constraintJson.has("constraint_id")
        || !constraintJson.has("cells")
    ) {
        return false;
    }
    
    if (constraintJson["operator"].t() != crow::json::type::String) {
        return false;
    }
    auto opString = constraintJson["operator"].s();
    if (constraintJson["type"].t() != crow::json::type::String) {
        return false;
    }
    auto typeString = constraintJson["type"].s();
    if (constraintJson["constraint_id"].t() != crow::json::type::String) {
        return false;
    }
    if (constraintJson["cells"].t() != crow::json::type::List) {
        return false;
    }
    auto cellIds = constraintJson["cells"];
    
    for (auto& cellId : cellIds) {
        if (cellId.t() != crow::json::type::String) {
            return false;
        }
    }
    
    Constraint::Operator op;
    bool isInequalityConstraint;
    if (typeString == "equality") {
        isInequalityConstraint = true;
        if (opString == "=") {
           op = Constraint::Operator::EqualTo;
        }
        else if (opString == "!=") {
            op = Constraint::Operator::NotEqualTo;
        }
        else {
            return false;
        }
        
        if (cellIds.size() < 2) {
            return false;
        }
        
        std::vector<unsigned long> cellIdxs(cellIds.size());
        try {
            std::transform(cellIds.begin(), cellIds.end(), cellIdxs.begin(),
                [pCsp](const crow::json::rvalue& json) {
                    return pCsp->FindCellIdx(json.s());
                }
            );
        }
        catch (...) {
            return false;
        }
        return pCsp->AddEqualityConstraint(cellIdxs, op);
    }
    else if (typeString == "inequality") {
        isInequalityConstraint = false;
        if (opString == "<") {
            op = Constraint::Operator::LessThan;
        }
        else if (opString == ">") {
            op = Constraint::Operator::GreaterThan;
        }
        else {
            return false;
        }
        
        if (cellIds.size() != 2) {
            return false;
        }
        
        unsigned long lhsIdx;
        unsigned long rhsIdx;
        try {
            lhsIdx = pCsp->FindCellIdx(cellIds[0].s());
            rhsIdx = pCsp->FindCellIdx(cellIds[1].s());
        }
        catch (...) {
            return false;
        }
        return pCsp->AddInequalityConstraint(lhsIdx, op, rhsIdx);
    }
    else {
        return false;
    }
}

bool AddConstraintsFromJson(const crow::json::rvalue& constraintsJson, ConstraintSatisfactionProblem* pCsp) {
    if (constraintsJson.t() != crow::json::type::List) {
        return false;
    }
    
    for (auto& constraintJson : constraintsJson) {
        if (!AddConstraintFromJson(constraintJson, pCsp)) {
            return false;
        }
    }
    return true;
}

}
