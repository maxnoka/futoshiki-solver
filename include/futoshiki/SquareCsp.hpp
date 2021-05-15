//
//  SquareCsp.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 27/03/2021.
//

#ifndef SquareCsp_hpp
#define SquareCsp_hpp

#include "TwoDimCsp.hpp"

namespace Csp {

class SquareCsp : public TwoDimCsp {
public:
    SquareCsp(
        const std::vector< std::vector<int> >& initValues,
        const std::set<int>& defaultPossibleValues
    );
    
    SquareCsp(
        std::vector< std::vector<Cell> >&& initCells
    );
    
    SquareCsp(
        unsigned long size,
        const std::set<int>& defaultPossibleValues
    );
    
    virtual crow::json::wvalue Serialize() const override;
    
protected:
    unsigned long m_size;
}; // SquareCsp

} // ::Csp

#endif /* SquareCsp_hpp */
