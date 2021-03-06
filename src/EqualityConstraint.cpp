//
//  EqualityConstraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 13/03/2021.
//

#include <futoshiki/EqualityConstraint.hpp>

#include <futoshiki/Cell.hpp>
#include <futoshiki/ConstraintSatisfactionProblem.hpp>

#include <futoshiki/utils/Utils.hpp>
#include <futoshiki/utils/easylogging++.h>

#include <set>
#include <iterator>
#include <utility>

namespace Csp {

namespace {

bool HaveDuplicateVals(const std::vector< std::weak_ptr<Cell> >& cellVec) {
    std::set<int> cellVals;
    for (auto cell : cellVec) {
        auto [it, didInsertion] = cellVals.insert(cell.lock()->Value());
        if (!didInsertion) {
            return true;
        }
    }
    return false;
}

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

bool EqualityConstraint::EvalOnlyOptions() {
    std::multiset<int> valueCounts;
    for (auto& combination : m_availableValues) {
        for (auto value : *combination) {
            valueCounts.insert(value);
        }
    }
    for (auto value : valueCounts) {
        if (valueCounts.count(value) == 1) {
            for (auto& cell : m_cells) {
                if (cell.lock()->IsSolved()) {
                    continue;
                }
                if (cell.lock()->GetPossibleValuesRef().count(value)) {
                    cell.lock()->SetVal(value);
                }
            }
        }
    }
    
    return Valid();
}

// bool: if the not equal condition turned out to be valid
bool EqualityConstraint::EvalMutuallyExclusiveNotEqualConditions() {
    // if we eliminate some variables, then we need to repeat
    // e.g. consider:
    //   four cells with (1, 2), (1, 2), (1, 2, 3), (1, 2, 3)
    //   we eliminate (1, 2) on the first pass, then we see
    //   invalid from the remaining (1, 2), (1, 2), (3), (3)
    bool eliminatedAny = false;
    do {
        // iterate only over the unique possible value combinations
        // TODO: use num with std::advance
        for (auto it_UniqueCombi = m_availableValues.begin();
             it_UniqueCombi != m_availableValues.end();
             it_UniqueCombi = m_availableValues.upper_bound(*it_UniqueCombi)
        ) {
            // the number of cells with the same possible value combinations
            // TODO: this doesn't work for some reason on some architectures / compilers ???
            // auto num = m_availableValues.count(*it_UniqueCombi);
            size_t num = 0;
            for (auto pSet : m_availableValues) {
                num += (**it_UniqueCombi == *pSet);
            }
            
            // e.g. three cells with (1, 2). This isn't possible
            if ( num > (*it_UniqueCombi)->size() ) {
                return false;
            }
            // e.g. two cells with (1, 2) => coupled and no *other* cells can have (1, 2)
            else if ( num == (*it_UniqueCombi)->size() ) {
                // first eliminate the variable from the *other* cells,
                // e.g. (1, 2, 3) -> (3), but not (1, 2) -> ()
                // then do the process again
                auto [valid, numElimintatedFrom] = EliminateSelectivelyFromCells(m_cells, **it_UniqueCombi, **it_UniqueCombi);
                if (!valid) {
                    return false;
                }
                eliminatedAny = numElimintatedFrom > 0;
                
                // no need to go again if all of the cells are solved already
                if (eliminatedAny) {
                    break;
                }
            }
        }
    } while (eliminatedAny);
    
    return Valid();
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
        m_availableValues.emplace( pCell.lock()->GetPossibleValuesPointer() );
    }
    
    if(!Valid()) {
        m_provenInvalid = true;
    }
    if(!SetSolvedIfPossible()) {
        m_csp->ReportIfConstraintBecomesActive();
    }
}

EqualityConstraint::EqualityConstraint(
    const EqualityConstraint& other,
    const std::vector< std::weak_ptr<Cell> >& newCells,
    ConstraintSatisfactionProblem* newCsp
)
    : Constraint(other, newCsp)
    , m_cells(newCells)
    , m_availableValues()
{
    for (auto& pCell : m_cells) {
        m_availableValues.emplace( pCell.lock()->GetPossibleValuesPointer() );
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
        *this,
        newCells,
        newCsp
    );
    
    return clonedConstraint;
}

std::string EqualityConstraint::dPrint(bool log) const {
    std::stringstream ss;
    
    ss << Constraint::dPrint(false);

    for (auto it = m_cells.cbegin(); it != m_cells.end() - 1; ++it) {
        ss << it->lock()->dPrint(false) << " " << m_operator << " ";
    }
    ss << m_cells.back().lock()->dPrint(false);
    
    if (log) {
        VLOG(2) << ss.str();
    }
    
    return ss.str();
}

bool EqualityConstraint::Apply() {
    assertm(IsActive(), "should not be applying inactive constraint");
    assertm(!m_provenInvalid, "Should not try to apply constraints that are already proven invalid");
    
    bool constraintWasValid = true;
    switch (m_operator) {
        case Operator::NotEqualTo: {
            constraintWasValid = EvalMutuallyExclusiveNotEqualConditions();
            // TODO: only do this if the prerequisite conditions are met
            constraintWasValid = constraintWasValid && EvalOnlyOptions();
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
        m_provenInvalid = true;
        VLOG(2) << "Could not apply constraint, it was not valid";
    }
    
    m_relatedCellsChanged = false;
    
    if(!m_solved) { // transition to inactive not yet reported when the cell reported back to the constraint
        ReportBecameInactive();
    }
    
    return constraintWasValid;
}

bool EqualityConstraint::Valid() {
    switch (m_operator) {
        case Operator::NotEqualTo:
            if (!m_solved) {
                // TODO: this needs to be implemented, but won't be simple
                // assertm(false, "not-equal-to validity check not yet implemented when there are unsolved cells");
            }
            else if (HaveDuplicateVals(m_cells)) {
                m_provenInvalid = true;
            }
            break;
        case Operator::EqualTo:
            assertm(false, "equal-to constraint not yet implemented");
            break;
        default:
            assertm(false, "invalid constraint operator for equality constraint");
            break;
    }
    return !m_provenInvalid;
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
