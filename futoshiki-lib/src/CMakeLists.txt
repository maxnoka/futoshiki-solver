set( HEADER_LIST 
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/Cell.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/Constraint.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/ConstraintSatisfactionProblem.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/CspBuilder.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/CspSolver.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/EqualityConstraint.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/Futoshiki.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/InequalityConstraint.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/LatinSquare.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/SquareCsp.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/TwoDimCsp.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/utils/MacroUtils.h"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/utils/Utils.hpp"
  "${Futoshiki_SOURCE_DIR}/include/futoshiki/utils/easylogging++.h"
)

set( SOURCES_LIST 
  "${Futoshiki_SOURCE_DIR}/src/Cell.cpp"
  "${Futoshiki_SOURCE_DIR}/src/Constraint.cpp"
  "${Futoshiki_SOURCE_DIR}/src/ConstraintSatisfactionProblem.cpp"
  "${Futoshiki_SOURCE_DIR}/src/CspBuilder.cpp"
  "${Futoshiki_SOURCE_DIR}/src/CspSolver.cpp"
  "${Futoshiki_SOURCE_DIR}/src/EqualityConstraint.cpp"
  "${Futoshiki_SOURCE_DIR}/src/Futoshiki.cpp"
  "${Futoshiki_SOURCE_DIR}/src/InequalityConstraint.cpp"
  "${Futoshiki_SOURCE_DIR}/src/LatinSquare.cpp"
  "${Futoshiki_SOURCE_DIR}/src/SquareCsp.cpp"
  "${Futoshiki_SOURCE_DIR}/src/TwoDimCsp.cpp"
  "${Futoshiki_SOURCE_DIR}/src/utils/utils.cpp"
  "${Futoshiki_SOURCE_DIR}/src/utils/easylogging++.cc"
)

add_library(futoshiki_library ${SOURCES_LIST} ${HEADER_LIST})
target_include_directories(futoshiki_library PUBLIC ../include)
target_link_libraries(futoshiki_library LINK_PUBLIC crow_library)
target_compile_features(futoshiki_library PRIVATE cxx_std_17)

# for IDEs
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST}
)
