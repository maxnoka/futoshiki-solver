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
    std::cout << "reporting became inactive\n";
    m_csp->ReportIfConstraintBecomesInactive();
}

void Constraint::ReportBecameActive() const {
    std::cout << "reporting became active\n";
    m_csp->ReportIfConstraintBecomesActive();
}

} // ::Csp
