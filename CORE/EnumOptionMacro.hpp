//
//  EnumOptionMacro.h
//  PixFight
//
//  Created by Marcin Małysz on 03/01/2018.
//  Copyright © 2018 Marcin Małysz. All rights reserved.
//

#ifndef EnumOptionMacro_h
#define EnumOptionMacro_h

#include <iostream>

#ifndef __has_feature      // Optional.
#define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif

#ifndef __has_extension
#define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif

#if (__cplusplus && __cplusplus >= 201103L && (__has_extension(cxx_strong_enums) || __has_feature(objc_fixed_enum))) || (!__cplusplus && __has_feature(objc_fixed_enum))
#define PF_ENUM(_type, _name) enum _name : _type _name; enum _name : _type
#if (__cplusplus)
#define PF_OPTIONS(_type, _name) _type _name; enum : _type
#else
#define PF_OPTIONS(_type, _name) enum _name : _type _name; enum _name : _type
#endif
#else
#define PF_ENUM(_type, _name) _type _name; enum
#define PF_OPTIONS(_type, _name) _type _name; enum
#endif

#endif /* EnumOptionMacro_h */
