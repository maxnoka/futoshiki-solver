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
    auto csp = Csp::ConstraintSatisfactionProblem({2, 3, Csp::Cell::kUnsolvedSymbol}, {1, 2, 3}) ;
    csp.AddInequalityConstraint(2, Csp::InequalityConstraint::InequalityOperator::LessThan, 1);
    csp.dPrint();
    csp.Solve(false);
    std::cout << "After Solve\n";
    csp.dPrint();
    
    return 0;
}
