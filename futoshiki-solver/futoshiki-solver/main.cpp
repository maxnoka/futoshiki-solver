//
//  main.cpp
//  futoshiki-solver
//
//  Created by Maximilian Noka on 01/11/2020.
//

#include <iostream>
#include <vector>
#include <sstream>

#include "FutoshikiGame.hpp"
#include "Constraint.hpp"

int main(int argc, const char * argv[]) {
    
    if (argc != 3) {
        return 1;
    }
    
    std::string board = argv[1];
    std::string constraints = argv[2];
    
    FutoshikiGame game(game.deserializeGrid(board));
    game.addInequalityConstraints(constraints);
    if (game.isValid()) {
        game.solve();
    }
    else {
        std::cout << "not valid";
        return 2;
    }
    std::cout << game.serializeGrid();
    
    return 0;
}

int main2(int argc, const char * argv[]) {
    try {
        
        std::vector<std::vector<int>> initialValues =
        {   {0, 0, 0, 0},
            {0, 0, 0, 2},
            {0, 0, 0, 0},
            {0, 0, 0, 0},
        };
        
        FutoshikiGame game(initialValues);
        
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(0, 0), std::make_tuple(0, 1));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(0, 2), std::make_tuple(0, 3));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(1, 1), std::make_tuple(1, 2));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(2, 2), std::make_tuple(2, 3));
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 0), std::make_tuple(3, 1));
        
        
        /*
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
                      std::make_tuple(0, 2), std::make_tuple(0, 3));
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(0, 3), std::make_tuple(0, 4));
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(1, 2), std::make_tuple(1, 3));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(2, 0), std::make_tuple(3, 0));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(2, 1), std::make_tuple(2, 2));
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(2, 4), std::make_tuple(3, 4));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(3, 0), std::make_tuple(3, 1));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(3, 1), std::make_tuple(3, 2));
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 1), std::make_tuple(4, 1));
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 3), std::make_tuple(4, 3));
        game.addInequalityConstraint(ConstraintOperator::isGreaterThan_CO,
                      std::make_tuple(3, 4), std::make_tuple(3, 5));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(4, 0), std::make_tuple(4, 1));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(4, 4), std::make_tuple(4, 5));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(5, 0), std::make_tuple(5, 1));
        game.addInequalityConstraint(ConstraintOperator::isLessThan_CO,
                      std::make_tuple(5, 1), std::make_tuple(5, 2));
        */
        
        /*
        std::vector<std::vector<int>> initialValues =
        {   {0, 0},
            {0, 0},
        };
        FutoshikiGame game(initialValues);
        */
        
        std::cout << game.serializeGrid();
        std::cout << game.serializeConstraints();
        
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
