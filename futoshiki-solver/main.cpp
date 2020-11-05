//
//  main.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include <iostream>
#include <vector>

#include "FutoshikiGame.hpp"
#include "Constraint.hpp"

int main(int argc, const char * argv[]) {
    try {
        std::vector<std::vector<int>> initialValues = {{1, 2, 3}, {4, 5, 0}, {7, 8, 9}};
        
        FutoshikiGame game(initialValues);
        
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(0, 0), std::make_tuple(0, 1),
                           ConstraintDirection::Right);
        
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(0, 0), std::make_tuple(1, 0),
                           ConstraintDirection::Down);
        
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(1, 1), std::make_tuple(1, 2),
                           ConstraintDirection::Down);
        
        game.printBoard();
        game.debugPrint();
        return 0;
    }
    catch (std::runtime_error& e) {
        std::cerr << "runtime error: " << e.what() << std::endl;
        return 1;
    }
}

// todo equality constraints
// inferring of constraint direction (although not so important tbf)
// 
