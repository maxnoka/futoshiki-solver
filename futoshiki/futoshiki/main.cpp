//
//  main.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "ConstraintSatisfactionProblem.hpp"
#include "InequalityConstraint.hpp"

#include <iostream>
#include <optional>

int main(int argc, const char * argv[]) {
    std::optional<Csp::ConstraintSatisfactionProblem> csp2;
    auto csp = Csp::ConstraintSatisfactionProblem({1, 2, 3}, {1, 2, 3, 4}) ;
    csp.AddInequalityConstraint(0, Csp::InequalityConstraint::InequalityOperator::LessThan, 1);
    csp.AddInequalityConstraint(0, Csp::InequalityConstraint::InequalityOperator::GreaterThan, 1);
    csp2 = csp;
    
    csp2.value().dPrint();
    
    return 0;
}
