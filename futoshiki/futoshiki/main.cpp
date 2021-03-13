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
    auto csp = Csp::ConstraintSatisfactionProblem({2, 3, Csp::Cell::kUnsolvedSymbol}, {1, 2, 3, 4}) ;
    csp.AddInequalityConstraint(2, Csp::InequalityConstraint::InequalityOperator::LessThan, 0);
    csp.dPrint();
    csp.Solve(false);
    std::cout << "After Solve\n";
    csp.dPrint();
    
    auto csp2 = csp;
    csp2.dPrint();
    
    return 0;
}
