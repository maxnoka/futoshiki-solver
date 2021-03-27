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
    
protected:
    unsigned long m_size;
}; // SquareCsp

} // ::Csp

#endif /* SquareCsp_hpp */
