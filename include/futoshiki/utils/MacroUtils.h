//
//  MacroUtils.h
//  futoshiki
//
//  Created by Maximilian Noka on 12/03/2021.
//

#ifndef MacroUtils_h
#define MacroUtils_h

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#endif /* MacroUtils_h */
