//
//  ConstraintSatisfactionProblem.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include <futoshiki/ConstraintSatisfactionProblem.hpp>

#include <futoshiki/Constraint.hpp>
#include <futoshiki/Cell.hpp>

#include <futoshiki/utils/Utils.hpp>

#include <futoshiki/utils/easylogging++.h>

#include <iostream>
#include <algorithm>
#include <utility>
#include <map>
#include <cassert>
#include <sstream>

namespace Csp {

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(
    const std::vector<int>& initValues,
    const std::set<int>& defaultPossibleValues
)
    : m_numSolvedCells(0) // reported back to us when we construct the cells
    , m_numSolvedConstraints(0)
    , m_numActiveConstraints(0)
    , m_provenValid(false)
    , m_completelySolved(false)
    , m_numCells(initValues.size())
    , m_defaultPossibleValues(defaultPossibleValues)
    , m_cells()
    , m_constraints()
{
    for (auto [cellIdx, initValue] : Utils::enumerate(initValues)) {
        m_cells.emplace(
            std::make_pair(
                cellIdx,
                std::make_shared<Cell>(
                    initValue,
                    std::to_string(cellIdx),
                    m_defaultPossibleValues,
                    this
                )
            )
        );
    }
}

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(
    const std::vector< std::pair<std::string, int> >& initValues,
    const std::set<int>& defaultPossibleValues
)
    : m_numSolvedCells(0) // reported back to us when we construct the cells
    , m_numSolvedConstraints(0)
    , m_numActiveConstraints(0)
    , m_provenValid(false)
    , m_completelySolved(false)
    , m_numCells(initValues.size())
    , m_defaultPossibleValues(defaultPossibleValues)
    , m_cells()
    , m_constraints()
{
    for (auto [cellIdx, initValue] : Utils::enumerate(initValues)) {
        m_cells.emplace(
            std::make_pair(
                cellIdx,
                std::make_shared<Cell>(
                    initValue.second,
                    initValue.first,
                    m_defaultPossibleValues,
                    this
                )
            )
        );
    }
}

ConstraintSatisfactionProblem::ConstraintSatisfactionProblem(const ConstraintSatisfactionProblem& other)
    : m_numSolvedCells(other.m_numSolvedCells)
    , m_numSolvedConstraints(other.m_numSolvedConstraints)
    , m_numActiveConstraints(other.m_numActiveConstraints)
    , m_provenValid(other.m_provenValid)
    , m_completelySolved(other.m_completelySolved)
    , m_numCells(other.m_numCells)
    , m_defaultPossibleValues(other.m_defaultPossibleValues)
    , m_cells()
    , m_constraints()
{
    // shallow copy all the cells to begin with
    for (auto& cell : other.m_cells) {
        m_cells.emplace( std::make_pair(cell.first, std::make_shared<Cell>(*cell.second.get(), this)) );
    }
    
    // Create the old to new cell lookup vectors
    std::vector<const Cell*> oldCellKeys(other.m_cells.size());
    std::transform(
        other.m_cells.cbegin(),
        other.m_cells.cend(),
        oldCellKeys.begin(),
        [](const std::pair< const unsigned long, std::shared_ptr<Cell> >& pair) -> const Cell* {
            return pair.second.get();
        }
    );
    std::vector< std::shared_ptr<Cell>* > newCellValues(m_cells.size());
    std::transform(
        m_cells.begin(),
        m_cells.end(),
        newCellValues.begin(),[](std::pair< const unsigned long, std::shared_ptr<Cell> >& pair) -> std::shared_ptr<Cell>* { return &pair.second; }
    );
    // for both the below:
    // key: old
    // value: new
    std::map<const Cell*, std::shared_ptr<Cell>* > newCellLookup =
        Utils::ZipVectorsToMap(oldCellKeys, newCellValues);
    
    // Deep copy the constraints: updates the cell pointers to the new cells with the lookup map
    for (const auto& otherConstraint : other.m_constraints) {
        m_constraints.emplace_back(otherConstraint->Clone(newCellLookup, this));
    }
    
    // Create the old to new constraint lookup vectors
    std::vector<const Constraint*> oldConstraintKeys(other.m_constraints.size());
    std::transform(
        other.m_constraints.cbegin(),
        other.m_constraints.cend(),
        oldConstraintKeys.begin(),[](const std::shared_ptr<Constraint>& c) -> const Constraint* { return c.get(); }
    );
    std::vector<std::shared_ptr<Constraint>*> newConstraintValues(m_constraints.size());
    std::transform(
        m_constraints.begin(),
        m_constraints.end(),
            newConstraintValues.begin(),[](std::shared_ptr<Constraint>& c) -> std::shared_ptr<Constraint>* { return &c; }
    );
    std::map< const Constraint*, std::shared_ptr<Constraint>* > newConstraintLookup =
        Utils::ZipVectorsToMap(oldConstraintKeys, newConstraintValues);
    
    // update the constraint pointers in the new cells
    for (const auto& cell : m_cells) {
        cell.second->UpdateConstraintPointers(newConstraintLookup);
    }
}

ConstraintSatisfactionProblem& ConstraintSatisfactionProblem::operator =(const ConstraintSatisfactionProblem& other) {
    if (this == &other) {
        return *this;
    }
    
    auto tmp = ConstraintSatisfactionProblem(other);
    
    std::swap(m_defaultPossibleValues, tmp.m_defaultPossibleValues);
    std::swap(m_cells, tmp.m_cells);
    std::swap(m_constraints, tmp.m_constraints);
    std::swap(m_numSolvedCells, tmp.m_numSolvedCells);
    std::swap(m_numSolvedConstraints, tmp.m_numSolvedConstraints);
    std::swap(m_numActiveConstraints, tmp.m_numActiveConstraints);
    std::swap(m_provenValid, tmp.m_provenValid);
    std::swap(m_completelySolved, tmp.m_completelySolved);
    std::swap(m_numCells, tmp.m_numCells);
    
    return *this;
}

void ConstraintSatisfactionProblem::dPrint(bool printCells) const {
    std::stringstream ss;
    ss << "CSP\n";

    if (printCells) {
        for (auto [cellIdx, cell] : m_cells) {
            ss << cell->dPrint(false) << "\n";
        }
    }
    
    for (auto constraint : m_constraints) {
        ss << constraint->dPrint(false) << "\n";
    }
    VLOG(1) << ss.str();
}

bool ConstraintSatisfactionProblem::AddInequalityConstraint(
    unsigned long lhsCellIdx,
    Constraint::Operator op,
    unsigned long rhsCellIdx,
    const std::string& idPrefix
) {
    bool lhsValid = lhsCellIdx < m_cells.size();
    bool rhsValid = rhsCellIdx < m_cells.size();
    
    if (!lhsValid || !rhsValid) {
        LOG(ERROR) << "Invalid cell indeces (out of range). Cannot add inequality constraint.";
        return false;
    }
    
    if (lhsCellIdx == rhsCellIdx) {
        LOG(ERROR) << "Invalid cell indeces (equal). Cannot add inequality constraint.";
        return false;
    }
    
    auto& lhsCell = m_cells.at(lhsCellIdx);
    auto& rhsCell = m_cells.at(rhsCellIdx);
    
    std::stringstream ss;
    ss << idPrefix << "_" << m_constraints.size();
    
    auto constraint = std::make_shared<InequalityConstraint>(ss.str(), lhsCell, op, rhsCell, this);
    
    if (!constraint->Valid()) {
        LOG(ERROR) << "Tried to add invalid constraint. Cannot add inequality constraint.";
        return false;
    }
    
    m_constraints.push_back(
        std::static_pointer_cast<Constraint>(
            std::move(constraint)
        )
    );
    
    lhsCell->AddConstraint(m_constraints.back());
    rhsCell->AddConstraint(m_constraints.back());
    
    return true;
}

bool ConstraintSatisfactionProblem::AddEqualityConstraint(
    const std::vector<unsigned long>& cellIndeces,
    Constraint::Operator op,
    const std::string& idPrefix
) {
    for (auto cellIndex : cellIndeces) {
        if ( cellIndex >= m_cells.size() ) {
            LOG(ERROR) << "Invalid cell indeces (out of range). Cannot add equality constraint.";
            return false;
        }
    }
    
    if (!Utils::isUnique(cellIndeces)) {
        LOG(ERROR) << "Invalid cell indeces (equal). Cannot add equality constraint.";
        return false;
    }
    
    std::vector< std::weak_ptr<Cell> > cellPointers;
    std::transform(cellIndeces.cbegin(), cellIndeces.cend(), std::back_inserter(cellPointers),
        [this](unsigned long cellIndex) {
            return m_cells.at(cellIndex);
        }
    );
    
    return AddEqualityConstraint(cellPointers, op, idPrefix);
}

bool ConstraintSatisfactionProblem::AddEqualityConstraint(
    const std::vector< std::weak_ptr<Cell> >& cellPointers,
    Constraint::Operator op,
    const std::string& idPrefix
) {
    std::stringstream ss;
    ss << idPrefix << "_" << m_constraints.size();
    
    auto constraint = std::make_shared<EqualityConstraint>(ss.str(), cellPointers, op, this);
    
    if (!constraint->Valid()) {
        LOG(ERROR) << "Tried to add invalid constraint. Cannot add inequality constraint.";
        return false;
    }
    
    m_constraints.push_back(
        std::static_pointer_cast<Constraint>(
            std::move(constraint)
        )
    );
    
    for (auto& pCell : cellPointers) {
        pCell.lock()->AddConstraint(m_constraints.back());
    }

    return true;
}

void ConstraintSatisfactionProblem::ReportIfCellNewlySolved() {
    ++m_numSolvedCells;
    
    assertm(m_numSolvedCells <= m_numCells, "number of solved cells should be less than the number cells");
    
    if (m_numSolvedCells == m_numCells) {
        m_completelySolved = true;
        VLOG(2) << "All cells solved";
    }
}

void ConstraintSatisfactionProblem::ReportIfConstraintNewlySolved() {
    ++m_numSolvedConstraints;
    
    // TODO: figure out a way to make this work
    // similar to ReportIfConstraintBecomesActive, we have to account for the
    // adding of already solved constraints upon construction
    // assertm(m_numSolvedConstraints <= m_constraints.size() + 1, "number of solved constraints should be less than the number of constraints");
    
    if (m_numSolvedConstraints == m_constraints.size()) {
        VLOG(2) << "All constraints solved";
    }
}

void ConstraintSatisfactionProblem::ReportIfConstraintBecomesActive() {
    // TODO: there must be a better way here: the +1 is necessary, as the when adding a constraint we call this before it is added to the vector
    // clearly the assertm can be made stricter after when not adding constraints for the first time
    assertm(m_numActiveConstraints < m_constraints.size() + 1, "can only have as many active constraints as there are constraints");
    ++m_numActiveConstraints;
}

void ConstraintSatisfactionProblem::ReportIfConstraintBecomesInactive() {
    assertm(m_numActiveConstraints > 0, "cannot have fewer than zero active constraints");
    --m_numActiveConstraints;

    if (m_numActiveConstraints == 0) {
        VLOG(2) << "No more active constraints.";
    }
}

ConstraintSatisfactionProblem::SolveSolution ConstraintSatisfactionProblem::DeterministicSolve() {
    VLOG(1) << "Starting deterministic solve ";
    while(m_numActiveConstraints > 0) {
        for (auto& constraint : m_constraints) {
            if (constraint->IsActive()) {
                if(!constraint->Apply()) {
                    LOG(WARNING) << "Constraint turned out to be invalid";
                    return {
                        false,
                        false,
                        {SolveSolution::ReasonType::ConstraintCannotBeSatisfied, constraint->Serialize()}
                    }; // invalid
                }
            }
            else if (constraint->ShouldStillCheckValid()) {
                if(!constraint->Valid()) {
                    LOG(WARNING) << "Constraint turned out to be invalid";
                    return {
                        false,
                        false,
                        {SolveSolution::ReasonType::ConstraintCannotBeSatisfied, constraint->Serialize()}
                    }; // invalid
                }
                constraint->SetChecked();
            }
        }
    }
    VLOG(1) << "Finished deterministic solve (" << (m_completelySolved ? "SOLVED" : "UNSOLVED") << ")";
    m_provenValid = true;
    return {m_completelySolved, m_provenValid, {SolveSolution::ReasonType::ManagedToSolve} }; // valid
}

ConstraintSatisfactionProblem::SolveSolution ConstraintSatisfactionProblem::Solve(int depthGuess) {
    
    if (depthGuess == 0) {
        LOG(INFO) << "Solving...";
    }
    
    auto res = DeterministicSolve();
    if (!res.valid) {
        LOG(INFO) << "Finished solving. Not valid";
        return res;
    }
    if (res.completeSolve) {
        LOG(INFO) << "Finished solving. Found solution.";
        return res;
    }

    LOG(INFO) << "Require guess. Depth to " << depthGuess + 1;
    auto guesses = GetGuesses();
    for (auto guess : guesses) {
        auto branchCsp(*this);
        branchCsp.MakeGuess(guess);
        auto res = branchCsp.Solve(depthGuess + 1);
        if (res.completeSolve) {
            *this = branchCsp;
            return res;
        }
    }
    
    crow::json::wvalue reasonJson;
    size_t numGuesses = guesses.size();
    for (unsigned int i = 0; i < numGuesses; ++i) {
        reasonJson[i] = guesses[i].Serialize();
    }
    
    LOG(INFO) << "No quesses worked. Proven invalid.";    
    return {
        false,
        false,
        {SolveSolution::ReasonType::NoGuessesWorked, std::move(reasonJson) }
    };
}

ConstraintSatisfactionProblem::SolveSolution ConstraintSatisfactionProblem::SolveUnique(int depthGuess) {
    
    if (depthGuess == 0) {
        LOG(INFO) << "Solving uniquely...";
    }
    
    auto res = DeterministicSolve();
    if (!res.valid) {
        if (depthGuess == 0) {
            LOG(INFO) << "Finished solving. Not valid";
        }
        return res;
    }
    if (res.completeSolve) {
        if (depthGuess == 0) {
            LOG(INFO) << "Finished solving. Found solution.";
        }
        return res;
    }
    
    ConstraintSatisfactionProblem lastValidSolution;
    bool alreadyHaveAValidSolution = false;
    
    auto guesses = GetGuesses();
    
    LOG(INFO) << "Require guess. Depth to " << depthGuess + 1;
    for (auto guess : guesses) {
        auto branchCsp(*this);
        branchCsp.MakeGuess(guess);
        auto res = branchCsp.SolveUnique(depthGuess + 1);
        if (!res.valid) { // the CSP with the guess does not have unique solution
            return res;
        }
        if (res.completeSolve) {
            LOG(INFO) << "Guess produced solution";
            if (alreadyHaveAValidSolution) {
                LOG(INFO) << "Found two solutions. Not unique";
                crow::json::wvalue reasonJson;
                reasonJson[0] = Serialize();
                reasonJson[1] = lastValidSolution.Serialize();
                return {
                    false,
                    false,
                    {SolveSolution::ReasonType::NotUnique, std::move(reasonJson) }
                };
            }
            lastValidSolution = branchCsp;
            alreadyHaveAValidSolution = true;
        }
    }
    
    if (alreadyHaveAValidSolution) {
        *this = lastValidSolution;
        LOG(INFO) << "Found only one solution. Proven unique";
        return { true, true, {SolveSolution::ReasonType::ManagedToSolve} };
    }
    
    LOG(INFO) << "No quesses worked. Proven invalid.";
    crow::json::wvalue reasonJson;
    size_t numGuesses = guesses.size();
    for (unsigned int i = 0; i < numGuesses; ++i) {
        reasonJson[i] = guesses[i].Serialize();
    }
    return {
        false,
        false,
        {SolveSolution::ReasonType::NoGuessesWorked, std::move(reasonJson) }
    };
}

void ConstraintSatisfactionProblem::MakeGuess(const Guess& guess) {
    m_cells.at(guess.cellKey)->SetVal(guess.val);
}

// our heuristic here is to just chose all of the possible values
// for the cell with the lowest number of possible values
std::vector<ConstraintSatisfactionProblem::Guess> ConstraintSatisfactionProblem::GetGuesses() const {
    
    auto remainingCellKeys = RemainingCellKeys();
    
    std::sort(
        remainingCellKeys.begin(),
        remainingCellKeys.end(),
        [&](unsigned int i, unsigned int j){
            return m_cells.at(i)->GetPossibleValuesRef().size() > m_cells.at(j)->GetPossibleValuesRef().size();
        }
    );
    
    auto chosenCellKey = remainingCellKeys.front();
    auto& possibleVals =m_cells.at(chosenCellKey)->GetPossibleValuesRef();
    
    std::vector<Guess> outGuesses(possibleVals.size());
    std::transform(
        possibleVals.begin(),
        possibleVals.end(),
        outGuesses.begin(),
        [chosenCellKey](const int possibleVal) -> Guess {
            return {chosenCellKey, possibleVal};
        }
    );
    
    return outGuesses;
}

std::vector<unsigned long> ConstraintSatisfactionProblem::RemainingCellKeys() const {
    std::vector<unsigned long> remainingCellKeys;
    std::for_each(
        m_cells.begin(),
        m_cells.end(),
        [&remainingCellKeys](const std::pair< const unsigned long, std::shared_ptr<Cell> >& pair){
            if(!pair.second->IsSolved()) {
                remainingCellKeys.push_back(pair.first);
        }
    });
    
    return remainingCellKeys;
}

crow::json::wvalue ConstraintSatisfactionProblem::Serialize() const {
    return SerializeCsp();
}

crow::json::wvalue ConstraintSatisfactionProblem::SerializeCsp() const {
    auto out = crow::json::wvalue();
    
    out["num_cells"] = m_cells.size();
    out["cells"] = SerializeCells();
    out["constraints"] = SerializeConstraints();
    
    return out;
}

std::vector<crow::json::wvalue> ConstraintSatisfactionProblem::SerializeCells() const {
    std::vector<crow::json::wvalue> cellsArray;
    std::transform(
        m_cells.cbegin(),
        m_cells.cend(),
        back_inserter(cellsArray),
        [](const std::pair< const unsigned long, std::shared_ptr<Cell> >& pair) {
            return pair.second->Serialize();
        }
    );
    return cellsArray;
}

std::vector<crow::json::wvalue> ConstraintSatisfactionProblem::SerializeConstraints() const {
    std::vector<crow::json::wvalue> constraintsArray;
    std::transform(
        m_constraints.cbegin(),
        m_constraints.cend(),
        back_inserter(constraintsArray),
        [](const std::shared_ptr<Constraint>& pConstraints) {
            return pConstraints->Serialize();
        }
    );
    return constraintsArray;
}

} // ::Csp


