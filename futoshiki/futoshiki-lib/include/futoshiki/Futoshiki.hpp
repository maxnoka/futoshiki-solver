//
//  Futoshiki.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 09/04/2021.
//

#ifndef Futoshiki_hpp
#define Futoshiki_hpp

#include "LatinSquare.hpp"

namespace Csp {

class Futoshiki : public LatinSquare {
public:
    Futoshiki(
        const std::vector< std::vector<int> >& initValues
    )
    : LatinSquare(initValues)
    { };
    
    bool AddRandomConstraint(
        const Futoshiki& refSolution
    );
    
    virtual SolveSolution Generate() override;
    
}; // LatinSquare

} // ::Csp

#endif /* Futoshiki_hpp */
