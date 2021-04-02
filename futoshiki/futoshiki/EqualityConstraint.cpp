//
//  EqualityConstraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 13/03/2021.
//

#include "EqualityConstraint.hpp"

#include "Cell.hpp"
#include "ConstraintSatisfactionProblem.hpp"

#include "utils/Utils.hpp"
#include "utils/easylogging++.h"

#include <set>
#include <iterator>
#include <utility>

namespace Csp {

namespace {

// 1. false if invalid (ended up removing all possible options from the cell)
// 2. the number of cells that we removed any values from
std::pair<bool, unsigned int> EliminateSelectivelyFromCells(const std::vector< std::weak_ptr<Cell> >& cellVec,
    const std::set<int>& toRemove,
    const std::set<int>& possibleValSetToIgnore) {
    
    unsigned int numCellRemovedFrom = 0;
    for (auto& p_Cell : cellVec) {
        if (p_Cell.lock()->GetPossibleValuesRef() != possibleValSetToIgnore) {
            auto [valid, removedSomething] = p_Cell.lock()->EliminateVals(toRemove);
            
            if (!valid) {
                return std::make_pair(false, 0);
            }
            numCellRemovedFrom += removedSomething;
        }
    }
    return std::make_pair(true, numCellRemovedFrom);
}

};

// bool: if the not equal condition turned out to be valid
bool EqualityConstraint::EvalMutuallyExclusiveNotEqualConditions() {
    // if we eliminate some variables, then we need to repeat
    // e.g. consider:
    //   four cells with (1, 2), (1, 2), (1, 2, 3), (1, 2, 3)
    //   we eliminate (1, 2) on the first pass, then we see
    //   invalid from the remaining (1, 2), (1, 2), (3), (3)
    bool eliminatedAny = false;
    do {
        // need to update this every time, as changes in the references in the container
        // do not get update in the multiset automatically
        m_availableValues.Update();
        auto& allPossibleValueCombinations = m_availableValues.m_container;
        
        // iterate only over the unique possible value combinations
        // TODO: use num with std::advance
        for (auto it_UniqueCombi = allPossibleValueCombinations.begin();
             it_UniqueCombi != allPossibleValueCombinations.end();
             it_UniqueCombi = allPossibleValueCombinations.upper_bound(*it_UniqueCombi)
        ) {
            // the number of cells with the same possible value combinations
            auto num = allPossibleValueCombinations.count(*it_UniqueCombi);
            
            // e.g. three cells with (1, 2). This isn't possible
            if ( num > it_UniqueCombi->m_ref.get().size() ) {
                return false;
            }
            // e.g. two cells with (1, 2) => coupled and no *other* cells can have (1, 2)
            else if ( num == it_UniqueCombi->m_ref.get().size() ) {
                // first eliminate the variable from the *other* cells,
                // e.g. (1, 2, 3) -> (3), but not (1, 2) -> ()
                // then do the process again
                auto [valid, numElimintatedFrom] = EliminateSelectivelyFromCells(m_cells, it_UniqueCombi->m_ref.get(), it_UniqueCombi->m_ref.get());
                if (!valid) {
                    return false;
                }
                eliminatedAny = numElimintatedFrom > 0;
                
                // no need to go again if all of the cells are solved already
                if (eliminatedAny) {
                    if (m_solved) { // set by the cells if they were set at all
                        return true;
                    }
                    break;
                }
            }
        }
    } while (eliminatedAny);
    return true;
}

EqualityConstraint::EqualityConstraint(
    const std::string& id,
    const std::vector< std::weak_ptr<Cell> >& cells,
    Operator op,
    ConstraintSatisfactionProblem* csp
)
    : Constraint(id, op, csp)
    , m_cells(cells)
    , m_availableValues()
{
    for (auto& pCell : m_cells) {
        m_availableValues.m_container.emplace( pCell.lock()->GetPossibleValuesRef() );
    }
    
    if(!SetSolvedIfPossible()) {
        m_csp->ReportIfConstraintBecomesActive();
    }
}

bool EqualityConstraint::SetSolvedIfPossible() {
    if (m_solved) {
        LOG(WARNING) << "Trying to set constraint to solved that was already solved";
    }
    
    for (auto& pCell : m_cells) {
        if (!pCell.lock()->IsSolved()) {
            return false;
        }
    }
    
    m_solved = true;
    m_csp->ReportIfConstraintNewlySolved();
    return true;
}


EqualityConstraint* EqualityConstraint::Clone(
    std::map< const Cell*,
    std::shared_ptr<Cell>* >& newCellLookup,
    ConstraintSatisfactionProblem* newCsp
) {
    std::vector< std::weak_ptr<Cell> > newCells;
    
    std::transform(m_cells.begin(), m_cells.end(), std::back_inserter(newCells),
        [&, newCellLookup, newCells](std::weak_ptr<Cell> oldCell) {
            return *newCellLookup.at( oldCell.lock().get() );
        }
    );
    
    auto clonedConstraint = new EqualityConstraint(
        Id(),
        newCells,
        m_operator,
        newCsp);
    
    return clonedConstraint;
}

std::string EqualityConstraint::dPrint(bool log) const {
    std::stringstream ss;
    ss << (m_solved ? "SOLVED" : "NOT SOLVED")
        << (m_relatedCellsChanged ? "* " : "  ");
    
    ss << m_id << ": ";

    for (auto it = m_cells.cbegin(); it != m_cells.end() - 1; ++it) {
        ss << it->lock()->dPrint(false) << " " << m_operator << " ";
    }
    ss << m_cells.back().lock()->dPrint(false);
    
    if (log) {
        VLOG(1) << ss.str();
    }
    
    return ss.str();
}

bool EqualityConstraint::Apply() {
    assertm(IsActive(), "should not be applying inactive constraint\n");
    
    bool constraintWasValid = true;
    switch (m_operator) {
        case Operator::NotEqualTo: {
            constraintWasValid = EvalMutuallyExclusiveNotEqualConditions();
            break;
        }
        case Operator::EqualTo: {
            assertm(false, "equal-to constraint not yet implemented");
            return false;
        }
        default:
            assertm(false, "invalid constraint operator for equality constraint");
            return false;
    }
    
    if (!constraintWasValid) {
        LOG(WARNING) << "Could not apply constraint, it was not valid";
    }
    
    m_relatedCellsChanged = false;
    
    if(!m_solved) { // transition to inactive not yet reported when the cell reported back to the constraint
        ReportBecameInactive();
    }
    
    return constraintWasValid;
}

bool EqualityConstraint::Valid() const {
    switch (m_operator) {
        case Operator::NotEqualTo:
            // TODO: probably a simple valid check will (it will be hard to keep track of eliminated values and all that without removing them from the cells)
            return true;
        case Operator::EqualTo:
            assertm(false, "equal-to constraint not yet implemented");
            return false;
        default:
            assertm(false, "invalid constraint operator for equality constraint");
            return false;
    }
}

std::vector<std::string> EqualityConstraint::GetCellIds() const {
    std::vector<std::string> outCellsIds(m_cells.size());
    std::transform(
        m_cells.begin(),
        m_cells.end(),
        outCellsIds.begin(),
        [](const std::weak_ptr<Cell>& pCell){
            return pCell.lock()->Id();
        }
    );
    return outCellsIds;
}

crow::json::wvalue EqualityConstraint::Serialize() const {
    auto out = crow::json::wvalue();   
    
    auto outCellsIds = GetCellIds();
    
    out["cells"] = outCellsIds;
    
    out["constraint_id"] = m_id;
    
    std::stringstream ss;
    ss << m_operator;
    out["operator"] = ss.str();
    
    out["type"] = "equality";

    return out;
}


} // ::Csp
