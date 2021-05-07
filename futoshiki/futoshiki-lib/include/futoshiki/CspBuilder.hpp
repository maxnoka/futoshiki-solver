//
//  CspBuilder.hpp
//  futoshiki
//
//  Created by Maximilian Noka on 07/05/2021.
//

#ifndef CspBuilder_hpp
#define CspBuilder_hpp

#include "Futoshiki.hpp"

namespace Csp {

Futoshiki MakeFutoshikiFromJson(const crow::json::rvalue& json);

}

#endif /* CspBuilder_hpp */
