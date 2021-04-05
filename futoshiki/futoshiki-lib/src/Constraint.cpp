//
//  Constraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include <futoshiki/Constraint.hpp>
#include <futoshiki/ConstraintSatisfactionProblem.hpp>

#include <futoshiki/utils/easylogging++.h>

namespace Csp {

std::string Constraint::dPrint(bool log) const {
    std::stringstream ss;
    if (m_provenInvalid) {
        ss << "INVALID ";
    }
    else {
        ss << (m_solved ? "SOLVED" : "NOT SOLVED")
           << (m_relatedCellsChanged ? "* " : "  ");
    }
    ss << m_id << ": ";
    
    if (log) {
        VLOG(1) << ss.str();
    }
    return ss.str();
}

void Constraint::ReportBecameInactive() const {
    assertm(!IsActive(), "reporting became inactive, but still active");
    VLOG(2) << "con_" << m_id << " reporting became inactive";
    m_csp->ReportIfConstraintBecomesInactive();
}

void Constraint::ReportBecameActive() const {
    assertm(IsActive(), "reporting became active, but still inactive");
    VLOG(2) << "con_" << m_id << " reporting became active";
    m_csp->ReportIfConstraintBecomesActive();
}

void Constraint::ReportChanged() {
    assertm(!m_solved, "cells changed, but constraint was already marked as solved\n");
    const bool previouslyActive = IsActive();
    m_relatedCellsChanged = true;
    if (SetSolvedIfPossible()) {
        if (previouslyActive) {
            ReportBecameInactive();
        }
    }
    else {
        if(!previouslyActive) {
            ReportBecameActive();
        }
    }
}

} // ::Csp
