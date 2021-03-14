//
//  main.cpp
//  futoshiki
//
//  Created by Maximilian Noka on 10/03/2021.
//

#include "ConstraintSatisfactionProblem.hpp"
#include "InequalityConstraint.hpp"
#include "EqualityConstraint.hpp"
#include "Cell.hpp"

#include <iostream>
#include <optional>

int main(int argc, const char * argv[]) {
    /*
    auto csp = Csp::ConstraintSatisfactionProblem({2, 3, Csp::Cell::kUnsolvedSymbol}, {1, 2, 3, 4}) ;
    csp.AddEqualityConstraint({0, 1}, Csp::EqualityConstraint::EqualityOperator::NotEqualTo);
    csp.dPrint();
    */
    
    auto csp = Csp::ConstraintSatisfactionProblem({Csp::Cell::kUnsolvedSymbol, Csp::Cell::kUnsolvedSymbol, Csp::Cell::kUnsolvedSymbol, 1, 2, 3}, {1, 2, 3});
    csp.AddEqualityConstraint({0, 1, 5}, Csp::EqualityConstraint::EqualityOperator::NotEqualTo); // 0, 1, are not equal to 3 => (1, 2)
    csp.AddInequalityConstraint(0, Csp::InequalityConstraint::InequalityOperator::GreaterThan, 1);
    csp.AddEqualityConstraint({0, 1, 2}, Csp::EqualityConstraint::EqualityOperator::NotEqualTo);
    
    auto res = csp.DeterministicSolve();
    csp.dPrint(true);
    
    return 0;
}
