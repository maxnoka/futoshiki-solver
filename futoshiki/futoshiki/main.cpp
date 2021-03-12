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
    {
        auto csp = Csp::ConstraintSatisfactionProblem({1, 2, 3});
        csp.AddInequalityConstraint(1, Csp::InequalityConstraint::InequalityOperator::LessThan, 2);
        csp2 = csp;
    }
    
    // csp2.value().dPrint();
    
    return 0;
}
