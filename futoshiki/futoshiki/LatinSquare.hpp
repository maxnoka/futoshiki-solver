//
//  LatinSquare.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 27/03/2021.
//

#ifndef LatinSquare_hpp
#define LatinSquare_hpp

#include "SquareCsp.hpp"

namespace Csp {

class LatinSquare : public SquareCsp {
public:
    LatinSquare(
        const std::vector< std::vector<int> >& initValues
    );
    
    virtual crow::json::wvalue Serialize() const override;
    
}; // LatinSquare

} // ::Csp

#endif /* LatinSquare_hpp */
