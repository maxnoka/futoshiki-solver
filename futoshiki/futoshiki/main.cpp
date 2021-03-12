//
//  main.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "ConstraintSatisfactionProblem.hpp"
#include "InequalityConstraint.hpp"

#include <iostream>

Csp::ConstraintSatisfactionProblem foo() {
    auto csp = Csp::ConstraintSatisfactionProblem({1, 2, 3});
    
    csp.AddInequalityConstraint(1, Csp::InequalityConstraint::InequalityOperator::LessThan, 2);

    int a = 5;
    std::cout << a;
    
    return csp;
}

int main(int argc, const char * argv[]) {
    auto csp = foo();
    
    csp.dPrint();
    
    return 0;
}
