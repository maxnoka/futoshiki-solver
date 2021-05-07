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
    
    Futoshiki(unsigned long size)
        : LatinSquare(size)
    { };
    
    Futoshiki(
        std::vector< std::vector<Cell> >&& initCells
    )
        : LatinSquare(std::move(initCells))
    { };
    
    bool AddRandomConstraint(
        const Futoshiki& refSolution
    );
    
    static Futoshiki Generate(unsigned long size);
}; // LatinSquare

} // ::Csp

#endif /* Futoshiki_hpp */
