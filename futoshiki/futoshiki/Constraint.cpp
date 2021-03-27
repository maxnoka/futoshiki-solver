//
//  Constraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "Constraint.hpp"
#include "ConstraintSatisfactionProblem.hpp"

#include "utils/easylogging++.h"

namespace Csp {

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

} // ::Csp
