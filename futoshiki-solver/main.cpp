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
        /*
        std::vector<std::vector<int>> initialValues =
        {   {0, 0, 0, 0},
            {0, 0, 0, 2},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        };
        
        FutoshikiGame game(initialValues);
        
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(0, 0), std::make_tuple(0, 1),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(0, 2), std::make_tuple(0, 3),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(1, 1), std::make_tuple(1, 2),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(2, 2), std::make_tuple(2, 3),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 0), std::make_tuple(3, 1),
                           ConstraintDirection::Right);
        */
        
        std::vector<std::vector<int>> initialValues =
        {   {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 4},
        };
        
        FutoshikiGame game(initialValues);
        
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(0, 2), std::make_tuple(0, 3),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(0, 3), std::make_tuple(0, 4),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(1, 2), std::make_tuple(1, 3),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(2, 0), std::make_tuple(3, 0),
                           ConstraintDirection::Down);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(2, 1), std::make_tuple(2, 2),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(2, 4), std::make_tuple(3, 4),
                           ConstraintDirection::Down);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(3, 0), std::make_tuple(3, 1),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(3, 1), std::make_tuple(3, 2),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 1), std::make_tuple(4, 1),
                           ConstraintDirection::Down);
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 3), std::make_tuple(4, 3),
                           ConstraintDirection::Down);
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 4), std::make_tuple(3, 5),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(4, 0), std::make_tuple(4, 1),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(4, 4), std::make_tuple(4, 5),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(5, 0), std::make_tuple(5, 1),
                           ConstraintDirection::Right);
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(5, 1), std::make_tuple(5, 2),
                           ConstraintDirection::Right);
        
        
        /*
        std::vector<std::vector<int>> initialValues =
        {   {0, 0},
            {0, 0},
        };
        FutoshikiGame game(initialValues);
        */
        
        game.printBoard();
        game.solve();
        game.printBoard();
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
