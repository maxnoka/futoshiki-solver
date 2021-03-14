//
//  Constraint.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "Constraint.hpp"
#include "ConstraintSatisfactionProblem.hpp"

namespace Csp {

void Constraint::ReportBecameInactive() const {
    assertm(!IsActive(), "reporting became inactive, but still active\n");
#if DEBUG_CSP
    std::cout << "con_" << m_id << " reporting became inactive\n";
#endif // DEBUG_CSP
    m_csp->ReportIfConstraintBecomesInactive();
}

void Constraint::ReportBecameActive() const {
    assertm(IsActive(), "reporting became active, but still inactive\n");
#if DEBUG_CSP
    std::cout << "con_" << m_id << " reporting became active\n";
#endif // DEBUG_CSP
    m_csp->ReportIfConstraintBecomesActive();
}

} // ::Csp
