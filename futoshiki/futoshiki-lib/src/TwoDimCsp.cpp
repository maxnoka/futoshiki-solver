//
//  TwoDimCsp.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 26/03/2021.
//

#include <futoshiki/TwoDimCsp.hpp>

#include <futoshiki/utils/Utils.hpp>
#include <futoshiki/utils/easylogging++.h>

#include <sstream>
#include <iterator>

namespace Csp {

namespace {

std::vector< std::pair<std::string, int> > GenIdValuePairs(const std::vector< std::vector<int> >& initValues) {
    std::vector<int> flattened = Utils::FlattenVector2d(initValues);

    std::vector< std::pair<std::string, int> > out( flattened.size() );

    for (auto [cellIdx, initValue] : Utils::enumerate(flattened)) {
        std::stringstream ss;
        ss << ( cellIdx % initValues[0].size() ) << "_" << ( cellIdx / initValues[0].size() );
        
        out[cellIdx] = std::make_pair(ss.str(), initValue);
    }
    
    return out;
}

std::vector< std::pair<std::string, int> > GenUnsetBoard(unsigned long numCols, unsigned long numRows) {
    std::vector<int> flattened(numCols * numRows, Cell::kUnsolvedSymbol);

    std::vector< std::pair<std::string, int> > out( flattened.size() );
    for (auto [cellIdx, initValue] : Utils::enumerate(flattened)) {
        std::stringstream ss;
        ss << ( cellIdx % numCols ) << "_" << ( cellIdx / numCols );
        
        out[cellIdx] = std::make_pair(ss.str(), initValue);
    }
    
    return out;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
std::string CoordsToId(const TwoDimCsp::CellCoords& cords) {
    std::stringstream ss;
    ss <<  cords.first << "_" << cords.second;
    return ss.str();
}
#pragma clang diagnostic pop

TwoDimCsp::CellCoords IdToCoords(const std::string& id) {
    std::size_t underScore = id.find_first_of("_");
    
    assertm(underScore != std::string::npos, "underscore not found in it");
    
    TwoDimCsp::CellCoords out;
    out.first = std::stoll(id.substr(0, underScore));
    out.second = std::stoll(id.substr(underScore + 1));
    
    return out;
}

enum class Direction {
    Up,
    Down,
    Left,
    Right,
    Complex
};

constexpr unsigned int switch_pair(unsigned int i, unsigned int j) {
   return (i << 16) + j;
}

Direction CoordsRelativeDirection (
    const TwoDimCsp::CellCoords a,
    const TwoDimCsp::CellCoords b) {
    
    // Left means: "a is to the immediate left of b"
    
    auto diffX = a.first - b.first;
    auto diffY = a.second - b.second;
    
    switch (switch_pair(
        static_cast<unsigned int>(diffX),
        static_cast<unsigned int>(diffY)
    )) {
        case switch_pair(0, -1):
            return Direction::Down;
        case switch_pair(0, 1):
            return Direction::Up;
        case switch_pair(-1, 0):
            return Direction::Right;
        case switch_pair(1, 0):
            return Direction::Left;
        default:
            return Direction::Complex;
    }
}


}

TwoDimCsp::TwoDimCsp(
    const std::vector< std::vector<int> >& initValues,
    const std::set<int>& defaultPossibleValues
)
    : ConstraintSatisfactionProblem(
        GenIdValuePairs(initValues),
        defaultPossibleValues)
    , m_numRows(initValues.size())
    , m_numCols(initValues[0].size())
{
    if (!Utils::Vector2dIsRectangular(initValues)) {
        throw std::invalid_argument("require rectangular cell vector for two dimensional CSP");
    }
}

TwoDimCsp::TwoDimCsp(
    unsigned long columns,
    unsigned long rows,
    const std::set<int>& defaultPossibleValues
)
    : ConstraintSatisfactionProblem(
        GenUnsetBoard(columns, rows),
        defaultPossibleValues)
    , m_numRows(rows)
    , m_numCols(columns)
{ }

bool TwoDimCsp::AddInequalityConstraint(
    const CellCoords& lhsCellCoords,
    Constraint::Operator op,
    const CellCoords& rhsCellCoords
) {
    if (!ValidCoords(lhsCellCoords) || !ValidCoords(rhsCellCoords)) {
        return false;
    }
    
    return ConstraintSatisfactionProblem::AddInequalityConstraint(
        CoordsToIndex(lhsCellCoords),
        op,
        CoordsToIndex(rhsCellCoords)
    );
}

bool TwoDimCsp::AddEqualityConstraint(
    const std::vector<CellCoords>& cellCoords,
    Constraint::Operator op
) {
    for (auto& coord : cellCoords) {
        if (!ValidCoords(coord)) {
            return false;
        }
    }
    
    std::vector<unsigned long> cellIndeces(cellCoords.size());
    std::transform(
        cellCoords.cbegin(),
        cellCoords.cend(),
        cellIndeces.begin(),
        [this](const CellCoords& coord){
            return this->CoordsToIndex(coord);
        });
    
    return ConstraintSatisfactionProblem::AddEqualityConstraint(cellIndeces, op);
}

void TwoDimCsp::dPrint(bool printCells) const {
    dPrintGrid();
    ConstraintSatisfactionProblem::dPrint(printCells);
}

void TwoDimCsp::dPrintGrid() const {
    auto grid = GetGrid();
    
    std::stringstream lineSpacer;
    for (unsigned long i = 0; i < grid.front().size(); ++i) {
        lineSpacer << "+-+ ";
    }
    
    std::stringstream emptyLine;
    for (unsigned long i = 0; i < grid.front().size(); ++i) {
        emptyLine << "    ";
    }
    
    std::vector<std::string> outStringGrid;
    for (auto& row : grid) {
        outStringGrid.push_back(lineSpacer.str());
        
        std::stringstream ss;
        for (auto pCell : row) {
            ss << "|" << pCell.lock()->Value() << "| ";
        }
        outStringGrid.push_back(ss.str());
        
        outStringGrid.push_back(lineSpacer.str());
        outStringGrid.push_back(emptyLine.str());
    }
    
    for (auto pConstraint : m_constraints) {
        auto op = pConstraint->GetOperator();
        bool showConstraint =
            op == Constraint::Operator::GreaterThan ||
            op == Constraint::Operator::LessThan;
        if (!showConstraint) {
            continue;
        }
        
        auto cellIds = pConstraint->GetCellIds();
        auto lhsCords = IdToCoords(cellIds[0]);
        auto rhsCords = IdToCoords(cellIds[1]);
        
        unsigned long rowIdx = 0;
        unsigned long colIdx = 0;
        bool flipDirection = false;
        switch( CoordsRelativeDirection(lhsCords, rhsCords) ) {
            case Direction::Down:
                rowIdx = lhsCords.second*4 + 3;
                colIdx = lhsCords.first*4 + 1;
                break;
            case Direction::Up:
                rowIdx = lhsCords.second*4 - 1;
                colIdx = lhsCords.first*4 + 1;
                flipDirection = true;
                break;
            case Direction::Left:
                rowIdx = lhsCords.second*4 + 1;
                colIdx = lhsCords.first*4 - 1;
                flipDirection = true;
                break;
            case Direction::Right:
                rowIdx = lhsCords.second*4 + 1;
                colIdx = lhsCords.first*4 + 3;
                break;
            default:
                continue;
        };
        
        std::stringstream ss;
        ss  << (flipDirection
                    ? Constraint::ReverseOperator(pConstraint->GetOperator())
                    : pConstraint->GetOperator() );
        outStringGrid[rowIdx].replace(colIdx, 1, ss.str());
    }
    
    std::stringstream ss;
    ss <<  "Board\n";
    for (std::string row : outStringGrid) {
        ss << row << "\n";
    }
    LOG(INFO) << ss.str();
}

bool TwoDimCsp::ValidCoords(const CellCoords& cords) {
    if (cords.first >= m_numCols) {
        return false;
    }
    
    if (cords.second >= m_numRows) {
        return false;
    }
    
    return true;
}

unsigned long TwoDimCsp::CoordsToIndex(const CellCoords& cords
) {
    assertm(ValidCoords(cords), "coordinates are not valid");
    return cords.second * m_numCols + cords.first;
}

std::vector< std::vector< std::weak_ptr<Cell> > > TwoDimCsp::GetGrid() const {
    std::vector< std::vector< std::weak_ptr<Cell> > > out(m_numRows);
    
    for (unsigned long rowIdx = 0; rowIdx < m_numRows; ++rowIdx) {
        out[rowIdx] = GetRow(rowIdx);
    }

    return out;
}

bool TwoDimCsp::AddEqualityConstraintToRow(
    unsigned long rowIdx,
    Constraint::Operator op
) {
    auto row = GetRow(rowIdx);
    
    std::stringstream ss;
    ss << "row" << rowIdx;
    
    return ConstraintSatisfactionProblem::AddEqualityConstraint( row, op, ss.str() );
}

bool TwoDimCsp::AddEqualityConstraintToCol(
    unsigned long colIdx,
    Constraint::Operator op
) {
    auto col = GetCol(colIdx);

    std::stringstream ss;
    ss << "col" << colIdx;
    
    return ConstraintSatisfactionProblem::AddEqualityConstraint( col, op, ss.str() );
}

bool TwoDimCsp::AddEqualityConstraintToAllRows(
    Constraint::Operator op
) {
    bool res = true;
    for (unsigned long rowIdx = 0; rowIdx < m_numRows; ++rowIdx) {
        res &= AddEqualityConstraintToRow(rowIdx, op);
    }
    return res;
}

bool TwoDimCsp::AddEqualityConstraintToAllCols(
    Constraint::Operator op
) {
    bool res = true;
    for (unsigned long colIdx = 0; colIdx < m_numCols; ++colIdx) {
        res &= AddEqualityConstraintToCol(colIdx, op);
    }
    return res;
}

bool TwoDimCsp::AddEqualityConstraintToAllRowsAndCols(
    Constraint::Operator op
) {
    bool res = AddEqualityConstraintToAllRows(op);
    res &= AddEqualityConstraintToAllCols(op);
    return res;
}

std::vector< std::weak_ptr<Cell> > TwoDimCsp::GetRow(unsigned long rowIdx) const {
    std::vector< std::weak_ptr<Cell> > out;
    
    auto itBegin = m_cells.lower_bound(m_numCols * rowIdx);
    auto itEnd = m_cells.upper_bound(m_numCols * rowIdx + m_numCols - 1);
    
    for (auto it = itBegin; it != itEnd; ++it) {
        out.push_back(it->second);
    }
    
    return out;
}

std::vector< std::weak_ptr<Cell> > TwoDimCsp::GetCol(unsigned long colIdx) const {
    std::vector< std::weak_ptr<Cell> > out;
    
    auto it = m_cells.begin();
    std::advance(it, colIdx);
    
    out.push_back(it->second);
    for (auto counter = 0; counter < m_numRows - 1; ++counter) {;
        std::advance(it, m_numCols);
        out.push_back(it->second);
    }
    
    return out;
}

crow::json::wvalue TwoDimCsp::Serialize() const {
    auto out = crow::json::wvalue();
    
    out["num_cells"] = m_cells.size();
    
    crow::json::wvalue gridSize;
    gridSize["num_rows"] = m_numRows;
    gridSize["num_cols"] = m_numCols;
    
    out["grid_size"] = std::move(gridSize);
    
    out["cells"] = SerializeGrid(); // different compared to CSP
    out["constraints"] = SerializeConstraints();
    
    return out;
}

crow::json::wvalue TwoDimCsp::SerializeGrid() const {
    auto grid = GetGrid();
    crow::json::wvalue outGrid;
    ;
    auto serializeCellVector = [](const std::vector< std::weak_ptr<Cell> >& vec){
        std::vector<crow::json::wvalue> rowOut( vec.size() );
        std::transform(
            vec.cbegin(),
            vec.cend(),
            rowOut.begin(),
            [](const std::weak_ptr<Cell>& cell) {
                return cell.lock()->Serialize();
            }
        );
        return rowOut;
    };
    
    for (unsigned int i = 0; i < grid.size(); ++i) {
        outGrid[i] = serializeCellVector(grid[i]);
    }
    
    return outGrid;
}

} // ::Csp
